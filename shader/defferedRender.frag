#version 400 core

/// in variables

/// phone shadeing (or smooth shading)
in vec3 pos;
in vec3 N;
in vec2 texCoord;

layout (location = 0)  out vec4 fragColor;
layout (location = 1) out vec3 positionData;
layout (location = 2) out vec3 normalData;
layout (location = 3) out vec3 colorData;

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
uniform sampler2D positionTex, normalTex, colorTex;



subroutine void RenderPassType();
subroutine uniform RenderPassType renderPass;

vec3 phoneModel(in vec3 pos, in vec3 N,in vec3 diffuseColor)
{

	vec3 L = normalize(light.position - pos);
	float diffuseIndex = max(dot(N,L),0.0);

	return  diffuseIndex * diffuseColor;
}

// pass #1
subroutine (RenderPassType)
void pass1()
{
	positionData = pos;
	normalData   = N;
	colorData    = mat.Kd;
}

// pass #2
subroutine (RenderPassType)
void pass2()
{
	//return texture(renderTex,texCoord);
	vec3 pos2 = vec3(texture(positionTex, texCoord));
	vec3 N2 =   vec3(texture(normalTex, texCoord));
	vec3 diffuseColor = vec3(texture(colorTex, texCoord));
	vec3 amdAndDiff;
	vec3 specular;
	vec4 color = vec4(phoneModel(pos2,N2,diffuseColor),1.0);
	fragColor = color;
}
void main()
{
	 renderPass();
}	