#version 400 core

/// in variables

/// phone shadeing (or smooth shading)
in vec3 pos;
in vec3 N;
in vec2 texCoord;

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
uniform vec3 eyePos;
uniform sampler2D renderTex;

uniform float edgeThreshold = 1.0;
uniform int width = 512;
uniform int height = 512;

subroutine vec4 RenderPassType();
subroutine uniform RenderPassType renderPass;

void phoneModel(in vec3 pos, in vec3 N,out vec3 ambientAndDiffuse, out vec3 specular)
{

	vec3 L = normalize(light.position - pos);
	float diffuseIndex = max(dot(N,L),0.0);

	ambientAndDiffuse = light.La * mat.Ka + diffuseIndex * light.Ld;

	vec3 V = normalize(eyePos - pos);
	vec3 halfVector = normalize(L+V);
	float specularIndex = max(0.0,dot(N,halfVector));

	specular = vec3(0.0);
	if(specularIndex > 0.0)
		specular = pow(specularIndex, mat.shiness) * light.Ls * mat.Ks;

	
}
float luma(vec3 color)
{
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}
// pass #1
subroutine (RenderPassType)
vec4 pass1()
{
	//return texture(renderTex,texCoord);
	vec3 amdAndDiff;
	vec3 specular;
	phoneModel(pos,N,amdAndDiff,specular);
	return vec4(amdAndDiff + specular, 1.0);
}

// pass #2
subroutine (RenderPassType)
vec4 pass2()
{
	//return texture(renderTex, texCoord);
	float dx = 1.0 / float (width);
	float dy = 1.0 / float (height);

	float s00 = luma(texture( renderTex,
			          texCoord + vec2(-dx, dy)).rgb);
	float s10 = luma(texture( renderTex,
			          texCoord + vec2(-dx, 0.0)).rgb);
	float s20 = luma(texture( renderTex,
			          texCoord + vec2(-dx, -dy)).rgb);
	float s01 = luma(texture( renderTex,
			          texCoord + vec2(0.0, dy)).rgb);
	float s21 = luma(texture( renderTex,
			          texCoord + vec2(0.0, -dy)).rgb);
	float s02 = luma(texture( renderTex,
			          texCoord + vec2(dx, dy)).rgb);
	float s12 = luma(texture( renderTex,
			          texCoord + vec2(dx, 0.0)).rgb);
	float s22 = luma(texture( renderTex,
			          texCoord + vec2(dx, -dy)).rgb);

	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);

	float dist = sx * sx + sy * sy;

	if (dist > edgeThreshold)
		return vec4(1.0);
	else
		return vec4(0.0,0.0,0.0,1.0);
}
void main()
{
	fragColor = renderPass();
}	