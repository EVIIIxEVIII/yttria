#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace yttria::backend {

class Window {
public:
    Window(int w, int h, std::string name);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    bool shouldClose() { return glfwWindowShouldClose(window_); }
    VkExtent2D getExtent() { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)}; }
    bool wasWindowResized() { return framebufferResized_; }
    void resetWindowResized() { framebufferResized_ = false; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    GLFWwindow *getGLFWWindow() const { return window_; }

private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void initWindow();

    int width_;
    int height_;
    bool framebufferResized_ = false;

    std::string windowName_;
    GLFWwindow *window_;
};

}  // namespace lve
