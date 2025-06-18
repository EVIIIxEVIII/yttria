#version 450
layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(set = 0, binding = 2, r32f) writeonly uniform image3D dst;

void main()
{
    ivec3  P   = ivec3(gl_GlobalInvocationID);
    ivec3  dim = imageSize(dst);
    vec3   pos = (vec3(P) + 0.5) / vec3(dim);

    vec3   center = vec3(0.5);
    float  radius = 0.25;
    float  inside = length(pos - center) < radius ? 1.0 : 0.0;

    imageStore(dst, P, vec4(inside));
}

