#version 400 core
#define PI 3.14159265
/// in variables
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D noiseTex;
uniform vec4 skyColor = vec4(0.3, 0.3, 0.9, 1.0);
uniform vec4 cloundColor = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
	vec4 noise = texture(noiseTex, texCoord);
	float t = (cos(noise.a * PI) + 1.0) / 2.0;
	#if 1
	t = clamp(t-0.25, 0.0,1.0);
	#endif 
	vec4 color = mix(skyColor, cloundColor, t);

	fragColor = vec4(color.rgb, 1.0);
}	