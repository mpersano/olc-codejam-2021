#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

#include <memory>

struct GLFWwindow;

class VulkanSurface;
class VulkanCommandPool;
class VulkanShaderModule;
class VulkanSemaphore;

class VulkanDevice : private NonCopyable
{
public:
    VulkanDevice();
    ~VulkanDevice();

    VkInstance instance() const { return m_instance; }
    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    uint32_t queueFamilyIndex() const { return m_queueFamilyIndex; }
    VkDevice device() const { return m_device; }
    VkQueue queue() const { return m_queue; }

    std::unique_ptr<VulkanSurface> createSurface(GLFWwindow *window) const;
    std::unique_ptr<VulkanSemaphore> createSemaphore() const;
    std::unique_ptr<VulkanCommandPool> createCommandPool() const;
    std::unique_ptr<VulkanShaderModule> createShaderModule(const char *spvFilePath) const;

private:
    void createInstance();
    void createDeviceAndQueue();
    void cleanup();

    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    uint32_t m_queueFamilyIndex;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_queue = VK_NULL_HANDLE;
};
