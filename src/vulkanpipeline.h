#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

#include <vector>

class VulkanDevice;
class VulkanSwapchain;
class VulkanShaderModule;

class VulkanPipeline : private NonCopyable
{
public:
    VulkanPipeline(VulkanDevice *device, VulkanSwapchain *swapchain);
    ~VulkanPipeline();

    VkPipeline pipeline() const { return m_pipeline; }

    void addShaderStage(VkShaderStageFlagBits stage, VulkanShaderModule *module);
    void create();

private:
    VulkanDevice *m_device;
    VulkanSwapchain *m_swapchain;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};
