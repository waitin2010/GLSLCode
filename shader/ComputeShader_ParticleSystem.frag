#version 430 
in vec4 vscolor;
out vec4 color;
void main()
{
	color = vec4(vscolor.rgb,1.0);
	//color = vec4(1.0,1.0,1.0,1.0);
}