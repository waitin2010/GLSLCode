#include "Dependencies.h"
#include "RenderSystem.h"

using glm::vec3;
using glm::mat4;

/// shader variables
GLShader *shader;

RenderSystem::FrameBufferObject fbo;

/// screen size
int width =320;
int height =320;

/// scene data
Obj *model;

/// for camera
int xOrigin = -1;
int yOrigin = -1;
float xAngle;
float yAngle;

/// texture 
GLuint brickTexID;
GLuint mossTexID;
GLuint fboHandle;
GLuint renderTex;


void init()
{
	shader = new GLShader("shader/rendertoTexture.vert","shader/rendertoTexture.frag");
	model = new Obj();
	
	model->load("data/cube.obj");
	model->createVao();
	initQuad();
	fbo.initialize();
	brickTexID = LoadGLTextures("data/brick.jpg");
	//mossTexID = LoadGLTextureBmp("moss.png");
	glClearColor(0.0,0.0,0.0,1.0);
	glEnable(GL_DEPTH_TEST);
}

void shape(int w, int h)
{
	if( w == 0)
		w =h;
	glViewport(0,0,w,h);
	width = w;
	height = h;
}

void display()
{
	static float angle = 0.0;
	angle +=0.5;
	/*glBindFramebuffer(GL_FRAMEBUFFER,fboHandle);
	glViewport(0,0,512,512);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);*/
	fbo.begin();
	shader->begin();

	/// init shader data
	glm::vec3 eyePos = glm::vec3(0.0,0.0,4.0);

	// init light in the scene
	glm::vec3 lightPos = glm::vec3(1.0,0.0,1.0);
	vec3 La = vec3(0.2,0.2,0.2);
	vec3 Ld = vec3(1.0,1.0,1.0);
	vec3 Ls = vec3(1.0,1.0,1.0);

	// init model material
	vec3 Ka = vec3(0.2,0.2,0.2);
	vec3 Kd = vec3(1.0,1.0,1.0);
	vec3 Ks = vec3(1.0,1.0,1.0);
	float shiness  = 5.0;

	glm::mat4 projection_matrix = glm::perspective(45.0f,(float)1.0,1.0f,1000.0f);
	glm::mat4 view_matrix = glm::lookAt(eyePos,glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
	//view_matrix = glm::rotate(view_matrix,xAngle,glm::vec3(0.0,1.0,0.0));
	//view_matrix = glm::rotate(view_matrix,yAngle,glm::vec3(1.0,0.0,0.0));

	//glm::mat4 model_matrix = glm::rotate(glm::mat4(1.0),0.0f,glm::vec3(0.0,1.0,0.0));
	//model_matrix = glm::translate(model_matrix,glm::vec3(0.0,-1.0,0));
	mat4 model_matrix = mat4(1.0);
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
	glBindTexture(GL_TEXTURE_2D,brickTexID);
	shader->setUniform("brickTex",0);

//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D,mossTexID);
//	shader->setUniform("mossTex",1);

	//glutSolidSphere(1.0,20,20);
	glGenerateMipmap(GL_TEXTURE_2D);
	model->draw();
	//mat4 rotate_matrix = glm::rotate(mat4(1.0),angle,glm::vec3(1.0,1.0,0.0));
//	model_matrix = glm::translate(glm::mat4(1.0),mat3(rotate_matrix) * lightPos);
//	shader->setUniform("model_matrix", model_matrix);
//	glutWireSphere(0.2,10,10);

#if 1
	/// #pass two 
	//glBindFramebuffer(GL_FRAMEBUFFER,0);
	fbo.end();
	glViewport(0,0,width,height);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	view_matrix = glm::rotate(view_matrix,xAngle,glm::vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,yAngle,glm::vec3(1.0,0.0,0.0));
	model_matrix = mat4(1.0);

	shader->setUniform("view_matrix",view_matrix);
	shader->setUniform("model_matrix",model_matrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,fbo.getRenderTexture());
	shader->setUniform("brickTex",0);
	
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLES,0,6);
	//model->draw();
#endif 


	
	glutSwapBuffers();
}
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 27:
		exit(0);
		break;
	}
}
void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		xOrigin = x;
		yOrigin = y;
	}else{
		xOrigin = -1;
		yOrigin = -1;
	}
}
void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		xAngle += x - xOrigin;
		xOrigin = x;
		yAngle += y - yOrigin;
		yOrigin = y;
	}
	glutPostRedisplay();
}
int main(int argc, char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(512,512);
	glutCreateWindow("OpenGL Shading Langue Demo");
	GLenum err = glewInit();
	if(0 != err)
	{
		printf("Cannot init Glew \n");
		exit(0);
	}
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(shape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	//glutIdleFunc(display);
	glutMainLoop();
}