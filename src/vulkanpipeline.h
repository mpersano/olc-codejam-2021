#pragma once

#include "noncopyable.h"
#include "vulkandevice.h"

#include <vulkan/vulkan.h>

#include <vector>

class VulkanSwapchain;
class VulkanShaderModule;

class VulkanPipeline : private NonCopyable
{
public:
    explicit VulkanPipeline(const VulkanDevice *device);
    ~VulkanPipeline();

    const VulkanDevice *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkPipeline handle() const { return m_pipeline; }

    void addShaderStage(VkShaderStageFlagBits stage, VulkanShaderModule *module);
    void create(const VulkanSwapchain *swapchain);

private:
    const VulkanDevice *m_device;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};
