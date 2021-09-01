#pragma once

#include "vdevice.h"

#include <vector>

namespace V {

class ShaderModule;
class Pipeline;
class PipelineLayout;

class PipelineBuilder
{
public:
    explicit PipelineBuilder(const Device *device);

    PipelineBuilder &addVertexInputBinding(uint32_t binding, uint32_t stride);
    PipelineBuilder &addVertexInputAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);
    PipelineBuilder &setViewport(uint32_t width, uint32_t height);
    PipelineBuilder &addShaderStage(VkShaderStageFlagBits stage, ShaderModule *module);

    std::unique_ptr<Pipeline> create(const PipelineLayout *layout, VkRenderPass renderPass) const;

private:
    const Device *m_device;
    std::vector<VkVertexInputBindingDescription> m_vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> m_vertexInputAttributes;
    VkViewport m_viewport;
    VkRect2D m_scissor;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
};

class Pipeline : private NonCopyable
{
public:
    explicit Pipeline(const Device *device, const VkGraphicsPipelineCreateInfo &createInfo);
    ~Pipeline();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkPipeline handle() const { return m_handle; }

private:
    const Device *m_device;
    VkPipeline m_handle = VK_NULL_HANDLE;
};

} // namespace V
