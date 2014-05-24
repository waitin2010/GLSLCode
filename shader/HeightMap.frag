#version 400

out vec4 fragColor;
in vec2 UV;
uniform float innerRadius = 0.2;
uniform float outerRadius = 0.6;
uniform vec3 innerColor = vec3(1.0,1.0,1.0);
uniform vec3 outerColor = vec3(0.0,0.0,0.0);
void main()
{
	float dx = UV.s - 0.5;
	float dy = UV.t - 0.5;
	float dist = sqrt(dx*dx + dy*dy);

	float ecoff = smoothstep(innerRadius, outerRadius, dist);
	vec3 color = mix(innerColor, outerColor, ecoff);
	fragColor = vec4(color,1.0);
}	