#include "yttria/backend/surface.hpp"

#include <stdexcept>

namespace yttria::backend {

Surface::Surface(const VkInstance& instance, GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface_) != VK_SUCCESS) {
        throw std::runtime_error("failed to craete window_ surface");
    }
}

}
