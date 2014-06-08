#include "camera.h"
using namespace RenderSystem;

Camera::Camera()
{
	setDefaultCamera();
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

void Camera::perspective(float angle, float ratio, float nearDist, float farDist)
{
	angle_ = angle;
	ratio_ = ratio;
	nearDist = nearDist;
	farDist = farDist;
	projection_matrix_ = glm::perspective(angle, ratio, nearDist,farDist);
}
void Camera::lookat(vec3 eyePosition, vec3 lookat, vec3 up)
{
	eye_ = eyePosition;
	
	n_ = eyePosition - lookat;
	u_ = glm::cross(up,n_);
	v_ = glm::cross(n_,u_);
	n_ = glm::normalize(n_);
	u_ = glm::normalize(u_);
	v_ = glm::normalize(v_);
	updateViewMatrix();
}


void Camera::updateViewMatrix()
{
	
	vec3 eVec = vec3(eye_.x, eye_.y, eye_.z); 

	mat4 m;
	m = mat4(u_.x,v_.x,n_.x,0.0f,
			u_.y, v_.y,n_.y,0.0f,
			u_.z, v_.z, n_.z, 0.0f,
			-glm::dot(eVec,u_), -glm::dot(eVec,v_),-glm::dot(eVec,n_),1.0f);
	view_matrix_= m;
	/*float m[16];
	m[0] = u_.x; m[4] = u_.y; m[8] = u_.z; m[12] = -glm::dot(eVec,u_); 
	m[1] = v_.x; m[5] = v_.y; m[9] = v_.z; m[13] = -glm::dot(eVec,v_); 
	m[2] = n_.x; m[6] = n_.y; m[10] = n_.z; m[14] = -glm::dot(eVec,n_); 
	m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f; 
	view_matrix_= mat4(m[0],m[4],m[8],m[12],
		m[1],m[5],m[9],m[13],
		m[2],m[6],m[10],m[14],
		m[3],m[7],m[11],m[15]);*/
	
}
void Camera::updateProjectionMatrix()
{
	projection_matrix_ = glm::perspective(angle_, ratio_, nearDist_,farDist_);
}
void Camera::slide(double du, double dv, double dn) 
{ // slide both eye and lookAt by amount du * u + dv * v + dn * n; 

	eye_.x += du * u_.x + dv * v_.x + dn * n_.x;
	eye_.y += du * u_.y + dv * v_.y + dn * n_.y;
	eye_.z += du * u_.z + dv * v_.z + dn * n_.z;
	updateViewMatrix();
} 

void Camera::rotAxes(vec3& a, vec3& b, float angle)
{
	// rotate orthogonal vectors a (like x axis) and b(like y axia) through angle degrees 
	float ang = 3.14159265/180*angle; 
	float C = cos(ang), S = sin(ang); 
	vec3 t(C * a.x + S * b.x, C * a.y + S * b.y, C * a.z + S * b.z); 
	b = vec3(-S * a.x + C * b.x, -S * a.y + C * b.y, 
		-S * a.z + C * b.z); 
	a = vec3(t.x, t.y, t.z); // put tmp into a' 
}
void Camera::roll(float angle)
{
	rotAxes(u_,v_,angle);
	updateViewMatrix();
}

void Camera::pitch(float angle)
{
	rotAxes(n_,v_,angle);
	updateViewMatrix();
}

void Camera::yaw(float angle)
{
	rotAxes(u_,n_,angle);
	updateViewMatrix();
}
void Camera :: setDefaultCamera(void) 
{ // make default camera 

	perspective(45.0f, 640/(float)480, 0.1f, 200.0f); // good default values here 

	vec3 eyePoint = vec3( 10.0, 0.0, 0.0 );
	vec3 lookPoint = vec3( 0.0, 0.0, 0.0 );
	vec3 upVector = vec3( 0, 1, 0 );

	lookat( eyePoint, lookPoint, upVector );

	//set(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0, 1, 0); 
} 