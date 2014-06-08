#include "SkyBox.h"
using namespace RenderSystem;

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