#include <cassert>
#include <cstring>

#include "yttria/backend/VulkanBuffer.hpp"
#include "yttria/backend/VulkanDevice.hpp"

namespace yttria::backend {

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize,
                                  VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) &
               ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

Buffer::Buffer(class Device &device, VkDeviceSize instanceSize,
               uint32_t instanceCount, VkBufferUsageFlags usageFlags,
               VkMemoryPropertyFlags memoryPropertyFlags,
               VkDeviceSize minOffsetAlignment)
    : device_{device},
      instanceSize_{instanceSize},
      instanceCount_{instanceCount},
      usageFlags_{usageFlags},
      memoryPropertyFlags_{memoryPropertyFlags} {
    alignmentSize_ = getAlignment(instanceSize_, minOffsetAlignment);
    bufferSize_ = alignmentSize_ * instanceCount_;
    device_.createBuffer(bufferSize_, usageFlags, memoryPropertyFlags, buffer_,
                        memory_);
}

Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(device_.device(), buffer_, nullptr);
    vkFreeMemory(device_.device(), memory_, nullptr);
}

VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(buffer_ && memory_ && "Called map on buffer before create");
    return vkMapMemory(device_.device(), memory_, offset, size, 0, &mapped_);
}

void Buffer::unmap() {
    if (mapped_) {
        vkUnmapMemory(device_.device(), memory_);
        mapped_ = nullptr;
    }
}

void Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
    assert(mapped_ && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        memcpy(mapped_, data, bufferSize_);
    } else {
        char *memOffset = (char *)mapped_;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(device_.device(), 1, &mappedRange);
}

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(device_.device(), 1, &mappedRange);
}

VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size,
                                              VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
        buffer_,
        offset,
        size,
    };
}

void Buffer::writeToIndex(void *data, int index) {
    writeToBuffer(data, instanceSize_,
                  static_cast<size_t>(index) * alignmentSize_);
}

VkResult Buffer::flushIndex(int index) {
    return flush(alignmentSize_, static_cast<size_t>(index) * alignmentSize_);
}

VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
    return descriptorInfo(alignmentSize_,
                          static_cast<size_t>(index) * alignmentSize_);
}

VkResult Buffer::invalidateIndex(int index) {
    return invalidate(alignmentSize_,
                      static_cast<size_t>(index) * alignmentSize_);
}

}

