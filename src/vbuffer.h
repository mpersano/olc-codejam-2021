#pragma once

#include "vdevice.h"

namespace V {

class Buffer : private NonCopyable
{
public:
    explicit Buffer(const Device *device, VkDeviceSize size);
    ~Buffer();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkDeviceSize size() const { return m_size; }

    VkBuffer handle() const { return m_handle; }

    void bindMemory(const Memory *memory, VkDeviceSize offset) const;

private:
    const Device *m_device;
    VkDeviceSize m_size;
    VkBuffer m_handle;
};

} // namespace V
