#pragma once

#include "vdevice.h"

#include <vector>

namespace V {

class DescriptorSetLayout;

class DescriptorSetLayoutBuilder
{
public:
    explicit DescriptorSetLayoutBuilder(const Device *device);

    DescriptorSetLayoutBuilder &addBinding(uint32_t binding, VkDescriptorType descriptorType);

    std::unique_ptr<DescriptorSetLayout> create() const;

private:
    const Device *m_device;
    std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;
};

class DescriptorSetLayout : private NonCopyable
{
public:
    explicit DescriptorSetLayout(const Device *device, const VkDescriptorSetLayoutCreateInfo &createInfo);
    ~DescriptorSetLayout();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkDescriptorSetLayout handle() const { return m_handle; }

private:
    const Device *m_device;
    VkDescriptorSetLayout m_handle = VK_NULL_HANDLE;
};

} // namespace V
