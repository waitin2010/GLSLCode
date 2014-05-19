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
uniform sampler2D brickTex;


out vec4 fragColor;

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

void main()
{
	vec3 ambAndDiff;
	vec3 specular;

	phoneModel(pos,N,ambAndDiff, specular);

	vec4 brickColor = texture(brickTex, texCoord);
	vec4 color = vec4(ambAndDiff,1.0) * brickColor + vec4(specular,1.0);

	fragColor =  color;// + vec4(0.2,0.2,0.2,1.0);
}	