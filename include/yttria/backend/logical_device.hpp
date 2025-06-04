#pragma once


#include "yttria/backend/physical_device.hpp"
namespace yttria::backend {

class LogicalDevice {

public:
    LogicalDevice(
        const VkPhysicalDevice& physicalDevice,
        const QueueFamilyIndices& indices,
        const std::vector<const char *>& deviceExtensions,
        const std::vector<const char *>& validationLayers,
        bool enableValidationLayers
    );

    VkDevice get() const { return device_; }

private:
    VkDevice device_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;

};


}
