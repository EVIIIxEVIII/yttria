#version 450
layout(location = 0) in vec3 vPosWorld;
layout(location = 1) in vec3 vPosObject;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler3D dyeTex;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4  projection;
    mat4  view;
    mat4  inverseView;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    mat4 modelInv;
} pc;

// ---------------------------------------------------------------------------------
// Slab method: ray–AABB intersection in object space (box = [-0.5,0.5]³)
bool intersectBox(vec3 ro, vec3 rd, out float t0, out float t1)
{
    vec3 invDir = 1.0 / rd;
    vec3 tmin = (-0.5 - ro) * invDir;
    vec3 tmax = ( 0.5 - ro) * invDir;
    vec3 t1s  = min(tmin, tmax);
    vec3 t2s  = max(tmin, tmax);
    t0 = max(max(t1s.x, t1s.y), t1s.z);
    t1 = min(min(t2s.x, t2s.y), t2s.z);
    return t1 >= max(t0, 0.0);
}

void main()
{
    vec3 camPosWorld = ubo.inverseView[3].xyz;
    vec3 rdWorld = normalize(vPosWorld - camPosWorld);

    vec3 roObj = (pc.modelInv * vec4(camPosWorld, 1.0)).xyz;
    vec3 rdObj = normalize((pc.modelInv * vec4(rdWorld, 0.0)).xyz);

    float tEnter, tExit;
    if (!intersectBox(roObj, rdObj, tEnter, tExit))
        discard;

    const float stepSize = 0.01;
    float t = tEnter;
    vec3  accum = vec3(0.0);
    float alpha = 0.0;

    for (int i = 0; i < 256 && t < tExit && alpha < 0.99; ++i) {
        vec3 pObj = roObj + rdObj * t;
        vec3 uvw  = pObj + 0.5;

        float density = step( length(pObj) , 0.3 ) * 1.0;
        float k   = 4.0;
        float a   = 1.0 - exp(-k * density * stepSize);
        accum += (1.0 - alpha) * vec3(density) * a;
        alpha += (1.0 - alpha) * a;

        t += stepSize;
    }

    outColor = vec4(accum, alpha);
}

