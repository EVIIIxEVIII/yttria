#pragma once

#include "yttria/backend/model.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_float2x2.hpp>
#include <memory>
#include <unordered_map>

namespace yttria::backend {

struct TransformComponent {
    glm::vec3 translation{};
    float scale = 1.f;
    glm::vec3 rotation{};

    glm::mat4 mat4();
};

struct PointLightComponent {
    float lightIntensity = 1.0f;
};

class SceneObject {
public:
    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, SceneObject>;

    static SceneObject createSceneObject() {
        static id_t currentId = 0;
        return SceneObject{currentId++};
    }

    static SceneObject makePointLight(
        float intensity = 10.f,
        float radius = 0.1f,
        glm::vec3 color = glm::vec3(1.f)
    );

    SceneObject(const SceneObject &) = delete;
    SceneObject &operator=(const SceneObject &) = delete;
    SceneObject(SceneObject &&) = default;
    SceneObject &operator=(SceneObject &&) = default;

    id_t getId() const { return id_; }

    std::shared_ptr<Model> model{};
    glm::vec3 color{};
    TransformComponent transform{};

    std::unique_ptr<PointLightComponent> pointLight = nullptr;

private:
    SceneObject(id_t objId): id_{objId} {};

    id_t id_;
};

}
