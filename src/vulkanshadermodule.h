#pragma once

#include "noncopyable.h"

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanShaderModule : private NonCopyable
{
public:
    explicit VulkanShaderModule(VulkanDevice *device, const char *spvFilePath);
    ~VulkanShaderModule();

    VkShaderModule handle() const { return m_handle; }

private:
    VulkanDevice *m_device;
    VkShaderModule m_handle = VK_NULL_HANDLE;
};
