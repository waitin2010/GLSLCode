#version 400


layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 world_matrix;

out vec3 Vs2Ps_normal;
out vec2 Vs2Ps_texCoord;

void main()
{
	Vs2Ps_normal = mat3(world_matrix) * normal;
	Vs2Ps_texCoord = texCoord;
	gl_Position = projection_matrix * view_matrix * position;
}