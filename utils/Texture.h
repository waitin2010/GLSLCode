#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "Dependencies.h"
#include "RenderSystem.h"

namespace RenderSystem{
	enum TextureType{
		Texture_1D,
		Texture_2D, 
		Texture_3D,
		Texture_Cube
	};
	class Texture {
	public:
		Texture();
		~Texture();

		bool initialize(char *filename,TextureType texture_type = Texture_2D);
		bool release();
		void enable();
		void disable();

		GLuint getTextureID();
	private:
		GLuint texture_id_;
		TextureType texture_type_;
	};
}
#endif