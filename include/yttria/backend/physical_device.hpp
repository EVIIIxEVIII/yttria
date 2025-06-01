#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  uint32_t graphicsFamily;
  uint32_t presentFamily;
  bool graphicsFamilyHasValue = false;
  bool presentFamilyHasValue = false;
  bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class PhysicalDevice {
public:
    PhysicalDevice(const VkInstance instance, VkSurfaceKHR surface);
    VkPhysicalDevice getPhysicalDevice() { return physicalDevice_; };

private:
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    void pickPhysicalDevice();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    const VkInstance instance_;
    VkSurfaceKHR surface_;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties_;

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}
