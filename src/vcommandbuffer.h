#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace V {

class CommandPool;
class Pipeline;
class DescriptorSet;
class PipelineLayout;
class Buffer;

class CommandBuffer : private NonCopyable
{
public:
    CommandBuffer(const CommandPool *commandPool);
    ~CommandBuffer();

    VkCommandBuffer handle() const { return m_handle; }

    void begin() const;
    void beginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, VkRect2D renderArea) const;
    void bindPipeline(const Pipeline *pipeline) const;
    void bindVertexBuffers(const std::vector<const Buffer *> &buffers) const;
    void bindDescriptorSet(const PipelineLayout *pipelineLayout, const DescriptorSet *descriptorSet) const;
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const;
    void endRenderPass() const;
    void end() const;

private:
    const CommandPool *m_commandPool;
    VkCommandBuffer m_handle;
};

} // namespace V
