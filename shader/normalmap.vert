#version 400 core
/// for gaurond shading 
/// add #define PHONE_MODEL in both vertex shader and fragment shader

/// vertex attributes
layout (location = 0) in  vec4 position;
layout (location = 1) in  vec3 normal;
layout (location = 2) in  vec2 vertexTexCoord;
layout (location = 3) in  vec3 vertexTangent;
/// output data

/// phone shadeing (or smooth shading)
out vec3 lightDir;
out vec3 viewDir;
out vec2 texCoord;

struct LightInfo{
	vec3 position; /// light position
	vec3 La;    /// light ambient
	vec3 Ld;    /// light diffuse
	vec3 Ls;    /// light specular
	};
uniform LightInfo light;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;



void main()
{
	vec3 norm = normalize(mat3(model_matrix) * normal);
	vec3 tang = normalize(mat3(model_matrix) * vertexTangent);
	vec3 binormal = normalize(cross(norm, tang));

	mat3 toObjectLocal = mat3(
		tang.x, binormal.x, norm.x,
		tang.y, binormal.y, norm.y,
		tang.z, binormal.z, norm.z);

	vec3 pos = vec3(view_matrix * model_matrix * position);

	lightDir = normalize(toObjectLocal * (light.position - pos));
	viewDir  = toObjectLocal * normalize(-pos);
	texCoord = vertexTexCoord;
	gl_Position = projection_matrix * view_matrix * model_matrix * position;
}