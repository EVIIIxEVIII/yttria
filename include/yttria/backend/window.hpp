#pragma once
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace yttria::backend {

class Window {
public:
    Window(int w, int h, std::string names);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    bool ShouldClose() {return glfwWindowShouldClose(window_); };
    VkExtent2D Extent() { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)}; }
    bool WasWindowResized() { return framebufferResized_; }

    void ResetWindowResized() { framebufferResized_ = false; }

    GLFWwindow *GLFWWindow() const { return window_; }

private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void initWindow();

    int width_;
    int height_;
    bool framebufferResized_ = false;

    std::string windowName_;
    GLFWwindow *window_;
};

}
