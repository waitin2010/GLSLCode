#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using  glm::vec3;
using  glm::mat4;

namespace RenderSystem{

class Camera{
private:
	mat4 view_matrix_;
	mat4 projection_matrix_;

	vec3 position_;
	vec3 lookat_;
	vec3 up_;

	float degree_;
	float ratio_;
	float nearDist_;
	float farDist_;
public:
	Camera();
	~Camera();
	mat4 getViewMatrix();
	mat4 getProjectionMatrix();
	void perspective(float degree, float ratio, float nearDist, float farDist);
	void lookat(vec3 eyePosition, vec3 lookat, vec3 up);
	void setPosition(vec3 eyePosition);
	void setLookat(vec3 lookat);
	void moveForward(float speed);
	void rotate(float angle);
private:
	void update();
};
}
#endif