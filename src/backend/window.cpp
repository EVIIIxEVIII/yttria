#include "yttria/backend/window.hpp"

#include <stdexcept>

namespace yttria {

backend::Window::Window(int w, int h, std::string name) : width_{w}, height_{h}, windowName_{name} {
    initWindow();
}

backend::Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void backend::Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window_ = glfwCreateWindow(width_, height_, windowName_.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
}

void backend::Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to craete window_ surface");
    }
}

void backend::Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto Window = reinterpret_cast<class Window*>(glfwGetWindowUserPointer(window));
    Window->framebufferResized_ = true;
    Window->width_ = width;
    Window->height_ = height;
}

}


