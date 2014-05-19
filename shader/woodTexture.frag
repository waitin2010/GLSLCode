#version 400 core
#define PI 3.14159265
/// in variables
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D noiseTex;
uniform vec4 darkWoodColor = vec4(0.8, 0.5, 0.1, 1.0);
uniform vec4 lightWoodColor = vec4(1.0, 0.75, 0.25, 1.0);
uniform mat4 slice;
void main()
{
	#if 1
	vec2 tc = texCoord;
	if(tc.s > 0.5) tc.s = 1.0 - tc.s;
	#endif 
	vec4 cy1 = slice * vec4(tc, 0.0, 1.0);
	float dist = length(cy1.xz);

	vec4 noise = texture(noiseTex, texCoord);
	dist += noise.b;

	float t = 1.0 - abs(fract(dist) * 2.0 - 1.0);
	t = smoothstep(0.2, 0.5, t);
	vec4 color = mix(darkWoodColor, lightWoodColor, t);
	fragColor = vec4(color.rgb, 1.0);
}	