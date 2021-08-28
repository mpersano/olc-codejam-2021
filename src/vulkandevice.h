#include "noncopyable.h"

#include <vulkan/vulkan.h>

class VulkanDevice : private NonCopyable
{
public:
    VulkanDevice();
    ~VulkanDevice();

private:
    void createInstance();
    void createDeviceAndQueue();
    void shutdown();

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
};
