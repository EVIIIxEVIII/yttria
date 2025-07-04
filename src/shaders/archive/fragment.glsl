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
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.inverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    for (int i = 0; i < ubo.numLights; ++i) {
        PointLight light = ubo.pointLights[i];

        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        directionToLight = normalize(directionToLight);

        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0, 1);
        blinnTerm = pow(blinnTerm, 64.0); // the bigger the number the more metalic the thing
        specularLight += intensity * blinnTerm;
    }

    // outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
    // outColor = vImgColor;

    vec2 st = clamp(fragUv, 0.0, 1.0);
    const float zSlice = (64.0 + 0.5) / 256.0;
    float r = texture(dyeTex, vec3(st, zSlice)).r;
    outColor = vec4(r, 0.0, 0.0, 1.0);
}
