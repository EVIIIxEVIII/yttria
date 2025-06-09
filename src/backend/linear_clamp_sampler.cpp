#include "yttria/backend/linear_clamp_sampler.hpp"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

LinearClampSampler::LinearClampSampler(
    Device& device
): device_{device}
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    vkCreateSampler(device_.device(), &samplerInfo, nullptr, &sampler_);
}

LinearClampSampler::~LinearClampSampler() {
    vkDestroySampler(device_.device(), sampler_, nullptr);
}

}
