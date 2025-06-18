#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inUV;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    mat4 modelInv;
} pc;

layout(location = 0) out vec3 vPosWorld;
layout(location = 1) out vec3 vPosObject;

void main() {
    vPosObject = inPos;
    vec4 world = pc.model * vec4(inPos, 1.0);
    vPosWorld  = world.xyz;
    gl_Position = ubo.projection * ubo.view * world;
}

