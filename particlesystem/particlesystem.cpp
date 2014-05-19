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
float zdistance = 1.0f;
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
GLuint posBuf[2], velBuf[2];
GLuint particleArray[2];
GLuint feedback[2], initVel, startTime[2];
GLuint drawBuf , query;
GLuint renderSub, updateSub;

void initBuffers()
{
	nParticles = 4000;

	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, startTime); // Start time buffers
	glGenBuffers(1, &initVel);  // Initial velocity buffer (never changes, only need one)

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);

	// Fill the first position buffer with zeroes
	GLfloat *data = new GLfloat[nParticles * 3];
	for( int i = 0; i < nParticles * 3; i += 3 ) {
		data[i] = glm::mix(-2.0f, 2.0f, randFloat());
		data[i+1] = 0.0f;
		data[i+2] = 0.0f;
	}
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the first velocity buffer with random velocities
	for( int i = 0; i < nParticles; i++ ) {
		data[3*i]   = 0.0f;
		data[3*i+1] = glm::mix(1.1f,1.5f,randFloat());
		data[3*i+2] = 0.0f;
	}
	glBindBuffer(GL_ARRAY_BUFFER,velBuf[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	glBindBuffer(GL_ARRAY_BUFFER,initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the first start time buffer
	delete [] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.001f;
	for( int i = 0; i < nParticles; i++ ) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER,startTime[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	delete [] data;

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}


void init()
{
	shader = new GLShader("shader/particlesystem.vert","shader/particlesystem.frag");
	GLuint programHandle =shader->getProgram();

	renderSub = glGetSubroutineIndex(programHandle, GL_VERTEX_SHADER, "render");
	updateSub = glGetSubroutineIndex(programHandle, GL_VERTEX_SHADER, "update");
	glGenQueries(1,&query);
	model = new Obj();
	char dir[256];
	sprintf(dir,"%s%s",RelativeDir,"data/cube.obj");
	model->load(dir);
	model->createVao();
	initBuffers();
	// init texture
	glActiveTexture(GL_TEXTURE0);
	//brickTexID = LoadGLTextures("bluewater.jpg");
	sprintf(dir,"%s%s",RelativeDir,"data/fire.png");
	mossTexID = LoadGLTextureBmp(dir);
	shader->setUniform("Tex",0);
	//glClearColor(1.0,1.0,1.0,1.0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.1f,0.1f,0.1f,1.0f);
	glEnable(GL_POINT_SPRITE);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set the point size
	glPointSize(50.0f);
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
	shader->begin();
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &updateSub);
	static float time = 0.0;
	static float deltaT = 0.01;
	time += 0.001;
	deltaT = 0.01;
	shader->setUniform("Time",time);
	shader->setUniform("H",deltaT);
#if 1
	glEnable(GL_RASTERIZER_DISCARD);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

	glBeginTransformFeedback(GL_POINTS);
	glBindVertexArray(particleArray[1-drawBuf]);
	glDrawArrays(GL_POINTS, 0, nParticles);
	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	GLuint ptsWritten;
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &ptsWritten);
	//printf("Written: %d\n", ptsWritten);
	glDisable(GL_RASTERIZER_DISCARD);
#endif 
	// Render pass
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &renderSub);
	glClear( GL_COLOR_BUFFER_BIT );
	mat4 projection_matrix = glm::perspective(60.0f,1.0f,0.3f,1000.0f);
	mat4 view_matrix = glm::lookAt(vec3(1.0,1.0,4.0),vec3(0.0),vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,xAngle,vec3(0.0,1.0,0.0));
	view_matrix = glm::rotate(view_matrix,yAngle,vec3(1.0,0.0,0.0));
	//view_matrix = glm::translate(view_matrix,vec3(0.0,0.0,zdistance));
	mat4 model_matrix  = glm::translate(mat4(1.0),vec3(0.0,0.0,0));
	model_matrix = glm::rotate(model_matrix,xAngle,vec3(0.0,1.0,0.0));
	model_matrix = glm::rotate(model_matrix,yAngle,vec3(1.0,0.0,0.0));

	shader->setUniform("MVP",projection_matrix * view_matrix * model_matrix);
//	time += 0.001;
	//shader->setUniform("Time", time);
	//printf("%f\n",zdistance);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glBindVertexArray(particleArray[drawBuf]);
	glDrawArrays(GL_POINTS, 0, nParticles);

	// Swap buffers
	drawBuf = 1 - drawBuf;
	glutSwapBuffers();
}
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 27:
		exit(0);
		break;
		case 'w':
			zdistance += 0.01f;
			break;
		case 's':
			zdistance -= 0.01f;
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
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);

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