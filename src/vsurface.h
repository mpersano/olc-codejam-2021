#pragma once

#include "vdevice.h"

#include <vector>

struct GLFWwindow;

namespace V {

class Swapchain;

class Surface : private NonCopyable
{
public:
    Surface(const Device *device, GLFWwindow *window);
    ~Surface();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkSurfaceKHR handle() const { return m_handle; }

    VkSurfaceCapabilitiesKHR surfaceCapabilities() const;
    std::vector<VkSurfaceFormatKHR> surfaceFormats() const;
    std::vector<VkPresentModeKHR> presentModes() const;

    std::unique_ptr<Swapchain> createSwapchain(int width, int height, int backbufferCount) const;

private:
    const Device *m_device;
    VkSurfaceKHR m_handle = VK_NULL_HANDLE;
};

} // namespace V
