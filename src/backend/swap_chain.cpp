#include "yttria/backend/swap_chain.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent)
: device_{deviceRef}, windowExtent_{extent} {
    init();
}

SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous)
: device_{deviceRef}, windowExtent_{extent}, oldSwapChain_{previous} {
    init();

    oldSwapChain_ = nullptr;
}

void SwapChain::init() {
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
    createSyncObjects();
}

SwapChain::~SwapChain() {
    for (auto imageView : swapChainImageViews_) {
        vkDestroyImageView(device_.device(), imageView, nullptr);
    }
    swapChainImageViews_.clear();

    if (swapChain_ != nullptr) {
        vkDestroySwapchainKHR(device_.device(), swapChain_, nullptr);
        swapChain_ = nullptr;
    }

    for (size_t i = 0; i < depthImages_.size(); i++) {
        vkDestroyImageView(device_.device(), depthImageViews_[i], nullptr);
        vkDestroyImage(device_.device(), depthImages_[i], nullptr);
        vkFreeMemory(device_.device(), depthImageMemorys_[i], nullptr);
    }

    for (auto framebuffer : swapChainFramebuffers_) {
        vkDestroyFramebuffer(device_.device(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(device_.device(), renderPass_, nullptr);


    for (size_t i = 0; i < imageCount(); i++) {
        vkDestroySemaphore(device_.device(), renderFinishedSemaphores_[i], nullptr);
    }

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device_.device(), imageAvailableSemaphores_[i], nullptr);
        vkDestroyFence(device_.device(), inFlightFences_[i], nullptr);
    }
}

VkResult SwapChain::acquireNextImage(uint32_t *imageIndex) {
    vkWaitForFences(
        device_.device(),
        1,
        &inFlightFences_[currentFrame_],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        device_.device(),
        swapChain_,
        std::numeric_limits<uint64_t>::max(),
        imageAvailableSemaphores_[currentFrame_],  // must be a not signaled semaphore
        VK_NULL_HANDLE,
        imageIndex);

    return result;
}

VkResult SwapChain::submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex) {
    if (imagesInFlight_[*imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device_.device(), 1, &imagesInFlight_[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight_[*imageIndex] = inFlightFences_[currentFrame_];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_[currentFrame_]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_[*imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device_.device(), 1, &inFlightFences_[currentFrame_]);
    if (vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, inFlightFences_[currentFrame_]) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain_};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(device_.presentQueue(), &presentInfo);

    currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = device_.getSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = device_.surface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device_.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsAndComputeFamily, indices.presentFamily};

    if (indices.graphicsAndComputeFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapChain_ == nullptr ? nullptr : oldSwapChain_->swapChain_;

    if (vkCreateSwapchainKHR(device_.device(), &createInfo, nullptr, &swapChain_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(device_.device(), swapChain_, &imageCount, nullptr);
    swapChainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_.device(), swapChain_, &imageCount, swapChainImages_.data());

    swapChainImageFormat_ = surfaceFormat.format;
    swapChainExtent_ = extent;
}

void SwapChain::createImageViews() {
    swapChainImageViews_.resize(swapChainImages_.size());
    for (size_t i = 0; i < swapChainImages_.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapChainImages_[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapChainImageFormat_;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device_.device(), &viewInfo, nullptr, &swapChainImageViews_[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

void SwapChain::createRenderPass() {
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = getSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};

    dependency.dstSubpass = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device_.device(), &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void SwapChain::createFramebuffers() {
    swapChainFramebuffers_.resize(imageCount());
    for (size_t i = 0; i < imageCount(); i++) {
        std::array<VkImageView, 2> attachments = {swapChainImageViews_[i], depthImageViews_[i]};

        VkExtent2D swapChainExtent = getSwapChainExtent();
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
            device_.device(),
            &framebufferInfo,
            nullptr,
            &swapChainFramebuffers_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void SwapChain::createDepthResources() {
    VkFormat depthFormat = findDepthFormat();
    swapChainDepthFormat_ = depthFormat;
    VkExtent2D swapChainExtent = getSwapChainExtent();

    depthImages_.resize(imageCount());
    depthImageMemorys_.resize(imageCount());
    depthImageViews_.resize(imageCount());

    for (size_t i = 0; i < depthImages_.size(); i++) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        device_.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthImages_[i],
            depthImageMemorys_[i]);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImages_[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device_.device(), &viewInfo, nullptr, &depthImageViews_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

void SwapChain::createSyncObjects() {
    imageAvailableSemaphores_.resize(imageCount());
    renderFinishedSemaphores_.resize(imageCount());
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight_.resize(imageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < renderFinishedSemaphores_.size(); i++) {
        if (vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (
            vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device_.device(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS
        ) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = windowExtent_;
        actualExtent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

VkFormat SwapChain::findDepthFormat() {
    return device_.findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

}
