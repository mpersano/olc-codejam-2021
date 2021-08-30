#include "vmemory.h"

namespace V {

Memory::Memory(const Device *device, const VkMemoryAllocateInfo &allocateInfo)
    : m_device(device)
    , m_size(allocateInfo.allocationSize)
{
    if (vkAllocateMemory(m_device->device(), &allocateInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate memory");
}

Memory::~Memory()
{
    if (m_handle != VK_NULL_HANDLE)
        vkFreeMemory(m_device->device(), m_handle, nullptr);
}

} // namespace V
