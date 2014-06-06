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