#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

namespace yttria::backend {

class Surface {

public:
    Surface(const VkInstance& instance, GLFWwindow* window);
    VkSurfaceKHR surface() { return surface_; };

private:
    VkSurfaceKHR surface_;

};

}
