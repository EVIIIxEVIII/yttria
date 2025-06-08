#pragma once

#include "yttria/backend/device.hpp"

namespace yttria::backend {

class Buffer {
   public:
    Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount,
           VkBufferUsageFlags usageFlags,
           VkMemoryPropertyFlags memoryPropertyFlags,
           VkDeviceSize minOffsetAlignment = 1);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE,
                       VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE,
                                          VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE,
                        VkDeviceSize offset = 0);

    void writeToIndex(void* data, int index);
    VkResult flushIndex(int index);
    VkDescriptorBufferInfo descriptorInfoForIndex(int index);
    VkResult invalidateIndex(int index);

    VkBuffer getBuffer() const { return buffer_; }
    void* getMappedMemory() const { return mapped_; }
    uint32_t getInstanceCount() const { return instanceCount_; }
    VkDeviceSize getInstanceSize() const { return instanceSize_; }
    VkDeviceSize getAlignmentSize() const { return instanceSize_; }
    VkBufferUsageFlags getUsageFlags() const { return usageFlags_; }
    VkMemoryPropertyFlags getMemoryPropertyFlags() const {
        return memoryPropertyFlags_;
    }
    VkDeviceSize getBufferSize() const { return bufferSize_; }

   private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize,
                                     VkDeviceSize minOffsetAlignment);

    Device& device_;
    void* mapped_ = nullptr;
    VkBuffer buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;

    VkDeviceSize bufferSize_;
    VkDeviceSize instanceSize_;
    uint32_t instanceCount_;
    VkDeviceSize alignmentSize_;
    VkBufferUsageFlags usageFlags_;
    VkMemoryPropertyFlags memoryPropertyFlags_;
};

}  // namespace yttria::backend

