#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec4 vImgColor;
layout (location = 4) in vec2 fragUv;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 4) uniform sampler3D dyeTex;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
} push;

void main() {

    const float zSlice = (64.0 + 0.5) / 256.0;
    float r = texture(dyeTex, vec3(fragUv, zSlice)).r;
    outColor = vec4(r, 0.0, 0.0, r);
}
