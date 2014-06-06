#ifndef _UTILITY_H_
#define _UTILITY_H_
#include "Dependencies.h"
#include "RenderSystem.h"

extern void renderTexture2D(GLuint texture_id);

extern int LoadGLTextures();

class SkyBox {
private:
	GLShader *shader;
	GLint cubeMapID;
public:
	SkyBox();
	~SkyBox();
	void initialize();
	void release();
	void render(int width, int height);

};
#endif