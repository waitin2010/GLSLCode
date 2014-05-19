#version 400 core

/// in variables
#ifdef PHONE_MODEL
/// gauround shading (or flat shading)
flat in vec4 frontColor;
flat in vec4 backColor;
flat in vec2 UV;
#else
/// phone shadeing (or smooth shading)
in vec4 frontColor;
in vec4 backColor;
in vec2 UV;
#endif 

out vec4 fragColor;

void main()
{
	const float scale = 15.0;
	bvec2 toDiscard = greaterThan(fract(UV * scale), vec2(0.2,0.2));
	if(all(toDiscard))
		discard;
	if(gl_FrontFacing)
		fragColor = frontColor;
	else
		fragColor = backColor;
}	 