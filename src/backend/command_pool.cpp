
#include "yttria/backend/command_pool.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

CommandPool::CommandPool(
    const VkDevice& device,
    const QueueFamilyIndices& indices
) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = indices.graphicsFamily;
    poolInfo.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
        throw::std::runtime_error("failed to create command pool!");
    }
}



}
