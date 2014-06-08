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

int LoadGLTexturesCube()									// Load Bitmaps And Convert To Textures
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


int LoadGLTextures(char *file)									// Load Bitmaps And Convert To Textures
{
	int len = strlen(file);
	GLuint texID;
	if(strcmp(file+len-3,"dds")){
		texID = SOIL_load_OGL_texture
			(
			file,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_DDS_LOAD_DIRECT
			);
	}else{
		texID = SOIL_load_OGL_texture
			(
			file,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
			);
	}
	/* load an image file directly as a new OpenGL texture */
	

	if(texID == 0)
		return -1;

	return texID;										// Return Success
}
int LoadGLTextureBmp(char *file)
{
	/* load an image file directly as a new OpenGL texture */
	GLuint texID = SOIL_load_OGL_texture
		(
		file,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);

	if(texID == 0)
		return -1;


	// Typical Texture Generation Using Data From The Bitmap
//	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	return texID;										// Return Success
}

GLuint quadVao;
void initQuad()
{
	static GLfloat quad[] = {
		-0.8, -0.8, 0.0,
		0.8, -0.8,  0.0,
		-0.8, 0.8,  0.0,
		0.8, -0.8, 0.0,
		0.8, 0.8, 0.0,
		-0.8, 0.8, 0.0
	};
	static GLfloat quad_normal[] = {
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0
	};
	static GLfloat quad_tex_coord[] = {
		0.0, 0.0, 
		1.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};
	GLuint quadID[3];
	glGenBuffers(3,quadID);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[0]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad),quad,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[1]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad_normal),quad_normal,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[2]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad_tex_coord),quad_tex_coord,GL_STATIC_DRAW);

	glGenVertexArrays(1,&quadVao);
	glBindVertexArray(quadVao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[0]);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[1]);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[2]);
	glVertexAttribPointer(2,2, GL_FLOAT,GL_FALSE,0, NULL);
}


/// texture display
GLuint quad_vertexbuffer;
GLuint quad_texturecoord;
GLuint quad_VertexArrayID;
GLShader *texture_shader;
void init_texture_display()
{
	texture_shader= new GLShader("shader/shadowmap.vert","shader/shadowmap.frag");
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	static const GLfloat g_quad_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	static const GLfloat g_quad_vertex_texture_coord[] = {
		0.0f,0.0f,
		1.0f,0.0f,
		0.0f,1.0f,
		0.0f,1.0f,
		1.0f,0.0f,
		1.0f,1.0f,
	};
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1,&quad_texturecoord);
	glBindBuffer(GL_ARRAY_BUFFER,quad_texturecoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_texture_coord), g_quad_vertex_texture_coord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,quad_texturecoord);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL
		);
}
void drawTexture(GLuint texture_id,GLShader *shader)
{
	glViewport(0,0,256,256);
	shader->begin();
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	shader->setUniform("tex", 0);

	// 1rst attribute buffer : vertices 
	
	// Draw the triangle !
	// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
	//	glPolygonMode(GL_FRONT,GL_LINE);
	glBindVertexArray(quad_VertexArrayID);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	shader->end();
}