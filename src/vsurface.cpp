#include "vsurface.h"

#include "vdevice.h"
#include "vswapchain.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

namespace V {

Surface::Surface(const Device *device, GLFWwindow *window)
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

Surface::~Surface()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(m_device->instance(), m_handle, nullptr);
}

VkSurfaceCapabilitiesKHR Surface::surfaceCapabilities() const
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device->physicalDevice(), m_handle, &surfaceCapabilities);
    return surfaceCapabilities;
}

std::vector<VkSurfaceFormatKHR> Surface::surfaceFormats() const
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->physicalDevice(), m_handle, &count, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->physicalDevice(), m_handle, &count, surfaceFormats.data());

    return surfaceFormats;
}

std::vector<VkPresentModeKHR> Surface::presentModes() const
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->physicalDevice(), m_handle, &count, nullptr);

    std::vector<VkPresentModeKHR> presentModes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_device->physicalDevice(), m_handle, &count, presentModes.data());

    return presentModes;
}

std::unique_ptr<Swapchain> Surface::createSwapchain(int width, int height, int backbufferCount) const
{
    return std::make_unique<Swapchain>(this, width, height, backbufferCount);
}

} // namespace V
