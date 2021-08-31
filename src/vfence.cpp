#include "vfence.h"

namespace V {

Fence::Fence(const Device *device, bool createSignaled)
    : m_device(device)
{
    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = createSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
    };

    if (vkCreateFence(m_device->device(), &fenceCreateInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create fence");
}

Fence::~Fence()
{
    vkDestroyFence(m_device->device(), m_handle, nullptr);
}

void Fence::wait()
{
    vkWaitForFences(m_device->device(), 1, &m_handle, VK_TRUE, UINT64_MAX);
}

void Fence::reset()
{
    vkResetFences(m_device->device(), 1, &m_handle);
}

} // namespace V
