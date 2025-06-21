#include "yttria/backend/systems/fluid_sim.hpp"

#include <memory>
#include <stdexcept>
#include <assert.h>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

FluidSim::FluidSim(Device& device, VkDescriptorSetLayout globalSetLayout, const FluidContext& fluidContext):
    device_{device},
    fluidContext_{fluidContext}
{
    createPipelineLayout(globalSetLayout);
    createPipeline();
}

FluidSim::~FluidSim() {
    vkDestroyPipelineLayout(device_.device(), pipelineLayout_, nullptr);
}

void FluidSim::createPipeline() {
    assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");


    compPipeline_ = std::make_unique<ComputePipeline>(
        device_,
        "/home/alderson/Projects/Mine/yttria/src/shaders/fluid_sim.comp.spv",
        pipelineLayout_
    );
}

void FluidSim::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(FluidSimPushConstants);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a pipeline layout");
    }
}

void FluidSim::record(FrameInfo &frameInfo) {
    vkCmdBindPipeline(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compPipeline_->pipeline());

    FluidSimPushConstants push{};
    push.frameTime = frameInfo.frameTime;

    vkCmdPushConstants(
        frameInfo.commandBuffer,
        pipelineLayout_,
        VK_SHADER_STAGE_COMPUTE_BIT,
        0,
        sizeof(FluidSimPushConstants),
        &push
    );

    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        pipelineLayout_, 0, 1,
        &frameInfo.globalDescriptorSet,
        0, nullptr
    );

    uint32_t threadsPerBlock = 8;
    auto roundUp = [](uint32_t x, uint32_t blockSize) {
        return (x + blockSize - 1) / blockSize;
    };

    vkCmdDispatch(
        frameInfo.commandBuffer,
        roundUp(fluidContext_.dim.x, threadsPerBlock),
        roundUp(fluidContext_.dim.y, threadsPerBlock),
        roundUp(fluidContext_.dim.z, threadsPerBlock)
    );

    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask       = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask       = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout           = VK_IMAGE_LAYOUT_GENERAL;
    barrier.newLayout           = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image               = fluidContext_.fluid->image();
    barrier.subresourceRange    = {
        VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1
    };

    vkCmdPipelineBarrier(frameInfo.commandBuffer,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier
    );
}

}
