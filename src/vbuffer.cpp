#include "vbuffer.h"

#include "vmemory.h"

namespace V {

Buffer::Buffer(const Device *device, VkDeviceSize size, VkBufferUsageFlags usage)
    : m_device(device)
    , m_size(size)
{
    uint32_t queueFamilyIndex = m_device->queueFamilyIndex();
    VkBufferCreateInfo bufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &queueFamilyIndex
    };

    if (vkCreateBuffer(m_device->device(), &bufferCreateInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer");
}

Buffer::~Buffer()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyBuffer(m_device->device(), m_handle, nullptr);
}

void Buffer::bindMemory(const Memory *memory, VkDeviceSize offset) const
{
    if (vkBindBufferMemory(m_device->device(), m_handle, memory->handle(), offset) != VK_SUCCESS)
        throw std::runtime_error("Failed to bind buffer");
}

} // namespace V
