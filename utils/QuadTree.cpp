#include "QuadTree.h"
using namespace RenderSystem;

const int MAX_TRIANGLES = 1000;

QuadTree::QuadTree()
{
	position_ = 0;
	normal_ = 0;
	textureCoord_ = 0;
	root_ = 0;
}

QuadTree::~QuadTree()
{

}

bool QuadTree::initialize(int vertexCount, vec3 *position, vec3 *normal, vec2 *textureCoord )
{
	
	float centerX, centerZ, width;
	// Get the number of vertices in the terrain vertex array.
	//vertexCount = terrain->GetVertexCount();

	// Store the total triangle count for the vertex list.
	triangleCount_ = vertexCount / 3;

	// Create a vertex array to hold all of the terrain vertices.
	//vertexList_ = new VertexType[vertexCount];
	position_ = position;
	normal_ = normal;
	textureCoord_ = textureCoord;

	// Copy the terrain vertices into the vertex list.
	//terrain->CopyVertexArray((void*)vertexList_);

	// calculate the center x, z and the width of the mesh
	calculateMeshDimensions(vertexCount,centerX, centerZ, width);

	// Create the root node for the quad tree
	root_ = new NodeType;
	if(!root_)
	{
		return false;
	}

	// Recursively build the quad tree based on the vertex list data
	// and mesh dimensions
	createTreeNode(root_,centerX,centerZ, width);

	// Release the vertex list since the quad tree now has the vertices 
	// in each node
	if(position_){
		delete [] position_;
		position_ = 0;
	}
	if(normal_) {
		delete [] normal_;
		normal_ = 0;
	}
	if(textureCoord_){
		delete [] textureCoord_;
		textureCoord_ = 0;
	}
	return true;
}

void QuadTree::release()
{
	// Recursively release the quad tree data
	if(root_){
		releaseNode(root_);
		delete root_;
		root_ = 0;
	}
	return ;
}

void QuadTree::render(Frustum *frustum)
{
	// Reset the number of triangles that are drawn for this frame
	drawCount_ = 0;

	// Render each node that is visible starting at the root node
	// and moving down the tree
	renderNode(root_,frustum);

	return ;
}

int QuadTree::getDrawCount()
{
	return drawCount_;
}

void QuadTree::calculateMeshDimensions(int vertexCount, float & centerX, float &centerZ, float &meshwidth)
{
	int i;
	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;


	// Initialize the center position of the mesh to zero.
	centerX = 0.0f;
	centerZ = 0.0f;

	// Sum all the vertices in the mesh.
	for(i=0; i<vertexCount; i++)
	{
		centerX += position_[i].x;
		centerZ += position_[i].z;
	}

	// And then divide it by the number of vertices to find the mid-point of the mesh.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// Initialize the maximum and minimum size of the mesh.
	maxWidth = 0.0f;
	maxDepth = 0.0f;

	minWidth = fabsf(position_[0].x - centerX);
	minDepth = fabsf(position_[0].z - centerZ);

	// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
	for(i=0; i<vertexCount; i++)
	{
		width = fabsf(position_[i].x - centerX);	
		depth = fabsf(position_[i].z - centerZ);	

		if(width > maxWidth) { maxWidth = width; }
		if(depth > maxDepth) { maxDepth = depth; }
		if(width < minWidth) { minWidth = width; }
		if(depth < minDepth) { minDepth = depth; }
	}

	// Find the absolute maximum value between the min and max depth and width.
	maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	// Calculate the maximum diameter of the mesh.
	meshwidth = max(maxX, maxZ) * 2.0f;

	return;
}

