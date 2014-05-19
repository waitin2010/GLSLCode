#include "gl/glew.h"
#include "gl/freeglut.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLShader.h"
#pragma comment(lib,"glew32.lib")
using namespace glm;
GLShader *shader;
GLint ps;
int width =320;
int height =320;

/// for camera
int xOrigin = -1;
int yOrigin = -1;
float xAngle;
float yAngle;
GLuint quadVao;
void init()
{
	shader = new GLShader("shader/FrameWork.vert","shader/FrameWork.frag");
	ps = shader->getProgram();
	glClearColor(0.0,0.0,0.0,1.0);
	glEnable(GL_DEPTH_TEST);
	static GLfloat quad[] = {
		-0.8, -0.8, 0.0,
		0.8, -0.8,  0.0,
		-0.8, 0.8,  0.0,
		0.8, -0.8, 0.0,
		0.8, 0.8, 0.0,
		-0.8, 0.8, 0.0
	};
	static GLfloat quad_tex_coord[] = {
		0.0, 0.0, 
		1.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};
	GLuint quadID[2];
	glGenBuffers(2,quadID);
	
	glBindBuffer(GL_ARRAY_BUFFER,quadID[0]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad),quad,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[1]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad_tex_coord),quad_tex_coord,GL_STATIC_DRAW);

	glGenVertexArrays(1,&quadVao);
	glBindVertexArray(quadVao);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[0]);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[1]);
	glVertexAttribPointer(1,2, GL_FLOAT,GL_FALSE,0, NULL);

}

void shape(int w, int h)
{
	if( w == 0)
		w =h;
	glViewport(0,0,w,h);
	width = w;
	height = h;
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
void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glUseProgram(ps);
	vec3 innerColor = vec3((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX);
	vec3 outerColor = vec3(0.0,0.0,0.0);
	float innerRadius = 0.1;
	float outerRadius = 0.6;
	shader->setUniform("innerColor",innerColor);
	shader->setUniform("outerColor",outerColor);
	shader->setUniform("innerRadius",innerRadius);
	shader->setUniform("outerRadius",outerRadius);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLES,0,6);
	glutSwapBuffers();

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
	return 0;
}