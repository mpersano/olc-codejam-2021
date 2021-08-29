#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanCommandPool;
class VulkanPipeline;

class VulkanCommandBuffer : private NonCopyable
{
public:
    VulkanCommandBuffer(const VulkanCommandPool *commandPool);
    ~VulkanCommandBuffer();

    VkCommandBuffer handle() const { return m_handle; }

    void begin() const;
    void beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea) const;
    void bindPipeline(const VulkanPipeline *pipeline) const;
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const;
    void endRenderPass() const;
    void end() const;

private:
    const VulkanCommandPool *m_commandPool;
    VkCommandBuffer m_handle;
};