void QuadTree::createTreeNode(NodeType* node, float positionX, float positionZ, float width)
{
	int numTriangles, i, count, vertexCount, index, vertexIndex;
	float offsetX, offsetZ;
	unsigned long* indices;
	bool result;

	// Store the node position and size.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// Initialize the triangle count to zero for the node.
	node->triangleCount = 0;

	// Initialize the vertex and index buffer to null.
	node->vao = 0;
	node->vbo = 0;

	// Initialize the children nodes of this node to null.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// Count the number of triangles that are inside this node.
	numTriangles = countTriangles(positionX, positionZ, width);

	// Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
	if(numTriangles == 0)
	{
		return;
	}

	// Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
	if(numTriangles > MAX_TRIANGLES)
	{
		for(i=0; i<4; i++)
		{
			// Calculate the position offsets for the new child node.
			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			// See if there are any triangles in the new node.
			count = countTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if(count > 0)
			{
				// If there are triangles inside where this new node would be then create the child node.
				node->nodes[i] = new NodeType;

				// Extend the tree starting from this new child node now.
				createTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			}
		}

		return;
	}
	// Case 3: If this node is not empty and the triangle count for it is less than the max then 
	// this node is at the bottom of the tree so create the list of triangles to store in it.
	node->triangleCount = numTriangles;

	// Calculate the number of vertices.
	vertexCount = numTriangles * 3;

	// Create the vertex array.
	vec3 *position = new vec3[vertexCount];
	vec3 *normal = new vec3[vertexCount];
	vec2 *textureCoord = new vec2[vertexCount];

	// Create the index array.
	indices = new unsigned long[vertexCount];

	// Initialize the index for this new vertex and index array.
	index = 0;

	// Go through all the triangles in the vertex list.
	for(i=0; i<triangleCount_; i++)
	{
		// If the triangle is inside this node then add it to the vertex array.
		result = isTriangleContained(i, positionX, positionZ, width);
		if(result == true)
		{
			// Calculate the index into the terrain vertex list.
			vertexIndex = i * 3;

			// Get the three vertices of this triangle from the vertex list.
			position[index] = position_[vertexIndex];
			textureCoord[index] = textureCoord_[vertexIndex];
			normal[index] = normal_[vertexIndex];
			indices[index] = index;
			index++;

			vertexIndex++;
			position[index] = position_[vertexIndex];
			textureCoord[index] = textureCoord_[vertexIndex];
			normal[index] = normal_[vertexIndex];
			indices[index] = index;
			index++;

			vertexIndex++;
			position[index] = position_[vertexIndex];
			textureCoord[index] = textureCoord_[vertexIndex];
			normal[index] = normal_[vertexIndex];
			indices[index] = index;
			index++;
		}
	}

	glGenVertexArrays(1,&node->vao);
	glBindVertexArray(node->vao);
	GLuint ab;
	glGenBuffers(1,&ab);
	glBindBuffer(GL_ARRAY_BUFFER,ab);
	glBufferData(GL_ARRAY_BUFFER,vertexCount*sizeof(vec3),position,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(0);

	GLuint ab2;
	glGenBuffers(1,&ab2);
	glBindBuffer(GL_ARRAY_BUFFER,ab2);
	glBufferData(GL_ARRAY_BUFFER,vertexCount*sizeof(vec3),normal,GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);

	GLuint ab3;
	glGenBuffers(1,&ab3);
	glBindBuffer(GL_ARRAY_BUFFER,ab3);
	glBufferData(GL_ARRAY_BUFFER,vertexCount*sizeof(vec2),textureCoord,GL_STATIC_DRAW);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1,&node->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,node->vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertexCount*sizeof(unsigned long),indices,GL_STATIC_DRAW);
	glBindVertexArray(0);

	delete [] position;
	position = 0;
	delete [] normal;
	normal = 0;
	delete [] textureCoord;
	textureCoord = 0;
	

	delete [] indices;
	indices = 0;

	return ;
}

int QuadTree::countTriangles(float positionX, float positionZ, float width)
{
	int count, i;
	bool result;


	// Initialize the count to zero.
	count = 0;

	// Go through all the triangles in the entire mesh and check which ones should be inside this node.
	for(i=0; i<triangleCount_; i++)
	{
		// If the triangle is inside the node then increment the count by one.
		result = isTriangleContained(i, positionX, positionZ, width);
		if(result == true)
		{
			count++;
		}
	}

	return count;
}

bool QuadTree::isTriangleContained(int index, float positionX, float positionZ, float width)
{
	float radius;
	int vertexIndex;
	float x1, z1, x2, z2, x3, z3;
	float minimumX, maximumX, minimumZ, maximumZ;


	// Calculate the radius of this node.
	radius = width / 2.0f;

	// Get the index into the vertex list.
	vertexIndex = index * 3;

	// Get the three vertices of this triangle from the vertex list.
	x1 = position_[vertexIndex].x;
	z1 = position_[vertexIndex].z;
	vertexIndex++;

	x2 = position_[vertexIndex].x;
	z2 = position_[vertexIndex].z;
	vertexIndex++;

	x3 = position_[vertexIndex].x;
	z3 = position_[vertexIndex].z;

	// Check to see if the minimum of the x coordinates of the triangle is inside the node.
	minimumX = min(x1, min(x2, x3));
	if(minimumX > (positionX + radius))
	{
		return false;
	}

	// Check to see if the maximum of the x coordinates of the triangle is inside the node.
	maximumX = max(x1, max(x2, x3));
	if(maximumX < (positionX - radius))
	{
		return false;
	}

	// Check to see if the minimum of the z coordinates of the triangle is inside the node.
	minimumZ = min(z1, min(z2, z3));
	if(minimumZ > (positionZ + radius))
	{
		return false;
	}

	// Check to see if the maximum of the z coordinates of the triangle is inside the node.
	maximumZ = max(z1, max(z2, z3));
	if(maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}

void QuadTree::releaseNode(NodeType* node)
{
	int i;


	// Recursively go down the tree and release the bottom nodes first.
	for(i=0; i<4; i++)
	{
		if(node->nodes[i] != 0)
		{
			releaseNode(node->nodes[i]);
		}
	}

	

	// Release the four child nodes.
	for(i=0; i<4; i++)
	{
		if(node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}

	return;
}

void QuadTree::renderNode(NodeType* node, Frustum* frustum)
{
	bool result;
	int count, i, indexCount;

	result = true;
	// Check to see if the node can be viewed, height doesn't matter in a quad tree.
	result = frustum->checkCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));
	
	// If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
	if(!result)
	{
		return;
	}

	// If it can be seen then check all four child nodes to see if they can also be seen.
	count = 0;
	for(i=0; i<4; i++)
	{
		if(node->nodes[i] != 0)
		{
			count++;
			renderNode(node->nodes[i],frustum);
		}
	}

	// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
	if(count != 0)
	{
		return;
	}

	// Otherwise if this node can be seen and has triangles in it then render these triangles.

	


	glBindVertexArray(node->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,node->vbo);


	// Determine the number of indices in this node.
	indexCount = node->triangleCount * 3;

	// Call the terrain shader to render the polygons in this node.
	glDrawElements(GL_TRIANGLES,indexCount,GL_UNSIGNED_INT,NULL);

	// Increase the count of the number of polygons that have been rendered during this frame.
	drawCount_ += node->triangleCount;

	return;
}