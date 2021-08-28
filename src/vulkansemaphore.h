#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanSemaphore : private NonCopyable
{
public:
    explicit VulkanSemaphore(VulkanDevice *device);
    ~VulkanSemaphore();

    VkSemaphore handle() const { return m_handle; }

private:
    VulkanDevice *m_device;
    VkSemaphore m_handle = VK_NULL_HANDLE;
};
