#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

#include <memory>

struct GLFWwindow;

namespace V {

class Surface;
class CommandPool;
class ShaderModule;
class Semaphore;

class Device : private NonCopyable
{
public:
    Device();
    ~Device();

    VkInstance instance() const { return m_instance; }
    VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
    uint32_t queueFamilyIndex() const { return m_queueFamilyIndex; }
    VkDevice device() const { return m_device; }
    VkQueue queue() const { return m_queue; }

    std::unique_ptr<Surface> createSurface(GLFWwindow *window) const;
    std::unique_ptr<Semaphore> createSemaphore() const;
    std::unique_ptr<CommandPool> createCommandPool() const;
    std::unique_ptr<ShaderModule> createShaderModule(const char *spvFilePath) const;

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

} // namespace V
