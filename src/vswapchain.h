#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace V {

class Device;
class Surface;
class Semaphore;

class Swapchain : private NonCopyable
{
public:
    Swapchain(const Surface *surface, int width, int height, int backbufferCount);
    ~Swapchain();

    uint32_t width() const { return m_width; }
    uint32_t height() const { return m_height; }
    uint32_t backbufferCount() const { return m_backbufferCount; }
    VkFormat format() const { return m_format; }
    VkSwapchainKHR swapchain() const { return m_swapchain; }
    const std::vector<VkImage> &images() const { return m_images; }
    const std::vector<VkImageView> &imageViews() const { return m_imageViews; }
    VkRenderPass renderPass() const { return m_renderPass; }
    const std::vector<VkFramebuffer> &framebuffers() const { return m_framebuffers; }

    uint32_t acquireNextImage(Semaphore *signalSemaphore) const;
    void queuePresent(uint32_t imageIndex, Semaphore *waitSemaphore) const;

private:
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void cleanup();

    const Surface *m_surface;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_backbufferCount;
    VkFormat m_format;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkRenderPass m_renderPass = VK_NULL_HANDLE; // XXX probably doesn't belong here
    std::vector<VkFramebuffer> m_framebuffers; // XXX probably doesn't belong here
};

} // namespace V
