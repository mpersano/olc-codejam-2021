#include "vulkancommandbuffer.h"

#include "vulkancommandpool.h"
#include "vulkandevice.h"
#include "vulkanpipeline.h"

#include <stdexcept>

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice *device, VulkanCommandPool *commandPool)
    : m_device(device)
    , m_commandPool(commandPool)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_commandPool->handle(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    if (vkAllocateCommandBuffers(m_device->device(), &commandBufferAllocateInfo, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffer");
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    if (m_handle != VK_NULL_HANDLE)
        vkFreeCommandBuffers(m_device->device(), m_commandPool->handle(), 1, &m_handle);
}

void VulkanCommandBuffer::begin() const
{
    VkCommandBufferBeginInfo commandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
    };

    if (vkBeginCommandBuffer(m_handle, &commandBufferBeginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin command buffer");
}

void VulkanCommandBuffer::beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea) const
{
    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = framebuffer,
        .renderArea = renderArea,
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };
    vkCmdBeginRenderPass(m_handle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::bindPipeline(VulkanPipeline *pipeline) const
{
    vkCmdBindPipeline(m_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline());
}

void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const
{
    vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::endRenderPass() const
{
    vkCmdEndRenderPass(m_handle);
}

void VulkanCommandBuffer::end() const
{
    if (vkEndCommandBuffer(m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to end command buffer");
}
