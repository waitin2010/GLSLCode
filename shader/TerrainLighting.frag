#version 400

struct Light
{
	vec4 ambientColor;
	vec4 diffuseColor;
	vec3 lightDirection;
	float padding;
};
uniform Light light;

in vec3 Vs2Ps_normal;
out vec4 fragColor;
void main()
{
	vec4 color = light.ambientColor;
	vec3 lightDir = -light.lightDirection;

	float lightIntensity = dot(Vs2Ps_normal,lightDir);

	if(lightIntensity > 0.0f)
	{
		color += (light.diffuseColor * lightIntensity);
	}

	//color = saturate(color);
	
	fragColor = color;
}	