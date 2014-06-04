#include "Frustum.h"
using namespace RenderSystem;

Frustum::Frustum()
{

}

Frustum::~Frustum()
{

}

void Frustum::constructFrustum(float screenDepth, mat4 projection_matrix, mat4 view_matrix)
{
	mat4 matrix;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = projection_matrix * view_matrix;

	// Calculate near plane of frustum.
	planes_[0].x = matrix[0][3] + matrix[0][2];
	planes_[0].y = matrix[1][3] + matrix[1][2];
	planes_[0].z = matrix[2][3] + matrix[2][2];
	planes_[0].w = matrix[3][3] + matrix[3][2];
	planes_[0] = vec4(glm::normalize(vec3(planes_[0])),planes_[0].w);

	// Calculate far plane of frustum.
	planes_[1].x = matrix[0][3] - matrix[0][2];
	planes_[1].y = matrix[1][3] - matrix[1][2];
	planes_[1].z = matrix[2][3] - matrix[2][2];
	planes_[1].w = matrix[3][3] - matrix[3][2];
	planes_[1] = vec4(glm::normalize(vec3(planes_[1])),planes_[1].w);

	// Calculate left plane of frustum.
	planes_[2].x = matrix[0][3] + matrix[0][0];
	planes_[2].y = matrix[1][3] + matrix[1][0];
	planes_[2].z = matrix[2][3] + matrix[2][0];
	planes_[2].w = matrix[3][3] + matrix[3][0];
	planes_[2] = vec4(glm::normalize(vec3(planes_[2])),planes_[2].w);

	// Calculate right plane of frustum.
	planes_[3].x = matrix[0][3] - matrix[0][0];
	planes_[3].y = matrix[1][3] - matrix[1][0];
	planes_[3].z = matrix[2][3] - matrix[2][0];
	planes_[3].w = matrix[3][3] - matrix[3][0];
	planes_[3] = vec4(glm::normalize(vec3(planes_[3])),planes_[3].w);

	// Calculate top plane of frustum.
	planes_[4].x = matrix[0][3] - matrix[0][1];
	planes_[4].y = matrix[1][3] - matrix[1][1];
	planes_[4].w = matrix[2][3] - matrix[2][1];
	planes_[4].z = matrix[3][3] - matrix[3][1];
	planes_[4] = vec4(glm::normalize(vec3(planes_[4])),planes_[4].w);;

	// Calculate bottom plane of frustum.
	planes_[5].x = matrix[0][3] + matrix[0][1];
	planes_[5].y = matrix[1][3] + matrix[1][1];
	planes_[5].w = matrix[2][3] + matrix[2][1];
	planes_[5].z = matrix[3][3] + matrix[3][1];
	planes_[5] = vec4(glm::normalize(vec3(planes_[5])),planes_[5].w);

	return;
}

bool Frustum::checkCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;


	// Check if any one point of the cube is in the view frustum.
	for(i=0; i<6; i++) 
	{
		if(glm::dot(vec3(planes_[i]), vec3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
			
		}

		if(glm::dot(vec3(planes_[i]), vec3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if(glm::dot(vec3(planes_[i]), vec3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if(glm::dot(vec3(planes_[i]), vec3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if(glm::dot(vec3(planes_[i]), vec3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if(glm::dot(vec3(planes_[i]), vec3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if(glm::dot(vec3(planes_[i]), vec3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if(glm::dot(vec3(planes_[i]), vec3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}