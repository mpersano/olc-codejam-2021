#pragma once

#include "noncopyable.h"
#include "vdevice.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace V {

class ShaderModule;

class Pipeline : private NonCopyable
{
public:
    explicit Pipeline(const Device *device);
    ~Pipeline();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkPipeline handle() const { return m_pipeline; }

    void setViewport(uint32_t width, uint32_t height);
    void addShaderStage(VkShaderStageFlagBits stage, ShaderModule *module);
    void create(VkRenderPass renderPass);

private:
    const Device *m_device;
    VkViewport m_viewport;
    VkRect2D m_scissor;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};

} // namespace V
