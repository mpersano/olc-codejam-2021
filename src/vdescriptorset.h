#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

namespace V {

class DescriptorPool;
class DescriptorSetLayout;
class Buffer;

class DescriptorSet : private NonCopyable
{
public:
    DescriptorSet(const DescriptorPool *descriptorPool, const DescriptorSetLayout *descriptorSetLayout);
    ~DescriptorSet();

    VkDescriptorSet handle() const { return m_handle; }

    void writeBuffer(uint32_t binding, const Buffer *buffer) const;

private:
    const DescriptorPool *m_descriptorPool;
    VkDescriptorSet m_handle;
};

} // namespace V
