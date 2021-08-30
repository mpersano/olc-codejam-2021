#include "vdescriptorpool.h"

#include "vdescriptorset.h"

namespace V {

DescriptorPoolBuilder::DescriptorPoolBuilder(const Device *device)
    : m_device(device)
{
}

DescriptorPoolBuilder &DescriptorPoolBuilder::add(VkDescriptorType type, uint32_t count)
{
    VkDescriptorPoolSize poolSize = {
        .type = type,
        .descriptorCount = count
    };
    m_poolSizes.push_back(poolSize);
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPoolBuilder::create() const
{

    VkDescriptorPoolCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1,
        .poolSizeCount = static_cast<uint32_t>(m_poolSizes.size()),
        .pPoolSizes = m_poolSizes.empty() ? nullptr : m_poolSizes.data()
    };

    return std::make_unique<DescriptorPool>(m_device, createInfo);
}

DescriptorPool::DescriptorPool(const Device *device, const VkDescriptorPoolCreateInfo &createInfo)
    : m_device(device)
{
    if (vkCreateDescriptorPool(m_device->device(), &createInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor pool");
}

DescriptorPool::~DescriptorPool()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(m_device->device(), m_handle, nullptr);
}

std::unique_ptr<DescriptorSet> DescriptorPool::allocateDescriptorSet(const DescriptorSetLayout *descriptorSetLayout) const
{
    return std::make_unique<DescriptorSet>(this, descriptorSetLayout);
}

} // namespace V
