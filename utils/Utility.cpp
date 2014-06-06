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



SkyBox::SkyBox()
{

}

SkyBox::~SkyBox()
{

}

void SkyBox::initialize()
{
	shader = new GLShader("shader/environmentmap.vert","shader/environmentmap.frag");
	cubeMapID = LoadGLTextures();
}

void SkyBox::render(int width, int height)
{
	shader->begin();

	/// init shader data
	glm::vec3 eyePos = glm::vec3(0.0,0.0,0.5);

	// init light in the scene
	glm::vec3 lightPos = glm::vec3(2.0,2.0,0.0);
	vec3 La = vec3(0.9,0.9,0.9);
	vec3 Ld = vec3(1.0,1.0,1.0);
	vec3 Ls = vec3(1.0,1.0,1.0);

	// init model material
	vec3 Ka = vec3(0.2,0.2,0.2);
	vec3 Kd = vec3(1.0,1.0,1.0);
	vec3 Ks = vec3(1.0,1.0,1.0);
	float shiness  = 5.0;

	glm::mat4 projection_matrix = glm::perspective(45.0f,(float)width/height,0.1f,1000.0f);
	glm::mat4 view_matrix = glm::lookAt(eyePos,glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));



	glm::mat4 model_matrix = glm::scale(mat4(1.0),vec3(1000.0,1000.0,1000.0));
	model_matrix = glm::rotate(model_matrix,180.0f,vec3(0.0,0.0,1.0));
	/// populate shader data to shader
	shader->setUniform("eyePos",eyePos);

	// for light
	shader->setUniform("light.position",lightPos);
	shader->setUniform("light.La",La);
	shader->setUniform("light.Ld",Ld);
	shader->setUniform("light.Ls",Ls);

	// for material
	shader->setUniform("mat.Ka",Ka);
	shader->setUniform("mat.Kd",Kd);
	shader->setUniform("mat.Ks",Ks);
	shader->setUniform("mat.shiness",shiness);

	shader->setUniform("view_matrix",view_matrix);
	shader->setUniform("projection_matrix",projection_matrix);
	shader->setUniform("model_matrix",model_matrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,cubeMapID);
	shader->setUniform("cubeMap",0);

	shader->setUniform("skyBox",true);
	glutSolidCube(1.0);

	shader->end();
}