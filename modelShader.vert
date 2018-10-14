#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform  UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
}ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};


vec2 positions[3] = vec2[]
(
    vec2(0.0,-0.5),
    vec2(0.5,0.5),
    vec2(-0.5,0.5)
);

vec3 colors[3] = vec3[]
(
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
	mat4 mat;
	mat[0][0] = 1;
	mat[1][1] = 1;
	mat[2][2] = 1;
	mat[3][3] = 1;

    gl_Position = ubo.proj * ubo.model * vec4(inPosition,0.0,1.0);
    fragColor = inColor;
}