#version 400 core

/// in variables

/// phone shadeing (or smooth shading)
in vec3 reflectDir;
in vec3 refractDir;

out vec4 fragColor;

/// uniform data
struct LightInfo{
	vec3 position; /// light position
	vec3 La;    /// light ambient
	vec3 Ld;    /// light diffuse
	vec3 Ls;    /// light specular
	};
uniform LightInfo light;

struct MaterialInfo {
	vec3 Ka;   // material ambient
	vec3 Kd;   // material diffuse
	vec3 Ks;   // material specular
	float shiness;
	};
uniform MaterialInfo mat;


uniform samplerCube cubeMap;
uniform bool skyBox;


void main()
{
	vec4 reflectColor = texture(cubeMap, reflectDir);
	vec4 refractColor = texture(cubeMap, refractDir);
	vec4 color;
	//skyBox = true;
	if(skyBox)
	{ 
		color  = reflectColor;
	}else
	{
		color = mix(reflectColor,refractColor, 0.1);
	}
	fragColor =  color;
}	