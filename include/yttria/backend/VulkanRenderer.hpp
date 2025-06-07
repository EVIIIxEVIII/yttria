#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan_core.h>
#include <cassert>

#include "yttria/backend/Window.hpp"
#include "yttria/backend/VulkanDevice.hpp"
#include "yttria/backend/VulkanSwapChain.hpp"

namespace yttria::backend {

class Renderer {

public:
    Renderer(Window& window, Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer operator=(const Renderer&) = delete;

    VkRenderPass getSwapChainRenderPass() const { return swapChain_->getRenderPass(); }
    float getAspectRatio() const { return swapChain_->extentAspectRatio(); };
    bool isFrameInProgress() const { return isFrameStarted_; }

    VkCommandBuffer getCurrentCommandBuffer() const {
        assert(isFrameStarted_ && "Cannot get command buffer when frame is in progress");
        return commandBuffers_[static_cast<size_t>(currentFrameIndex_)];
    }

    int getFrameIndex() const {
        assert(isFrameStarted_ && "Cannot get the frame index when frame is in progress");
        return currentFrameIndex_;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void recreateSwapChain();
    void freeCommandBuffers();

    Window& window_;
    Device& device_;
    std::unique_ptr<SwapChain> swapChain_;
    std::vector<VkCommandBuffer> commandBuffers_;

    uint32_t currentImageIndex_;
    int currentFrameIndex_ = 0;
    bool isFrameStarted_ = false;
};

}

