#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vulkan/vulkan_core.h>

#include "yttria/backend/logical_device.hpp"

namespace yttria::backend {

class SwapChain {

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(const LogicalDevice& device, VkExtent2D extent);
    SwapChain(const LogicalDevice& device, VkExtent2D extent, std::shared_ptr<SwapChain> previous);
    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    SwapChain& operator=(const SwapChain &) = delete;

    VkFramebuffer FrameBuffer(int index) { return swapChainFramebuffer_[index]; }

private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats
    );
    VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes
    );
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat swapChainImageFormat_;
    VkFormat swapChainDepthFormat_;
    VkExtent2D swapChainExtent_;

    std::vector<VkFramebuffer> swapChainFramebuffers_;
    VkRenderPass renderPass_;
};


}
