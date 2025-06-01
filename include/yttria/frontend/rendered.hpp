#pragma once
#include "yttria/backend/instance.hpp"
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

};

}
