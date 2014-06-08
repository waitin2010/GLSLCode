#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Dependencies.h"

using  glm::vec3;
using  glm::mat4;

namespace RenderSystem{

class Camera{
private:
	mat4 view_matrix_;
	mat4 projection_matrix_;

	/// View matrix
	vec3 eye_;
	vec3 u_;
	vec3 v_;
	vec3 n_;

	/// Projection Matrix
	float angle_;
	float ratio_;
	float nearDist_;
	float farDist_;

public:
	Camera();
	~Camera();

	mat4 getViewMatrix();
	mat4 getProjectionMatrix();

	void perspective(float angle, float ratio, float nearDist, float farDist);
	void lookat(vec3 eyePosition, vec3 lookat, vec3 up);

	void roll(float angle);
	void pitch(float angle);
	void yaw(float angle);
	void slide(double du, double dv, double dn);

private:
	void updateViewMatrix();
	void updateProjectionMatrix();
	void rotAxes(vec3 &a, vec3 &b, float angle);
	void setDefaultCamera(); 

};
}
#endif