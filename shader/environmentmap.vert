#version 400 core
/// for gaurond shading 
/// add #define PHONE_MODEL in both vertex shader and fragment shader

/// vertex attributes
layout (location = 0) in  vec4 position;
layout (location = 1) in  vec3 normal;
layout (location = 2) in  vec2 vertexTexCoord;
/// output data

/// phone shadeing (or smooth shading)
out vec3 reflectDir;
out vec3 refractDir;
uniform bool skyBox;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform vec3 worldCameraPosition;
uniform vec3 eyePos;
void main()
{
	//skyBox = true;
	if(skyBox)
	{
		reflectDir = vec3(position);
	}else
	{	
		vec3 worldPos = vec3 (model_matrix * position);
		vec3 worldNorm = vec3(model_matrix * vec4(normal, 1.0));
		vec3 worldView = normalize(eyePos - worldPos);
		/// output 
		reflectDir = reflect(-worldView, worldNorm);
		refractDir = refract(worldView, worldNorm, 0.9);
	}

	gl_Position = projection_matrix * view_matrix * model_matrix * position;

}