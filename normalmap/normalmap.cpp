#include <GL/glew.h>
#include <GL/freeglut.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLShader.h"
#include "ObjModel.h"
#include "Directory.h"
#include "SOIL.h"
#pragma  comment(lib,"soil")

using glm::vec3;
using glm::mat4;
/// shader variables
GLShader *shader;


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
float zdistance = 0.0;

/// texture 
GLuint ao_earsTexID;
GLuint diffuseTexID;

int LoadGLTextures(char *file)									// Load Bitmaps And Convert To Textures
{
	/* load an image file directly as a new OpenGL texture */
	 GLuint texID = SOIL_load_OGL_texture
		(
		file,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

	if(texID == 0)
		return -1;


	// Typical Texture Generation Using Data From The Bitmap
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

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
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

	if(texID == 0)
		return -1;


	// Typical Texture Generation Using Data From The Bitmap
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	return texID;										// Return Success
}
void init()
{
	shader = new GLShader("shader/normalmap.vert","shader/normalmap.frag");
	model = new Obj();
	char dir[256];
	sprintf(dir,"%s%s",RelativeDir,"data/bs_ears.obj");
	model->load(dir);
	model->generateTangent();
	model->createVao();

	// init texture
	sprintf(dir,"%s%s",RelativeDir,"data/ogre_normalmap.png");
	ao_earsTexID = LoadGLTextures(dir);
	sprintf(dir,"%s%s",RelativeDir,"data/diffuse.png");
	diffuseTexID = LoadGLTextureBmp(dir);
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
	//angle +=0.5;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	shader->begin();

	/// init shader data
	glm::vec3 eyePos = glm::vec3(4.0,0.0,0.0);

	// init light in the scene
	glm::vec3 lightPos = glm::vec3(-2.0,2.0,0.0);
	vec3 La = vec3(0.2,0.2,0.2);
	vec3 Ld = vec3(1.0,1.0,1.0);
	vec3 Ls = vec3(0.9,0.9,0.9);

	// init model material
	vec3 Ka = vec3(0.2,0.2,0.2);
	vec3 Kd = vec3(1.0,1.0,1.0);
	vec3 Ks = vec3(0.2,0.2,0.2);
	float shiness  = 2.0;

	glm::mat4 projection_matrix = glm::perspective(45.0f,(float)width/height,1.0f,1000.0f);
	glm::mat4 view_matrix = glm::lookAt(eyePos,glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,xAngle,glm::vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,yAngle,glm::vec3(1.0,0.0,0.0));
	view_matrix = glm::translate(view_matrix,glm::vec3(0.0,0.0,zdistance));

	glm::mat4 model_matrix = glm::rotate(glm::mat4(1.0),0.0f,glm::vec3(0.0,1.0,0.0));
	model_matrix = glm::translate(model_matrix,glm::vec3(0.0,0.0,0));
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
	glBindTexture(GL_TEXTURE_2D,ao_earsTexID);
	shader->setUniform("normalMapTex",0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,diffuseTexID);
	shader->setUniform("colorTex",1);

	model->draw();

	mat4 rotate_matrix = glm::rotate(mat4(1.0),angle,glm::vec3(1.0,1.0,0.0));
	
	model_matrix = glm::translate(glm::mat4(1.0),mat3(rotate_matrix) * lightPos);
	

	shader->setUniform("model_matrix", model_matrix);

	glutWireSphere(0.2,10,10);
	glutSwapBuffers();

}
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 27:
		exit(0);
		break;
	case 'w':
		zdistance+=0.5;
		break;
	case 's':
		zdistance -= 0.5;
		break;
	}
	glutPostRedisplay();
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
	glutInitWindowSize(320,320);
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