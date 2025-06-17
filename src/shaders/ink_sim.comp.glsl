#version 450
layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(set = 0, binding = 2, rgba16f) writeonly uniform image3D outVol;

void main()
{
    // Give each voxel a simple RGB gradient that’s obvious
    ivec3  p   = ivec3(gl_GlobalInvocationID);
    ivec3  dim = imageSize(outVol);        // (256,256,256)

    vec3 colour = vec3(p) / vec3(dim - 1); // 0-1 ramp
    imageStore(outVol, p, vec4(1.0, 0.0, 0.0, 1.0));
}
