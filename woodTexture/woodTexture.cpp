#include <GL/glew.h>
#include <GL/freeglut.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/transform2.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "GLShader.h"
#include "ObjModel.h"
#include "vaoQuad.h"
#include "noise/noise.h"
#include "Directory.h"
#pragma comment(lib,"libnoise.lib")
using glm::vec3;
using glm::mat4;
/// shader variables
GLShader *shader;
GLuint noiseTexID;

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



void init()
{
	shader = new GLShader("shader/woodTexture.vert","shader/woodTexture.frag");
	model = new Obj();
	char dir[256];
	sprintf(dir,"%s%s",RelativeDir,"data/cylinder.obj");
	model->load(dir);
	model->createVao();
	initQuad();
	noiseTexID = initNoise();
	
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
	
	mat4 view_matrix = mat4(1.0);
	mat4 projection_matrix = mat4(1.0);
	mat4 model_matrix = mat4(1.0);
	mat4 slice;
	slice *= glm::rotate(10.0f, vec3(1.0, 0.0, 0.0));
	slice *= glm::rotate(-20.0f, vec3(0.0,0.0,1.0));
	slice *= glm::scale(vec3(40.0, 40.0, 1.0));
	slice *= glm::translate( vec3(-0.35, -0.5, 2.0));

	shader->setUniform("slice",slice);
	shader->setUniform("view_matrix",view_matrix);
	shader->setUniform("projection_matrix",projection_matrix);
	shader->setUniform("model_matrix",model_matrix);
	shader->setUniform("noiseTex",0);
	//model->draw();
	drawQuad();

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