#include "vsemaphore.h"

#include <stdexcept>

namespace V {

Semaphore::Semaphore(const Device *device)
    : m_device(device)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    if (vkCreateSemaphore(device->device(), &semaphoreCreateInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create semaphore");
}

Semaphore::~Semaphore()
{
    if (m_handle != VK_NULL_HANDLE)
        vkDestroySemaphore(m_device->device(), m_handle, nullptr);
}

} // namespace V
