#include "vdescriptorsetlayout.h"

namespace V {

DescriptorSetLayoutBuilder::DescriptorSetLayoutBuilder(const Device *device)
    : m_device(device)
{
}

DescriptorSetLayoutBuilder &DescriptorSetLayoutBuilder::addBinding(uint32_t binding, VkDescriptorType descriptorType)
{
    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = binding,
        .descriptorType = descriptorType,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_ALL // FIXME
    };
    m_layoutBindings.push_back(layoutBinding);
    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayoutBuilder::create() const
{
    VkDescriptorSetLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(m_layoutBindings.size()),
        .pBindings = m_layoutBindings.empty() ? nullptr : m_layoutBindings.data()
    };
    return std::make_unique<DescriptorSetLayout>(m_device, createInfo);
}

DescriptorSetLayout::DescriptorSetLayout(const Device *device, const VkDescriptorSetLayoutCreateInfo &createInfo)
    : m_device(device)
{
    if (vkCreateDescriptorSetLayout(m_device->device(), &createInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor set layout");
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(m_device->device(), m_handle, nullptr);
}

} // namespace V
