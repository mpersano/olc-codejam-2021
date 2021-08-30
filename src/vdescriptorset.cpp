#include "vdescriptorset.h"

#include "vbuffer.h"
#include "vdescriptorpool.h"
#include "vdescriptorsetlayout.h"

namespace V {

DescriptorSet::DescriptorSet(const DescriptorPool *descriptorPool, const DescriptorSetLayout *descriptorSetLayout)
    : m_descriptorPool(descriptorPool)
{
    VkDescriptorSetLayout descriptorSetLayoutHandle = descriptorSetLayout->handle();
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_descriptorPool->handle(),
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayoutHandle
    };

    if (vkAllocateDescriptorSets(m_descriptorPool->deviceHandle(), &descriptorSetAllocateInfo, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate descriptor sets");
}

DescriptorSet::~DescriptorSet() = default; // automatically freed by the pool

void DescriptorSet::writeBuffer(uint32_t binding, const Buffer *buffer) const
{
    VkDescriptorBufferInfo inputDescriptorBufferInfo = {
        .buffer = buffer->handle(),
        .offset = 0,
        .range = VK_WHOLE_SIZE
    };

    VkWriteDescriptorSet writeDescriptorSet = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = m_handle,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pBufferInfo = &inputDescriptorBufferInfo
    };

    vkUpdateDescriptorSets(m_descriptorPool->deviceHandle(), 1, &writeDescriptorSet, 0, nullptr);
}

} // namespace V
