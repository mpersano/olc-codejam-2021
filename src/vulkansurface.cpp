#include "vulkansurface.h"

#include "vulkandevice.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

VulkanSurface::VulkanSurface(VulkanDevice *device, GLFWwindow *window)
    : m_device(device)
{
    if (glfwCreateWindowSurface(device->instance(), window, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create surface");

    VkBool32 presentSupported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device->physicalDevice(), 0, m_handle, &presentSupported);
    if (!presentSupported)
        throw std::runtime_error("Surface doesn't support presentation");

    std::cout << "m_surface=" << m_handle << '\n';
}

VulkanSurface::~VulkanSurface()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(m_device->instance(), m_handle, nullptr);
}

VkSurfaceCapabilitiesKHR VulkanSurface::surfaceCapabilities() const
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->physicalDevice(), m_handle, &surfaceCapabilities);
    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR> VulkanSurface::surfaceFormats() const
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->physicalDevice(), m_handle, &count, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->physicalDevice(), m_handle, &count, surfaceFormats.data());

    return surfaceFormats;
}

std::vector<VkPresentModeKHR> VulkanSurface::presentModes() const
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->physicalDevice(), m_handle, &count, nullptr);

    std::vector<VkPresentModeKHR> presentModes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->physicalDevice(), m_handle, &count, presentModes.data());

    return presentModes;
}
