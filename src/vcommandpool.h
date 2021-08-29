#pragma once

#include "noncopyable.h"
#include "vdevice.h"

#include <vulkan/vulkan.h>

#include <memory>

namespace V {

class CommandBuffer;

class CommandPool : private NonCopyable
{
public:
    explicit CommandPool(const Device *device);
    ~CommandPool();

    const Device *device() const { return m_device; }
    VkDevice deviceHandle() const { return m_device->device(); }

    VkCommandPool handle() const { return m_handle; }

    std::unique_ptr<CommandBuffer> allocateCommandBuffer() const;

private:
    const Device *m_device;
    VkCommandPool m_handle;
};

} // namespace V
