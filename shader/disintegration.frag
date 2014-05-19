#version 400 core

/// in variables

/// phone shadeing (or smooth shading)
in vec3 pos;
in vec3 N;
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
uniform sampler2D noiseTex;
uniform float hightthrehold = 0.9;
uniform float lowthrehold = 0.1;

out vec4 fragColor;

void phoneModel(in vec3 pos, in vec3 N,in vec3 diffuse, out vec3 ambientAndDiffuse, out vec3 specular)
{

	vec3 L = normalize(light.position - pos);
	float diffuseIndex = max(dot(N,L),0.0);

	ambientAndDiffuse = light.La * mat.Ka + diffuseIndex * light.Ld * diffuse;

	vec3 V = normalize(eyePos - pos);
	vec3 halfVector = normalize(L+V);
	float specularIndex = max(0.0,dot(N,halfVector));

	specular = vec3(0.0);
	if(specularIndex > 0.0)
		specular = pow(specularIndex, mat.shiness) * light.Ls * mat.Ks;

	
}

void main()
{
	vec4 noise = texture(noiseTex, texCoord);
	if(noise.a > hightthrehold) discard;
	if(noise.a < lowthrehold ) discard;
	vec3 color = mat.Kd;
	

	vec3 ambAndDiff;
	vec3 specular;
	
	phoneModel(pos,N, color, ambAndDiff, specular);



	fragColor =  vec4(ambAndDiff + specular, 1.);
}	