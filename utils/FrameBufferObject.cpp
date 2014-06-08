#include "FrameBufferObject.h"
#include <iostream>
using namespace std;
using namespace RenderSystem;

FrameBufferObject::FrameBufferObject()
{

}
FrameBufferObject::~FrameBufferObject()
{

}

bool FrameBufferObject::initialize()
{
	glGenTextures(1,&render_texture_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,render_texture_);

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,512,512,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D,0);


	glGenRenderbuffers(1,&depth_texture_);
	glBindRenderbuffer(GL_RENDERBUFFER,depth_texture_);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,512,512);
	glBindRenderbuffer(GL_RENDERBUFFER,0);

	glGenFramebuffers(1,&framebuffer_handle_);
	glBindFramebuffer(GL_FRAMEBUFFER,framebuffer_handle_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_texture_);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,render_texture_,0);

	GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1,drawBufs);
	GLenum state =	glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(!checkFramebufferStatus())
		return false;
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	return true;
}

bool FrameBufferObject::release()
{
	return true;
}

void FrameBufferObject::begin()
{
	glBindFramebuffer(GL_FRAMEBUFFER,framebuffer_handle_);
	glViewport(0,0,512,512);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}
void FrameBufferObject::end()
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

GLuint FrameBufferObject::getRenderTexture()
{
	return render_texture_;
}
bool FrameBufferObject::checkFramebufferStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        std::cout << "Framebuffer complete." << std::endl;
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
        return false;
/*
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
        std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
        return false;
*/
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
        return false;

    default:
        std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
        return false;
    }
}