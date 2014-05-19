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

void main()
{
	if(gl_FrontFacing)
		fragColor = frontColor;
	else
		fragColor = backColor;
}	