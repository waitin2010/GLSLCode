#version 400 core
layout (location = 0) in vec3 vertexInitVel;
layout (location = 1) in float startTime;

out float transp;

uniform float time;
uniform vec3 gravity = vec3(0.0, -0.05, 0.0);
uniform float particleLifeTime = 3.15;
uniform mat4 MVP;

void main()
{
	vec3 pos = vec3(0.0);
	transp = 0.0;

	if(time > startTime) {
		float t = time - startTime;
		if(t < particleLifeTime) {
			pos = vertexInitVel * t + gravity * t * t;
			transp = 1.0 - t/particleLifeTime;
			}
	}
	gl_Position = MVP * vec4(pos, 1.0);
}