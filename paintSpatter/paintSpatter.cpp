#include <GL/glew.h>
#include <GL/freeglut.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLShader.h"
#include "ObjModel.h"
#include "noise/noise.h"
#include "SOIL.h"
#include "Directory.h"
#pragma  comment(lib,"soil")
#pragma comment(lib,"libnoise.lib")
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

/// texture 
GLuint brickTexID;
GLuint mossTexID;
GLuint initNoise()
{
	int width = 320;
	int height = 320;
	noise::module::Perlin perlinNoise;

	// Base frequency for octave 1
	perlinNoise.SetFrequency(4.0);
	GLubyte *data = new GLubyte[width * height * 4];

	double xRange = 1.0;
	double yRange = 1.0;
	double xFactor = xRange /width;
	double yFactor = yRange / height;

	for (int oct = 0; oct < 4 ; oct++)
	{
		perlinNoise.SetOctaveCount(oct+1);
		for(int i = 0; i < width; i++)
		{
			for(int j =0; j< height; j++)
			{
				double x = xFactor * i;
				double y = yFactor * j;
				double z = 0.0;
#if 0
				float val = (float) perlinNoise.GetValue(x,y,z);

#else
				float val = 0.0f;
				double a, b, c, d;
				a = perlinNoise.GetValue(x, y, z);
				b = perlinNoise.GetValue(x+xRange, y, z);
				c = perlinNoise.GetValue(x, y+yRange,z);
				d = perlinNoise.GetValue(x+xRange,y+yRange,z);

				double xmix = 1.0 - x/ xRange;
				double ymix = 1.0 - y/ yRange;

				double x1 = glm::mix(a, b, xmix);
				double x2 = glm::mix(c, d, xmix);

				val = ( val + 1.0f ) * 0.5f;

				val = val  > 1.0f ? 1.0f: val;
				val = val  < 0.0f ? 0.0f: val;
				val = glm::mix(x1, x2, ymix);
#endif
				data[(j*width + i) * 4 + oct] = (GLubyte) (val * 255.0f);
			}
		}
	}

	GLuint texID;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&texID);
	glBindTexture(GL_TEXTURE_2D,texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,GL_UNSIGNED_BYTE,data);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	delete [] data;
	return texID;
}

int LoadGLTextures(char *file)									// Load Bitmaps And Convert To Textures
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
		SOIL_FLAG_MIPMAPS
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
	shader = new GLShader("shader/paintSpatter.vert","shader/paintSpatter.frag");
	model = new Obj();
	char dir[256];
	sprintf(dir,"%s%s",RelativeDir,"data/cube.obj");
	model->load(dir);
	model->createVao();
	initNoise();
	// init texture
	sprintf(dir,"%s%s",RelativeDir,"data/brick.jpg");
	brickTexID = LoadGLTextures(dir);
		sprintf(dir,"%s%s",RelativeDir,"data/moss.png");
	mossTexID = LoadGLTextureBmp(dir);
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
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	shader->begin();

	/// init shader data
	glm::vec3 eyePos = glm::vec3(4.0,0.0,0.0);

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

	glm::mat4 projection_matrix = glm::perspective(45.0f,(float)width/height,1.0f,1000.0f);
	glm::mat4 view_matrix = glm::lookAt(eyePos,glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,xAngle,glm::vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,yAngle,glm::vec3(1.0,0.0,0.0));


	glm::mat4 model_matrix = glm::rotate(glm::mat4(1.0),0.0f,glm::vec3(0.0,1.0,0.0));
	model_matrix = glm::translate(model_matrix,glm::vec3(0.0,-1.0,0));
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
	

	shader->setUniform("noiseTex",0);



	model->draw();

	mat4 rotate_matrix = glm::rotate(mat4(1.0),angle,glm::vec3(1.0,1.0,0.0));
	
	model_matrix = glm::translate(glm::mat4(1.0),mat3(rotate_matrix) * lightPos);
	

	shader->setUniform("model_matrix", model_matrix);

	glutWireSphere(0.2,10,10);
	glutSwapBuffers();
	glutPostRedisplay();
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