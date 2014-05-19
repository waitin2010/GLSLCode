#version 400
in vec3 position;
out vec2 UV;
void main()
{
    UV = (position.xy+vec2(1.0,1.0))/2.0;
	gl_Position = vec4(position,1);
}