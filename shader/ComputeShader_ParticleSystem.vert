#version 430
layout(location = 4) in vec4 pos;
layout (location = 5) in vec4 velcolor;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec4 vscolor;

void main()
{
	vscolor = velcolor;
	gl_Position = projection_matrix * view_matrix * model_matrix * pos;
}
