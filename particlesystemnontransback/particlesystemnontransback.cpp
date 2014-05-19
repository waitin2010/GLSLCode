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

/// texture 
GLuint brickTexID;
GLuint mossTexID;

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

float randFloat() {
	return ((float)rand() / RAND_MAX);
}
int nParticles;
GLuint initVel;
GLuint startTime;
GLuint particles;
void initBuffers()
{
	nParticles = 8000;

	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	vec3 v(0.0f);
	float velocity, theta, phi;
	float PI = 3.1415926;
	float TWOPI = 2 * PI;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++ ) {

		theta = glm::mix(0.0f, (float)PI / 6.0f, randFloat());
		phi = glm::mix(0.0f, (float)TWOPI, randFloat());

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(1.25f,1.5f,randFloat());
		v = glm::normalize(v) * velocity;

		data[3*i]   = v.x;
		data[3*i+1] = v.y;
		data[3*i+2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER,initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete [] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for( unsigned int i = 0; i < nParticles; i++ ) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER,startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	delete [] data;

	// Attach these to the torus's vertex array
	glGenVertexArrays(1, &particles);
	glBindVertexArray(particles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}


void init()
{
	shader = new GLShader("shader/particlesystemnontransback.vert","shader/particlesystemnontransback.frag");
	model = new Obj();
	char dir[256];
	sprintf(dir,"%s%s",RelativeDir,"data/cube.obj");
	model->load(dir);
	model->createVao();
	initBuffers();
	// init texture
	glActiveTexture(GL_TEXTURE0);
	sprintf(dir,"%s%s",RelativeDir,"blueWater.png");
	//brickTexID = LoadGLTextures("bluewater.jpg");
	mossTexID = LoadGLTextureBmp(dir);
	shader->setUniform("Tex",0);
	glClearColor(1.0,1.0,1.0,1.0);
	glDisable(GL_DEPTH_TEST);
	//glClearColor(0.1f,0.1f,0.1f,1.0f);
	glEnable(GL_POINT_SPRITE);
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set the point size
	glPointSize(10.0f);
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
	static float time = 0.0f;
	time += 0.0001;
	glClear(GL_COLOR_BUFFER_BIT);
	shader->begin();

	/// init shader data
	glm::vec3 eyePos = glm::vec3(4.0,0.0,0.0);

	glm::mat4 projection_matrix = glm::perspective(45.0f,(float)width/height,1.0f,1000.0f);
	glm::mat4 view_matrix = glm::lookAt(eyePos,glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,xAngle,glm::vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,yAngle,glm::vec3(1.0,0.0,0.0));


	glm::mat4 model_matrix = glm::rotate(glm::mat4(1.0),0.0f,glm::vec3(0.0,1.0,0.0));
	model_matrix = glm::translate(model_matrix,glm::vec3(0.0,-1.0,0));
	/// populate shader data to shader

	shader->setUniform("MVP",projection_matrix * view_matrix * model_matrix);
	shader->setUniform("time",time);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D,brickTexID);

	
	glBindVertexArray(particles);
	glDrawArrays(GL_POINTS,0,nParticles);
	//model->draw();
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
	glutIdleFunc(display);
	glutMainLoop();
}