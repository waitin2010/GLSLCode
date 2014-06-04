#ifndef _QUADTREE_H_
#define _QUADTREE_H_
#include "Dependencies.h"
#include "RenderSystem.h"
#include "Frustum.h"
using namespace RenderSystem;
namespace RenderSystem{
	
	struct NodeType{
		float positionX, positionZ, width;
		int triangleCount;
		GLuint vao, vbo;
		NodeType *nodes[4];
	};
	class QuadTree {

	private:
		int triangleCount_;
		int drawCount_;
		vec3 *position_;
		vec3 *normal_;
		vec2 *textureCoord_;
		NodeType *root_;
	public:
		QuadTree();
		~QuadTree();

		bool initialize(int vertexCount, vec3 *position, vec3 *normal, vec2 *textureCoord);
		void render(Frustum *frustum);
		void release();
		int getDrawCount();
	private:
		void calculateMeshDimensions(int , float &, float &, float &);
		void createTreeNode(NodeType *, float, float, float);
		int countTriangles(float , float, float);
		bool isTriangleContained(int, float, float, float);
		void releaseNode(NodeType*);
		void renderNode(NodeType* node, Frustum *frustum);



	};
}
#endif