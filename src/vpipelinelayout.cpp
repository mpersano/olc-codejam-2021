#include "vpipelinelayout.h"

#include <stdexcept>

namespace V {

PipelineLayout::PipelineLayout(const Device *device)
    : m_device(device)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };
    if (vkCreatePipelineLayout(m_device->device(), &pipelineLayoutCreateInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout");
}

PipelineLayout::~PipelineLayout()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(m_device->device(), m_handle, nullptr);
}

} // namespace V
