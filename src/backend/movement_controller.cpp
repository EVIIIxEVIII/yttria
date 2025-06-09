#include "yttria/backend/movement_controller.hpp"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <limits>

namespace yttria::backend {

void MovementController::moveInPlaneXZ(GLFWwindow* window, float dt, SceneObject& sceneObject) {
    static bool firstMouse = true;
    static double lastX = 0.0, lastY = 0.0;

    int w, h;
    glfwGetWindowSize(window, &w, &h);
    double centerX = w * 0.5, centerY = h * 0.5;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse) {
        lastX = centerX;
        lastY = centerY;
        firstMouse = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    double dx = mouseX - lastX;
    double dy = mouseY - lastY;

    glfwSetCursorPos(window, centerX, centerY);
    lastX = centerX;
    lastY = centerY;

    float sensitivity = turnSpeed;
    sceneObject.transform.rotation.y += static_cast<float>(dx) * sensitivity * dt;
    sceneObject.transform.rotation.x -= static_cast<float>(dy) * sensitivity * dt;

    constexpr float PITCH_LIMIT = glm::half_pi<float>() - 0.01f;
    sceneObject.transform.rotation.x = glm::clamp(
        sceneObject.transform.rotation.x,
       -PITCH_LIMIT,
        PITCH_LIMIT
    );

    sceneObject.transform.rotation.y = glm::mod(
        sceneObject.transform.rotation.y,
        glm::two_pi<float>()
    );

    float yaw = sceneObject.transform.rotation.y;
    const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
    const glm::vec3 rightDir{  forwardDir.z, 0.f, -forwardDir.x };
    const glm::vec3 upDir   { 0.f, 1.f, 0.f };

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, keys.moveForward ) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight   ) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft    ) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp      ) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown    ) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        sceneObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
}

}

