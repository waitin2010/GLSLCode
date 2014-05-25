#version 400

uniform mat4 mvp_matrix;
in vec4 position;
out vec4 pos;
void main()
{
	pos = mvp_matrix * position;
	gl_Position = pos;
}