#include "yttria/backend/scene_object.hpp"
#include <memory>

namespace yttria::backend {

glm::mat4 TransformComponent::mat4() {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    return glm::mat4{ // these are the columns not the rows!
        {
            scale * (c1 * c3 + s1 * s2 * s3),
            scale * (c2 * s3),
            scale * (c1 * s2 * s3 - c3 * s1),
            0.0f,
        },
        {
            scale * (c3 * s1 * s2 - c1 * s3),
            scale * (c2 * c3),
            scale * (c1 * c3 * s2 + s1 * s3),
            0.0f,
        },
        {
            scale * (c2 * s1),
            scale * (-s2),
            scale * (c1 * c2),
            0.0f,
        },
        {translation.x, translation.y, translation.z, 1.0f}};
}

SceneObject SceneObject::makePointLight(float intensity, float radius, glm::vec3 color) {
    SceneObject sceneObj = SceneObject::createSceneObject();
    sceneObj.color = color;
    sceneObj.transform.scale = radius;
    sceneObj.pointLight = std::make_unique<PointLightComponent>();
    sceneObj.pointLight->lightIntensity = intensity;
    return sceneObj;
}

}
