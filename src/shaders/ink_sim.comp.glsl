#version 450

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(set = 0, binding = 1, rgba16f) uniform image3D velocityImage;
layout(set = 0, binding = 2) uniform sampler3D currentDyeSampler;
layout(set = 0, binding = 3, r16f) uniform image3D nextDyeImage;

// Instead of timestep, just bake it into velocity texture

void main() {
	ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
	vec3 pos = vec3(coord);  // now purely grid coordinates

	// Sample velocity at current grid cell
	vec4 velocity = imageLoad(velocityImage, coord);
	vec3 vel = velocity.xyz;

	// Backtrace directly in grid units
	vec3 prevPos = pos - vel;  // assume vel already contains timestep * velocity

	// Normalize for texture sampling
	vec3 texSize = vec3(imageSize(velocityImage));
	vec3 uv = prevPos / texSize;

	uv = clamp(uv, vec3(0.0), vec3(1.0));

	float dye = texture(currentDyeSampler, uv).r;

	imageStore(nextDyeImage, coord, vec4(dye, 0, 0, 0));
}

