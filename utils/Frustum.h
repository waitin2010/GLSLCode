#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_
#include "Dependencies.h"

typedef vec4 Plane;
namespace RenderSystem{
	
	class Frustum {
	private:
		Plane planes_[6];
	public:
		Frustum();
		~Frustum();

		void constructFrustum(float, mat4 projection_matrix, mat4 view_matrix);

		//bool checkPoint(float, float, float);
		bool checkCube(float, float, float, float);
		//bool checkSphere(float, float, float, float);
		//bool CheckRectangle(float, float, float, float, float, float);

	};
}
#endif