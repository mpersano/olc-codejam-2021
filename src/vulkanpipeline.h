#pragma once

#include "noncopyable.h"
#include "vulkandevice.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace V {

class Swapchain;
class ShaderModule;

class Pipeline : private NonCopyable
{
public:
    explicit Pipeline(const Device *device);
    ~Pipeline();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkPipeline handle() const { return m_pipeline; }

    void addShaderStage(VkShaderStageFlagBits stage, ShaderModule *module);
    void create(const Swapchain *swapchain);

private:
    const Device *m_device;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};

} // namespace V
