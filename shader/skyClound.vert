#version 400 core
/// for gaurond shading 
/// add #define PHONE_MODEL in both vertex shader and fragment shader

/// vertex attributes
layout (location = 0) in  vec4 position;
layout (location = 1) in  vec3 normal;
layout (location = 2) in  vec2 vertexTexCoord;

out vec2 texCoord;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;



void main()
{
	texCoord = vertexTexCoord;
	gl_Position = projection_matrix * view_matrix * model_matrix * position;
}