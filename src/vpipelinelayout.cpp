#include "vpipelinelayout.h"

#include "vdescriptorsetlayout.h"

#include <stdexcept>

namespace V {

PipelineLayoutBuilder::PipelineLayoutBuilder(const Device *device)
    : m_device(device)
{
}

PipelineLayoutBuilder &PipelineLayoutBuilder::addSetLayout(const DescriptorSetLayout *setLayout)
{
    m_setLayouts.push_back(setLayout->handle());
    return *this;
}

std::unique_ptr<PipelineLayout> PipelineLayoutBuilder::create() const
{
    VkPipelineLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(m_setLayouts.size()),
        .pSetLayouts = m_setLayouts.empty() ? nullptr : m_setLayouts.data(),
    };
    return std::make_unique<PipelineLayout>(m_device, createInfo);
}

PipelineLayout::PipelineLayout(const Device *device, const VkPipelineLayoutCreateInfo &createInfo)
    : m_device(device)
{
    if (vkCreatePipelineLayout(m_device->device(), &createInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout");
}

PipelineLayout::~PipelineLayout()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(m_device->device(), m_handle, nullptr);
}

} // namespace V
