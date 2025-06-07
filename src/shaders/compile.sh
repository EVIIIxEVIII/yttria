glslc -fshader-stage=vertex vertex.glsl -o vert.spv
glslc -fshader-stage=fragment fragment.glsl -o frag.spv

glslc -fshader-stage=vertex point_light.vert.glsl -o point_light.vert.spv
glslc -fshader-stage=fragment point_light.frag.glsl -o point_light.frag.spv
# glslc -fshader-stage=compute compute.glsl -o compute.spv
