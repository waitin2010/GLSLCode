#version 400 core
/// for gaurond shading 
/// add #define PHONE_MODEL in both vertex shader and fragment shader

/// vertex attributes
layout (location = 0) in  vec4 position;
layout (location = 1) in  vec3 normal;

/// output data
#ifdef PHONE_MODEL
/// gauround shading (or flat shading)
flat out vec4 frontColor;
flat out vec4 backColor;
#else
/// phone shadeing (or smooth shading)
out vec4 frontColor;
out vec4 backColor;
out vec3 pos;
out vec3 N;
#endif 

uniform float K = 0.5;
uniform float velocity = 0.01;
uniform float amp = 0.1;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

uniform float time;

void main()
{
	vec4 posTemp = position;
	float u = K * (posTemp.x - velocity * time);

	posTemp.y = amp*sin(u);

	vec3 n = vec3(0.0);
	n.xy = normalize(vec2(-K * amp * cos(u), 1.0));

	pos = vec3(model_matrix * posTemp);
	N = normalize(mat3(model_matrix) *n);
	gl_Position = projection_matrix * view_matrix * model_matrix * posTemp;
}