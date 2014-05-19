#version 400 core

in float Transp;

uniform sampler2D Tex;

layout (location = 0) out vec4 fragColor;
void main()
{

	fragColor = texture(Tex,gl_PointCoord);
	//fragColor = vec4(1.0,0.0,0.0,1.0);
	fragColor.a *= Transp;
}	 