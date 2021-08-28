#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

#include <vector>

class VulkanDevice;
struct GLFWwindow;

class VulkanSurface : private NonCopyable
{
public:
    VulkanSurface(VulkanDevice *device, GLFWwindow *window);
    ~VulkanSurface();

    VkSurfaceKHR handle() const { return m_handle; }

    VkSurfaceCapabilitiesKHR surfaceCapabilities() const;
    std::vector<VkSurfaceFormatKHR> surfaceFormats() const;
    std::vector<VkPresentModeKHR> presentModes() const;

private:
    VulkanDevice *m_device;
    VkSurfaceKHR m_handle = VK_NULL_HANDLE;
};
