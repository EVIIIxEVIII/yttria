#version 450
layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(set = 0, binding = 2, rg16f) writeonly uniform image3D outVol;

void main()
{
    ivec3 p   = ivec3(gl_GlobalInvocationID); // (x,y,z)
    if (p.z != 64) return;                   // slice 64 only

    vec2 centre = vec2(128.0);   // centre of 256×256 slice
    float radius = 40.0;         // tweak as you like

    vec2 pos    = vec2(p.xy) + 0.5;
    float dist  = length(pos - centre);

    if (dist < radius)
        imageStore(outVol, p, vec4(1.0));
    else
        imageStore(outVol, p, vec4(0.0));
}

