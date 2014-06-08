#ifndef _FRAMEBUFFEROBJECT_H_
#define _FRAMEBUFFEROBJECT_H_
#include "Dependencies.h"
#include "RenderSystem.h"

namespace RenderSystem{
	
	class FrameBufferObject {
	public:
		FrameBufferObject();
		~FrameBufferObject();
		bool initialize();
		bool release();
		void begin();
		void end();
		GLuint getRenderTexture();
	private:
		bool checkFramebufferStatus();
	private:
		GLuint render_texture_;
		GLuint depth_texture_;
		GLuint framebuffer_handle_;
	};
}
#endif