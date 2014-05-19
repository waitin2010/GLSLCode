#version 400 core

/// in variables
#ifdef PHONE_MODEL
/// gauround shading (or flat shading)
flat in vec4 frontColor;
flat in vec4 backColor;
#else
/// phone shadeing (or smooth shading)
in vec3 pos;
in vec3 N;
#endif 
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
out vec4 fragColor;

vec3 phoneModel(in vec3 pos, in vec3 N)
{

	vec3 L = normalize(light.position - pos);
	float diffuseIndex = max(dot(N,L),0.0);

	vec3 V = normalize(eyePos - pos);
	vec3 halfVector = normalize(L+V);
	float specularIndex = max(0.0,dot(N,halfVector));
	vec3 specular = vec3(0.0);
	if(specularIndex > 0.0)
		specular = pow(specularIndex, mat.shiness) * light.Ls * mat.Ks;

	return light.La * mat.Ka + diffuseIndex * light.Ld * mat.Kd + specular;
}

void main()
{
	vec4 frontColor = vec4(phoneModel(pos,N),1.0);
	vec4 backColor = vec4(phoneModel(pos,-N),1.0);
	if(gl_FrontFacing)
		fragColor = frontColor;
	else
		fragColor = backColor;
}	