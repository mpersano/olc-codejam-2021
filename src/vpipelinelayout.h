#pragma once

#include "vdevice.h"

#include <vector>

namespace V {

class PipelineLayout;
class DescriptorSetLayout;

class PipelineLayoutBuilder
{
public:
    explicit PipelineLayoutBuilder(const Device *device);

    PipelineLayoutBuilder &addSetLayout(const DescriptorSetLayout *setLayout);

    std::unique_ptr<PipelineLayout> create() const;

private:
    const Device *m_device;
    std::vector<VkDescriptorSetLayout> m_setLayouts;
};

class PipelineLayout : private NonCopyable
{
public:
    explicit PipelineLayout(const Device *device, const VkPipelineLayoutCreateInfo &createInfo);
    ~PipelineLayout();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkPipelineLayout handle() const { return m_handle; }

private:
    const Device *m_device;
    VkPipelineLayout m_handle = VK_NULL_HANDLE;
};

} // namespace V
