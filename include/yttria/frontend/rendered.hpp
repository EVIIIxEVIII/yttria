#pragma once
#include "yttria/backend/instance.hpp"
#include "yttria/backend/logical_device.hpp"
#include "yttria/backend/surface.hpp"
#include "yttria/backend/physical_device.hpp"
#include "yttria/backend/window.hpp"

namespace be = yttria::backend;

namespace yttria::frontend {

class Renderer {
    public:
        Renderer();

    private:
        be::Window window_;
        be::Instance instance_;
        be::Surface surface_;
        be::PhysicalDevice physicalDevice_;
        be::LogicalDevice logicalDevice_;

        const std::vector<const char *> validationLayers_ = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions_ = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

};

}
