#pragma once

#include "noncopyable.h"
#include "vulkandevice.h"

#include <vulkan/vulkan.h>

#include <vector>

struct GLFWwindow;

class VulkanSwapchain;

class VulkanSurface : private NonCopyable
{
public:
    VulkanSurface(const VulkanDevice *device, GLFWwindow *window);
    ~VulkanSurface();

    const VulkanDevice *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkSurfaceKHR handle() const { return m_handle; }

    VkSurfaceCapabilitiesKHR surfaceCapabilities() const;
    std::vector<VkSurfaceFormatKHR> surfaceFormats() const;
    std::vector<VkPresentModeKHR> presentModes() const;

    std::unique_ptr<VulkanSwapchain> createSwapchain(int width, int height, int backbufferCount) const;

private:
    const VulkanDevice *m_device;
    VkSurfaceKHR m_handle = VK_NULL_HANDLE;
};
