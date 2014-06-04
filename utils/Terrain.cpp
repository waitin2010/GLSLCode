#include "Terrain.h"
using namespace RenderSystem;
Terrain::Terrain()
{

}

Terrain::~Terrain()
{

}

void Terrain::render()
{
	glBindVertexArray(Vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eb);
	glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
}


void Terrain::normalizeHeightMap()
{
	int i, j;


	for(j=0; j<height_; j++)
	{
		for(i=0; i<width_; i++)
		{
			heightMap_[(height_ * j) + i].y /= 15.0f;
		}
	}

	return;
}
bool Terrain::calculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(height_-1) * (width_-1)];
	if(!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for(j=0; j<(height_-1); j++)
	{
		for(i=0; i<(width_-1); i++)
		{
			index1 = (j * height_) + i;
			index2 = (j * height_) + (i+1);
			index3 = ((j+1) * height_) + i;

			// Get three vertices from the face.
			vertex1[0] = heightMap_[index1].x;
			vertex1[1] = heightMap_[index1].y;
			vertex1[2] = heightMap_[index1].z;

			vertex2[0] = heightMap_[index2].x;
			vertex2[1] = heightMap_[index2].y;
			vertex2[2] = heightMap_[index2].z;

			vertex3[0] = heightMap_[index3].x;
			vertex3[1] = heightMap_[index3].y;
			vertex3[2] = heightMap_[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (height_-1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for(j=0; j<height_; j++)
	{
		for(i=0; i<width_; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (height_-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (width_-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (height_-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (height_-1)))
			{
				index = (j * (height_-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if((i < (width_-1)) && (j < (height_-1)))
			{
				index = (j * (height_-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * height_) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			heightMap_[index].nx = (sum[0] / length);
			heightMap_[index].ny = (sum[1] / length);
			heightMap_[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete [] normals;
	normals = 0;

	return true;
}

void Terrain::calculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)width_;

	// Calculate how many times to repeat the texture.
	incrementCount = width_ / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for(j=0; j<height_; j++)
	{
		for(i=0; i<width_; i++)
		{
			// Store the texture coordinate in the height map.
			heightMap_[(height_ * j) + i].tu = tuCoordinate;
			heightMap_[(height_ * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if(tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if(tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}
bool Terrain::loadHeightMap(unsigned char *heightMap,int width, int height)
{
	int i, j,k;
	int index;
	width_ = width;
	height_ = height;
	// Create the structure to hold the height map data.
	heightMap_ = new HeightMapType[width_ * height_];
	if(!heightMap_)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k=0;

	// Read the image data into the height map.
	for(j=0; j<height_; j++)
	{
		for(i=0; i<width_; i++)
		{
			height = heightMap[k];

			index = (height_ * j) + i;

			heightMap_[index].x = (float)i;
			heightMap_[index].y = (float)height;
			heightMap_[index].z = (float)j;

			k++;
		}
	}
	return true;
}

bool Terrain::initialize(unsigned char* heightMap,int width, int height, char *texturefile)
{
	loadHeightMap(heightMap,width, height);
	normalizeHeightMap();
	calculateNormals();
	calculateTextureCoordinates();
	initBuffer();
	//texture->create(texturefile);
	return true;
	
}
bool Terrain::initBuffer()
{
	
	unsigned long* indices;
	int index, i, j;
	int index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	vertexCount_ = (width_ - 1) * (height_ - 1) * 6;

	// Set the index count to the same as the vertex count.
	indexCount_ = vertexCount_;

	// Create the vertex array.
	position = new vec3[vertexCount_];
	normal = new vec3[vertexCount_];
	textureCoord = new vec2[vertexCount_];
	if(!position || !normal || !textureCoord)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount_];
	if(!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;
	// Load the vertex and index array with the terrain data.
	for(j=0; j<(height_-1); j++)
	{
		for(i=0; i<(width_-1); i++)
		{
			index1 = (height_ * j) + i;          // Bottom left.
			index2 = (height_ * j) + (i+1);      // Bottom right.
			index3 = (height_ * (j+1)) + i;      // Upper left.
			index4 = (height_ * (j+1)) + (i+1);  // Upper right.

			// Upper left.
			tv = heightMap_[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if(tv == 1.0f) { tv = 0.0f; }

			position[index] = vec3(heightMap_[index3].x, heightMap_[index3].y, heightMap_[index3].z);
			textureCoord[index] = vec2(heightMap_[index3].tu, tv);
			normal[index] = vec3(heightMap_[index3].nx, heightMap_[index3].ny, heightMap_[index3].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = heightMap_[index4].tu;
			tv = heightMap_[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if(tu == 0.0f) { tu = 1.0f; }
			if(tv == 1.0f) { tv = 0.0f; }

			position[index] = vec3(heightMap_[index4].x, heightMap_[index4].y, heightMap_[index4].z);
			textureCoord[index] = vec2(tu, tv);
			normal[index] = vec3(heightMap_[index4].nx, heightMap_[index4].ny, heightMap_[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			position[index] = vec3(heightMap_[index1].x, heightMap_[index1].y, heightMap_[index1].z);
			textureCoord[index] = vec2(heightMap_[index1].tu, heightMap_[index1].tv);
			normal[index] = vec3(heightMap_[index1].nx, heightMap_[index1].ny, heightMap_[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			position[index] = vec3(heightMap_[index1].x, heightMap_[index1].y, heightMap_[index1].z);
			textureCoord[index] = vec2(heightMap_[index1].tu, heightMap_[index1].tv);
			normal[index] = vec3(heightMap_[index1].nx, heightMap_[index1].ny, heightMap_[index1].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = heightMap_[index4].tu;
			tv = heightMap_[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if(tu == 0.0f) { tu = 1.0f; }
			if(tv == 1.0f) { tv = 0.0f; }

			position[index] = vec3(heightMap_[index4].x, heightMap_[index4].y, heightMap_[index4].z);
			textureCoord[index] = vec2(tu, tv);
			normal[index] = vec3(heightMap_[index4].nx, heightMap_[index4].ny, heightMap_[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right.
			tu = heightMap_[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if(tu == 0.0f) { tu = 1.0f; }

			position[index] = vec3(heightMap_[index2].x, heightMap_[index2].y, heightMap_[index2].z);
			textureCoord[index] = vec2(tu, heightMap_[index2].tv);
			normal[index] = vec3(heightMap_[index2].nx, heightMap_[index2].ny, heightMap_[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	glGenVertexArrays(1,&Vao);
	glBindVertexArray(Vao);
	GLuint ab;
	glGenBuffers(1,&ab);
	glBindBuffer(GL_ARRAY_BUFFER,ab);
	glBufferData(GL_ARRAY_BUFFER,vertexCount_*sizeof(vec3),position,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(0);

	GLuint ab2;
	glGenBuffers(1,&ab2);
	glBindBuffer(GL_ARRAY_BUFFER,ab2);
	glBufferData(GL_ARRAY_BUFFER,vertexCount_*sizeof(vec3),normal,GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);

	GLuint ab3;
	glGenBuffers(1,&ab3);
	glBindBuffer(GL_ARRAY_BUFFER,ab3);
	glBufferData(GL_ARRAY_BUFFER,vertexCount_*sizeof(vec2),textureCoord,GL_STATIC_DRAW);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1,&eb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexCount_*sizeof(unsigned long),indices,GL_STATIC_DRAW);
	glBindVertexArray(0);

	//free(position);
	//free(normal);
	//free(textureCoord);

	return true;
}
void Terrain::copyVertexArray(vec3 *positionOut, vec3* normalOut, vec2 *textureCoordOut)
{
	memcpy(positionOut, position, sizeof(vec3) * vertexCount_);
	memcpy(normalOut,normal,sizeof(vec3)*vertexCount_);
	memcpy(textureCoordOut,textureCoord,sizeof(vec2)*vertexCount_);
	return;
}
int Terrain::getVertexCount()
{
	return vertexCount_;
}