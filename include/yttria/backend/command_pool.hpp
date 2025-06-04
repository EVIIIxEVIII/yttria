#pragma once

#include "yttria/backend/physical_device.hpp"

namespace yttria::backend {

class CommandPool {

public:
    CommandPool(
        const VkDevice& device,
        const QueueFamilyIndices& indices
    );

private:
    VkCommandPool commandPool_;
};

}
