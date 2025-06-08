#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

namespace yttria::backend {

class Camera {

public:
    void setOrthographicProjection(
        float right, float left,
        float top, float bottom,
        float near, float far
    );

    void setPerspectiveProjection(float fovy, float aspect, float near, float far);

    void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

    const glm::mat4& getProjection() const { return projectionMatrix_; }
    const glm::mat4& getView() const { return viewMatrix_; }
    const glm::mat4& getInverseView() const { return inverseViewMatrix_; }

private:
    glm::mat4 projectionMatrix_{1.f};
    glm::mat4 viewMatrix_{1.f};
    glm::mat4 inverseViewMatrix_{1.f};
};

}
