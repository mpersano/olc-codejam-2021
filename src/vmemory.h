#pragma once

#include "vdevice.h"

namespace V {

class Memory : private NonCopyable
{
public:
    explicit Memory(const Device *device, const VkMemoryAllocateInfo &allocateInfo);
    ~Memory();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkDeviceSize size() const { return m_size; }

    VkDeviceMemory handle() const { return m_handle; }

    template<typename T>
    T *map(VkDeviceSize offset, VkDeviceSize size) const
    {
        T *buffer;
        if (vkMapMemory(m_device->device(), m_handle, offset, size, 0, reinterpret_cast<void **>(&buffer)) != VK_SUCCESS)
            throw std::runtime_error("Failed to map memory");
        return buffer;
    }

    template<typename T>
    T *map() const
    {
        return map<T>(0, m_size);
    }

    void unmap()
    {
        vkUnmapMemory(m_device->device(), m_handle);
    }

private:
    const Device *m_device;
    VkDeviceSize m_size;
    VkDeviceMemory m_handle = VK_NULL_HANDLE;
};

} // namespace V
