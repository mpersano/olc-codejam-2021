#include "vulkanswapchain.h"

#include "vulkandevice.h"
#include "vulkansemaphore.h"
#include "vulkansurface.h"

#include <cassert>
#include <iostream>
#include <iterator>

VulkanSwapchain::VulkanSwapchain(const VulkanSurface *surface, int width, int height, int backbufferCount)
    : m_surface(surface)
    , m_width(width)
    , m_height(height)
    , m_backbufferCount(backbufferCount)
{
    createSwapchain();
    createImageViews();
    createRenderPass();
    createFramebuffers();
}

VulkanSwapchain::~VulkanSwapchain()
{
    cleanup();
}

void VulkanSwapchain::createSwapchain()
{
    const std::vector<VkSurfaceFormatKHR> surfaceFormats = m_surface->surfaceFormats();

    m_format = [&surfaceFormats] {
        if (surfaceFormats.size() == 1 && surfaceFormats.front().format == VK_FORMAT_UNDEFINED)
            return VK_FORMAT_R8G8B8A8_UNORM;
        else
            return surfaceFormats.front().format;
    }();
    VkColorSpaceKHR colorSpace = surfaceFormats.front().colorSpace;

    const VkSurfaceCapabilitiesKHR surfaceCapabilities = m_surface->surfaceCapabilities();

    const VkExtent2D swapchainSize = surfaceCapabilities.currentExtent;
    if (swapchainSize.width != m_width || swapchainSize.height != m_height)
        throw std::runtime_error("Unexpected current extent?");

    if (m_backbufferCount < surfaceCapabilities.minImageCount || (surfaceCapabilities.maxImageCount != 0 && m_backbufferCount > surfaceCapabilities.maxImageCount))
        throw std::runtime_error("Unsupported swapchain backbuffer count?");

    const VkSurfaceTransformFlagBitsKHR surfaceTransformFlags = [&surfaceCapabilities] {
        if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
            return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        else
            return surfaceCapabilities.currentTransform;
    }();

    // create swapchain

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = m_surface->handle(),
        .minImageCount = static_cast<uint32_t>(m_backbufferCount),
        .imageFormat = m_format,
        .imageColorSpace = colorSpace,
        .imageExtent = swapchainSize,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    if (vkCreateSwapchainKHR(m_surface->deviceHandle(), &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain");

    std::cout << "swapchain=" << m_swapchain << '\n';

    // get image handles

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(m_surface->deviceHandle(), m_swapchain, &imageCount, nullptr);
    assert(imageCount == m_backbufferCount);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_surface->deviceHandle(), m_swapchain, &imageCount, m_images.data());
}

void VulkanSwapchain::createImageViews()
{
    m_imageViews.resize(m_backbufferCount);
    std::fill(m_imageViews.begin(), m_imageViews.end(), static_cast<VkImageView>(VK_NULL_HANDLE));

    for (int i = 0; i < m_backbufferCount; ++i) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_format,
            .subresourceRange = VkImageSubresourceRange {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .levelCount = 1,
                    .layerCount = 1,
            }
        };
        if (vkCreateImageView(m_surface->deviceHandle(), &imageViewCreateInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create image view");
    }

    std::cout << "image views=";
    std::copy(m_imageViews.begin(), m_imageViews.end(), std::ostream_iterator<VkImageView>(std::cout, " "));
    std::cout << '\n';
}

void VulkanSwapchain::createRenderPass()
{
    VkAttachmentDescription attachmentDescription = {
        .format = m_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference attachmentReference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentReference,
    };

    VkRenderPassCreateInfo renderPassCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
    };

    if (vkCreateRenderPass(m_surface->deviceHandle(), &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass");

    std::cout << "render pass=" << m_renderPass << '\n';
}

void VulkanSwapchain::createFramebuffers()
{
    m_framebuffers.resize(m_backbufferCount);
    std::fill(m_framebuffers.begin(), m_framebuffers.end(), static_cast<VkFramebuffer>(VK_NULL_HANDLE));

    for (int i = 0; i < m_backbufferCount; ++i) {
        VkFramebufferCreateInfo framebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = m_renderPass,
            .attachmentCount = 1,
            .pAttachments = &m_imageViews[i],
            .width = m_width,
            .height = m_height,
            .layers = 1
        };

        if (vkCreateFramebuffer(m_surface->deviceHandle(), &framebufferCreateInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create framebuffer");
    }

    std::cout << "framebuffers=";
    std::copy(m_framebuffers.begin(), m_framebuffers.end(), std::ostream_iterator<VkFramebuffer>(std::cout, " "));
    std::cout << '\n';
}

void VulkanSwapchain::cleanup()
{
    for (auto framebuffer : m_framebuffers) {
        if (framebuffer != VK_NULL_HANDLE)
            vkDestroyFramebuffer(m_surface->deviceHandle(), framebuffer, nullptr);
    }

    if (m_renderPass != VK_NULL_HANDLE)
        vkDestroyRenderPass(m_surface->deviceHandle(), m_renderPass, nullptr);

    for (auto imageView : m_imageViews) {
        if (imageView != VK_NULL_HANDLE)
            vkDestroyImageView(m_surface->deviceHandle(), imageView, nullptr);
    }

    if (m_swapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(m_surface->deviceHandle(), m_swapchain, nullptr);
}

uint32_t VulkanSwapchain::acquireNextImage(VulkanSemaphore *semaphore) const
{
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_surface->deviceHandle(), m_swapchain, UINT64_MAX, semaphore->handle(), VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS) {
        // TODO handle swapchain re-creation
        throw std::runtime_error("Failed to acquire image");
    }
    return imageIndex;
}

void VulkanSwapchain::queuePresent(uint32_t imageIndex, VulkanSemaphore *semaphore) const
{
    VkSemaphore semaphoreHandle = semaphore->handle();
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &semaphoreHandle,
        .swapchainCount = 1,
        .pSwapchains = &m_swapchain,
        .pImageIndices = &imageIndex
    };
    VkResult result = vkQueuePresentKHR(m_surface->device()->queue(), &presentInfo);
    if (result != VK_SUCCESS) {
        // TODO handle swapchain re-creation
        throw std::runtime_error("Failed to queue image for presentation");
    }
}
