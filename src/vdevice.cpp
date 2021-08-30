#include "vdevice.h"

#include "vbuffer.h"
#include "vcommandpool.h"
#include "vdescriptorpool.h"
#include "vdescriptorsetlayout.h"
#include "vmemory.h"
#include "vpipeline.h"
#include "vpipelinelayout.h"
#include "vsemaphore.h"
#include "vshadermodule.h"
#include "vsurface.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace V {

namespace {

std::vector<const char *> instanceExtensions()
{
    uint32_t extensionCount = 0;
    const char **extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
    return std::vector<const char *>(extensionNames, extensionNames + extensionCount);
}

std::vector<const char *> instanceLayers()
{
    return { "VK_LAYER_KHRONOS_validation" };
}

std::vector<const char *> deviceExtensions()
{
    return { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

} // namespace

Device::Device()
{
    createInstance();
    createDeviceAndQueue();
}

Device::~Device()
{
    cleanup();
}

void Device::createInstance()
{
    VkApplicationInfo applicationInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Hello",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Demo Engine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_0
    };

    const auto layers = instanceLayers();
    const auto extensions = instanceExtensions();

    VkInstanceCreateInfo instanceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.empty() ? nullptr : layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data()
    };

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance");
}

void Device::createDeviceAndQueue()
{
    std::tie(m_physicalDevice, m_queueFamilyIndex) = [this]() -> std::tuple<VkPhysicalDevice, uint32_t> {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());

        // find physical device with a graphics queue family

        for (const auto physicalDevice : physicalDevices) {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

            auto it = std::find_if(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties &queueFamily) {
                return queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
            });

            if (it != queueFamilies.end()) {
                return { physicalDevice, std::distance(queueFamilies.begin(), it) };
            }
        }
        return { VK_NULL_HANDLE, 0 };
    }();
    if (m_physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Could not find a physical device with a graphics queue");

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = m_queueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    const auto extensions = deviceExtensions();

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.empty() ? nullptr : extensions.data()
    };

    if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create device");

    vkGetDeviceQueue(m_device, m_queueFamilyIndex, 0, &m_queue);
}

void Device::cleanup()
{
    if (m_device != VK_NULL_HANDLE)
        vkDestroyDevice(m_device, nullptr);

    if (m_instance != VK_NULL_HANDLE)
        vkDestroyInstance(m_instance, nullptr);
}

std::unique_ptr<Surface> Device::createSurface(GLFWwindow *window) const
{
    return std::make_unique<Surface>(this, window);
}

std::unique_ptr<Semaphore> Device::createSemaphore() const
{
    return std::make_unique<Semaphore>(this);
}

std::unique_ptr<CommandPool> Device::createCommandPool() const
{
    return std::make_unique<CommandPool>(this);
}

std::unique_ptr<ShaderModule> Device::createShaderModule(const char *spvFilePath) const
{
    return std::make_unique<ShaderModule>(this, spvFilePath);
}

PipelineLayoutBuilder Device::pipelineLayoutBuilder() const
{
    return PipelineLayoutBuilder(this);
}

PipelineBuilder Device::pipelineBuilder() const
{
    return PipelineBuilder(this);
}

std::unique_ptr<Memory> Device::allocateMemory(VkDeviceSize size) const
{
    const int memoryTypeIndex = [this, size]() -> int {
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &physicalDeviceMemoryProperties);
        for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
            const VkMemoryType &memoryType = physicalDeviceMemoryProperties.memoryTypes[i];
            const VkMemoryHeap &memoryHeap = physicalDeviceMemoryProperties.memoryHeaps[memoryType.heapIndex];
            const auto propertyFlags = memoryType.propertyFlags;
            if ((propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && (propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) &&
                memoryHeap.size >= size) {
                return i;
            }
        }
        return -1;
    }();

    VkMemoryAllocateInfo memoryAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = size,
        .memoryTypeIndex = static_cast<uint32_t>(memoryTypeIndex)
    };
    return std::make_unique<Memory>(this, memoryAllocateInfo);
}

std::unique_ptr<Buffer> Device::createBuffer(VkDeviceSize size) const
{
    return std::make_unique<Buffer>(this, size);
}

DescriptorSetLayoutBuilder Device::descriptorSetLayoutBuilder() const
{
    return DescriptorSetLayoutBuilder(this);
}

DescriptorPoolBuilder Device::descriptorPoolBuilder() const
{
    return DescriptorPoolBuilder(this);
}

} // namespace V
