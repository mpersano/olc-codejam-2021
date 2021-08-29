#pragma once

#include "vdevice.h"

namespace V {

class PipelineLayout : private NonCopyable
{
public:
    explicit PipelineLayout(const Device *device);
    ~PipelineLayout();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkPipelineLayout handle() const { return m_handle; }

private:
    const Device *m_device;
    VkPipelineLayout m_handle = VK_NULL_HANDLE;
};

} // namespace V
