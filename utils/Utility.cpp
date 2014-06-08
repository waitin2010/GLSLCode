#include "Utility.h"

void renderTexture2D(GLuint texture_id, GLShader *shader)
{

	float quad_verts[] =
	{
		-1, -1, 0, 1,
		-1,  1, 0, 1,
		1, -1, 0, 1,
		1,  1, 0, 1,
	};
	GLuint quad_vb;
	glGenBuffers(1,&quad_vb);
	glBindBuffer(GL_ARRAY_BUFFER,quad_vb);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad_verts),quad_verts,GL_STATIC_DRAW);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(0);

	glBindTexture(GL_TEXTURE_2D,texture_id);

	shader->begin();
	shader->setUniform("texture",0);
	glDrawArrays(GL_QUADS,0,4);
	shader->end();


}

int LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	GLuint texID = SOIL_load_OGL_cubemap
		(
		"data/night_posx.png",
		"data/night_negx.png",
		"data/night_negy.png",
		"data/night_posy.png",

		"data/night_posz.png",
		"data/night_negz.png",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);


	// Typical Texture Generation Using Data From The Bitmap
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	return texID;										// Return Success
}



