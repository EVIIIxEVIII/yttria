#include "yttria/backend/camera.hpp"

#include <cassert>
#include <limits>

namespace yttria::backend {

void Camera::setOrthographicProjection(
    float left, float right, float top, float bottom, float near, float far) {
    projectionMatrix_ = glm::mat4{1.0f};
    projectionMatrix_[0][0] = 2.f / (right - left);
    projectionMatrix_[1][1] = 2.f / (bottom - top);
    projectionMatrix_[2][2] = 1.f / (far - near);
    projectionMatrix_[3][0] = -(right + left) / (right - left);
    projectionMatrix_[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix_[3][2] = -near / (far - near);
}

void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tanf(fovy / 2.f);
    projectionMatrix_ = glm::mat4{0.0f};
    projectionMatrix_[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix_[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix_[2][2] = far / (far - near);
    projectionMatrix_[2][3] = 1.f;
    projectionMatrix_[3][2] = -(far * near) / (far - near);
}

void Camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    viewMatrix_ = glm::mat4{1.f};
    viewMatrix_[0][0] = u.x;
    viewMatrix_[1][0] = u.y;
    viewMatrix_[2][0] = u.z;
    viewMatrix_[0][1] = v.x;
    viewMatrix_[1][1] = v.y;
    viewMatrix_[2][1] = v.z;
    viewMatrix_[0][2] = w.x;
    viewMatrix_[1][2] = w.y;
    viewMatrix_[2][2] = w.z;
    viewMatrix_[3][0] = -glm::dot(u, position);
    viewMatrix_[3][1] = -glm::dot(v, position);
    viewMatrix_[3][2] = -glm::dot(w, position);

    inverseViewMatrix_ = glm::mat4{1.f};
    inverseViewMatrix_[0][0] = u.x;
    inverseViewMatrix_[0][1] = u.y;
    inverseViewMatrix_[0][2] = u.z;
    inverseViewMatrix_[1][0] = v.x;
    inverseViewMatrix_[1][1] = v.y;
    inverseViewMatrix_[1][2] = v.z;
    inverseViewMatrix_[2][0] = w.x;
    inverseViewMatrix_[2][1] = w.y;
    inverseViewMatrix_[2][2] = w.z;
    inverseViewMatrix_[3][0] = position.x;
    inverseViewMatrix_[3][1] = position.y;
    inverseViewMatrix_[3][2] = position.z;
}

void Camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    setViewDirection(position, target - position, up);
}

void Camera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
    viewMatrix_ = glm::mat4{1.f};
    viewMatrix_[0][0] = u.x;
    viewMatrix_[1][0] = u.y;
    viewMatrix_[2][0] = u.z;
    viewMatrix_[0][1] = v.x;
    viewMatrix_[1][1] = v.y;
    viewMatrix_[2][1] = v.z;
    viewMatrix_[0][2] = w.x;
    viewMatrix_[1][2] = w.y;
    viewMatrix_[2][2] = w.z;
    viewMatrix_[3][0] = -glm::dot(u, position);
    viewMatrix_[3][1] = -glm::dot(v, position);
    viewMatrix_[3][2] = -glm::dot(w, position);

    inverseViewMatrix_ = glm::mat4{1.f};
    inverseViewMatrix_[0][0] = u.x;
    inverseViewMatrix_[0][1] = u.y;
    inverseViewMatrix_[0][2] = u.z;
    inverseViewMatrix_[1][0] = v.x;
    inverseViewMatrix_[1][1] = v.y;
    inverseViewMatrix_[1][2] = v.z;
    inverseViewMatrix_[2][0] = w.x;
    inverseViewMatrix_[2][1] = w.y;
    inverseViewMatrix_[2][2] = w.z;
    inverseViewMatrix_[3][0] = position.x;
    inverseViewMatrix_[3][1] = position.y;
    inverseViewMatrix_[3][2] = position.z;
}

}
