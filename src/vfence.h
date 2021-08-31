#pragma once

#include "vdevice.h"

namespace V {

class Fence : private NonCopyable
{
public:
    explicit Fence(const Device *device, bool createSignaled = false);
    ~Fence();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkFence handle() const { return m_handle; }

    void reset();
    void wait();

private:
    const Device *m_device;
    VkFence m_handle = VK_NULL_HANDLE;
};

} // namespace V
