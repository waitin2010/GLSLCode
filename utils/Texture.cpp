#include "Texture.h"
using namespace RenderSystem;


Texture::Texture()
{

}

Texture::~Texture()
{

}

bool Texture::initialize(char *filename,TextureType texture_type)
{
	texture_id_ = LoadGLTextures(filename);
	texture_type_ = texture_type;

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	return true;
}

void Texture::enable()
{
	switch(texture_type_){
	case Texture_2D:
		glBindTexture(GL_TEXTURE_2D,texture_id_);
		break;

	}
}

void Texture::disable()
{
	switch(texture_type_){
	case Texture_2D:
		glBindTexture(GL_TEXTURE_2D,0);
		break;

	}
}