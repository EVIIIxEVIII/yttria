#include "yttria/backend/device.hpp"
#include <vulkan/vulkan.h>

namespace yttria::backend {

class LinearClampSampler {
public:
    LinearClampSampler(Device& device);
    ~LinearClampSampler();

    LinearClampSampler(const LinearClampSampler&) = delete;
    LinearClampSampler& operator=(const LinearClampSampler&) = delete;

    VkSampler sampler() const { return sampler_; }

private:
    Device& device_;
    VkSampler sampler_;
};

}
