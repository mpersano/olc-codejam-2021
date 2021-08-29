#pragma once

#include "noncopyable.h"
#include "vulkandevice.h"

#include <vulkan/vulkan.h>

class VulkanSemaphore : private NonCopyable
{
public:
    explicit VulkanSemaphore(const VulkanDevice *device);
    ~VulkanSemaphore();

    const VulkanDevice *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkSemaphore handle() const { return m_handle; }

private:
    const VulkanDevice *m_device;
    VkSemaphore m_handle = VK_NULL_HANDLE;
};
