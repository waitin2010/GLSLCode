#version 400 core
/// for gaurond shading 
/// add #define PHONE_MODEL in both vertex shader and fragment shader

/// vertex attributes
layout (location = 0) in  vec4 position;
layout (location = 1) in  vec3 normal;

/// output data
#ifdef PHONE_MODEL
/// gauround shading (or flat shading)
flat out vec4 frontColor;
flat out vec4 backColor;
#else
/// phone shadeing (or smooth shading)
out vec4 frontColor;
out vec4 backColor;
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

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform vec3 eyePos;

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
	vec3 pos = vec3(model_matrix *  position);
	vec3 N = normalize(mat3(model_matrix) * normal);

	frontColor = vec4(phoneModel(pos,N),1.0);
	backColor = vec4(phoneModel(pos,-N),1.0);

	gl_Position = projection_matrix * view_matrix * model_matrix * position;
}