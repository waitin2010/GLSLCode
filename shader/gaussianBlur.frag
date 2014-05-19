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
uniform float PixOffset[5] = float[](0.0,1.0,2.0,3.0,4.0);
uniform float weight[5];
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
	//return texture(renderTex,texCoord);
	float dy = 1.0/ float (height);

	vec4 sum = texture(renderTex, texCoord) * weight[0];

	for(int i = 1; i < 5; i++)
	{
		sum += texture(renderTex,texCoord + vec2(0.0, PixOffset[i] * dy)) * weight[i];
		sum += texture(renderTex, texCoord - vec2(0.0, PixOffset[i] * dy )) * weight[i];
	}
	return sum;
}

subroutine (RenderPassType)
vec4 pass3()
{
	return texture(renderTex,texCoord);
	float dx = 1.0 / float (width);

	vec4 sum = texture(renderTex, texCoord) * weight[0];

	for(int i =1; i<5; i++)
	{
		sum += texture(renderTex, texCoord + vec2(PixOffset[i] * dx, 0.0))* weight[i];
		sum += texture(renderTex, texCoord - vec2(PixOffset[i] * dx , 0.0))* weight[i];
	}
	return sum;
}
void main()
{
	fragColor = renderPass();
}	