#include "camera.h"
using namespace RenderSystem;

Camera::Camera()
{
	degree_ = 45.0f;
	ratio_ = 1.0;
	nearDist_ = 1.0f;
	farDist_ = 1000.0f;
	view_matrix_ = mat4(1.0);
	projection_matrix_ = mat4(1.0);
}
Camera::~Camera()
{
}

mat4 Camera::getViewMatrix()
{
	return view_matrix_;
}
mat4 Camera::getProjectionMatrix()
{
	return projection_matrix_;
}

void Camera::perspective(float degree, float ratio, float nearDist, float farDist)
{
	degree_ = degree;
	ratio_ = ratio;
	nearDist = nearDist;
	farDist = farDist;
	projection_matrix_ = glm::perspective(degree, ratio, nearDist,farDist);
}
void Camera::lookat(vec3 eyePosition, vec3 lookat, vec3 up)
{
	position_ = eyePosition;
	lookat_ = lookat;
	up_ = up;
	view_matrix_ = glm::lookAt(eyePosition,lookat,up);
}

void Camera::setPosition(vec3 eyePosition)
{
	position_ = eyePosition;
	update();
}
void Camera::setLookat(vec3 lookat)
{
	lookat_ = lookat;
	update();
}
void Camera::update()
{
	view_matrix_ = glm::lookAt(position_,lookat_,up_);
	projection_matrix_ = glm::perspective(degree_, ratio_, nearDist_,farDist_);
}
void Camera::moveForward(float speed)
{
	vec3 dir = lookat_ - position_;
	dir = glm::normalize(dir);
	position_ = position_ + dir*speed;
	lookat_ = lookat_ + dir*speed;
	update();
}

void Camera::rotate(float angle)
{
	float x,y,z;
	float a,b,c,k,cosalpha, sinalpha;
	vec3 eye_direction = position_ - lookat_;
	vec3 rotate_axis = glm::cross(eye_direction ,up_);
	rotate_axis = up_;
	rotate_axis = glm::normalize(rotate_axis);
	
	a = rotate_axis.x;
	b = rotate_axis.y;
	c = rotate_axis.z;
	cosalpha = cos(angle);
	sinalpha = sin(angle);
	k = 1 - cosalpha;

	x = (a*a*k+cosalpha)*eye_direction.x  
		+ (a*b*k-c*sinalpha)*eye_direction.y
		+ (a*c*k+b*sinalpha)*eye_direction.z;
	y = (a*b*k+c*sinalpha)*eye_direction.x
		+(b*b*k+cosalpha)*eye_direction.y
		+(b*c*k-a*sinalpha)*eye_direction.z;
	z = (a*c*k-b*sinalpha)*eye_direction.x
		+(b*c*k+a*sinalpha)*eye_direction.y
		+(c*c*k+cosalpha)*eye_direction.z;
	lookat_ = position_ +  vec3(x,y,z) * 10.0f;
	update();
}