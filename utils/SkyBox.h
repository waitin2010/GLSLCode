#ifndef _SKYBOX_H_
#define _SKYBOX_H_
#include "Dependencies.h"
#include "RenderSystem.h"
namespace RenderSystem{

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
}
#endif