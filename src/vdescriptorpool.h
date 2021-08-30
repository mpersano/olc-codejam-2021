#pragma once

#include "vdevice.h"

#include <vector>

namespace V {

class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;

class DescriptorPoolBuilder
{
public:
    explicit DescriptorPoolBuilder(const Device *device);

    DescriptorPoolBuilder &add(VkDescriptorType type, uint32_t count);

    std::unique_ptr<DescriptorPool> create() const;

private:
    const Device *m_device;
    std::vector<VkDescriptorPoolSize> m_poolSizes;
};

class DescriptorPool : private NonCopyable
{
public:
    explicit DescriptorPool(const Device *device, const VkDescriptorPoolCreateInfo &createInfo);
    ~DescriptorPool();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkDescriptorPool handle() const { return m_handle; }

    std::unique_ptr<DescriptorSet> allocateDescriptorSet(const DescriptorSetLayout *descriptorSetLayout) const;

private:
    const Device *m_device;
    VkDescriptorPool m_handle = VK_NULL_HANDLE;
};

} // namespace V
