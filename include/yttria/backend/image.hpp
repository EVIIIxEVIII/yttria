#pragma once

#include "yttria/backend/device.hpp"

namespace yttria::backend {

class Image {

public:
    Image(
        Device& device,
        uint32_t width, uint32_t height, uint32_t depth,
        VkFormat format,
        VkImageUsageFlags usage,
        VkImageAspectFlags aspectFlags,
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
        VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VkImageLayout targetLayout = VK_IMAGE_LAYOUT_GENERAL
    );

    ~Image();

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    VkImageView getImageView() const { return imageView_; }
    VkImage getImage() const { return image_; }
    VkDescriptorImageInfo descriptorInfo(VkSampler sampler);

private:
    Device& device_;
    VkImageLayout layout_;
    VkImage image_{VK_NULL_HANDLE};
    VkDeviceMemory memory_{VK_NULL_HANDLE};
    VkImageView imageView_{VK_NULL_HANDLE};

    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
    void createImage(
        uint32_t width, uint32_t height, uint32_t depth,
        VkFormat format, VkImageUsageFlags usage,
        VkImageTiling tiling, VkMemoryPropertyFlags properties
    );
    void transitionImage(
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkImageAspectFlags aspectMask,
        uint32_t mipLevels
    );
};


}
