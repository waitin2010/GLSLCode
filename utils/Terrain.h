#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include "Dependencies.h"

namespace RenderSystem{

	struct HeightMapType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct VectorType{
		float x, y, z;
	};
	const int TEXTURE_REPEAT = 64;
	class Terrain {
	private:
		int width_;
		int height_;
		int vertexCount_;
		int indexCount_;

		HeightMapType *heightMap_;
		//Texture *texture;

		GLuint Vao;
		GLuint eb;

	public:
		Terrain();
		~Terrain();
		void render();
		bool initialize(unsigned char* heightMap,int width, int height, char *texturefile);
		bool release();

	private:
		bool initBuffer();
		bool loadHeightMap(unsigned char *heightMap, int width, int height);
		void normalizeHeightMap();
		bool calculateNormals();
		void calculateTextureCoordinates();
	};
}
#endif