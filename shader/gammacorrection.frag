#version 400 core

/// in variables
#ifdef PHONE_MODEL
/// gauround shading (or flat shading)
flat in vec4 frontColor;
flat in vec4 backColor;
#else
/// phone shadeing (or smooth shading)
in vec4 frontColor;
in vec4 backColor;
#endif 

out vec4 fragColor;

uniform float gamma = 2.2;
vec4 powVec4(vec4 color)
{
	float r = pow(color.r, 1/gamma);
	float g = pow(color.g, 1/gamma);
	float b = pow(color.b, 1/gamma);
	return vec4(r,g,b,1.0);
}
void main()
{
	if(gl_FrontFacing)
		fragColor = powVec4(frontColor);
	else
		fragColor =  powVec4(frontColor);
}	