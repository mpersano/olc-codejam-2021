#include "vulkancommandpool.h"

#include "vulkancommandbuffer.h"

#include <iostream>

namespace V {

CommandPool::CommandPool(const Device *device)
    : m_device(device)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = m_device->queueFamilyIndex(),
    };

    if (vkCreateCommandPool(m_device->device(), &commandPoolCreateInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool");

    std::cout << "command pool=" << m_handle << '\n';
}

CommandPool::~CommandPool()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroyCommandPool(m_device->device(), m_handle, nullptr);
}

std::unique_ptr<CommandBuffer> CommandPool::allocateCommandBuffer() const
{
    return std::make_unique<CommandBuffer>(this);
}

} // namespace V
