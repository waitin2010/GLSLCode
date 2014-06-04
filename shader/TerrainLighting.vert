#version 400


layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 world_matrix;

out vec3 Vs2Ps_normal;

void main()
{
	Vs2Ps_normal = mat3(world_matrix) * normal;
	gl_Position = projection_matrix * view_matrix * position;
}