#version 400 core

/// in variables

/// phone shadeing (or smooth shading)
in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord;
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
uniform vec3 eyePos;
uniform sampler2D colorTex;
uniform sampler2D normalMapTex;

out vec4 fragColor;

vec3  phoneModel(vec3 norm, vec3 diffR)
{
	vec3 r = reflect(-lightDir, norm);
	vec3 ambient = light.La * mat.Ka;
	float sDotN = max(dot(lightDir, norm),0.0);
	
	vec3 diffuse = light.Ld * diffR * sDotN;
	
	vec3 spec = vec3(0.0);
	if(sDotN > 0.0)
		spec = light.Ls * mat.Ks * pow (max(dot(r, viewDir), 0.0),mat.shiness);
	return ambient + diffuse + spec;	
}

void main()
{
	vec4 normal = texture(normalMapTex, texCoord);
	vec4 texColor = texture(colorTex, texCoord);
	vec4 color = vec4(phoneModel(normal.xyz, texColor.rgb),1.0);
	fragColor =  color;
}	