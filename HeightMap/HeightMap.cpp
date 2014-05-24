/* Learn to use the AntTweakBar GUI For OpenGL
 * Step 1 : include head file
 * Step 2 : initialization
 * Step 3 : create a tweak bar and add variable
 * Step 4 : Draw your tweak bar
 * Step 5 : handle events, and window size change
 * reference: http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:howto
 * Example: example/TwSimpleGLUT.c
*/

/* the program use AntTweakBar GUI to control the inner color
 * in the demo of GLSL4.0 cookbook.
*/
/// OpenGL Library
#include "gl/glew.h"
#include "gl/freeglut.h"

/// Mathematics Library for matrix manipulation
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/// Shader Class 
#include "GLShader.h"


/// OpenGL Texture Library
#include "SOIL.h"
#pragma comment(lib, "soil")
// step 1: AntTweakBar GUI head file
#include "AntTweakBar.h"



using namespace glm;

// the innerColor variable
vec3 innerColor = vec3(1.0,1.0,1.0);//default black.
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

void LoadHeightMap()
{
	/* load an image as a heightmap, forcing greyscale (so channels should be 1) */
	int width, height, channels;
	unsigned char* ht_map = SOIL_load_image
		(
		"data/heightmap01.bmp",
		&width, &height, &channels,
		SOIL_LOAD_L
		);
	printf("heightmap\n");
}
void init()
{
	shader = new GLShader("shader/HeightMap.vert","shader/HeightMap.frag");
	LoadHeightMap();
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

	/// step 5: handle events and window size changes
	TwWindowSize(w,h);
}
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 27:
		exit(0);
			break;
	}
	/// step 5: handle events and window size changes
	TwEventKeyboardGLUT(key,x,y);
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
	/// step 5: handle events and window size changes
	TwEventMouseButtonGLUT(button,state,x,y);
}
void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		xAngle += x - xOrigin;
		xOrigin = x;
		yAngle += y - yOrigin;
		yOrigin = y;
	}
		/// step 5: handle events and window size changes
	TwEventMouseMotionGLUT(x,y);
	glutPostRedisplay();
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	gluPerspective(45.0,(float)width/height,1,100);

	shader->begin();

	shader->end();
	/// Step 4: Draw your tweak bar
	// It must be called just before the frame buffer is presented(swapped).
	TwDraw();
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

	/// step 2: initializtion
	// After OpenGL initializations and the creation of 
	// your graphic window, initialize AntTweakBar
	TwInit(TW_OPENGL,NULL);
	// or for core profile
	// TwInit(TW_OPENGL_CORE, NULL);

	/// step 3: Create a tweak bar and add variables
	// Create a tweak bar
	TwBar  *myBar;
	myBar = TwNewBar("NameOffMyTweakBar");

	// Add some variables
	TwAddVarRW(myBar,"width",TW_TYPE_INT32,&width, " min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");
	TwAddVarRW(myBar,"innerColor",TW_TYPE_COLOR3F,&innerColor, "");

	//glutIdleFunc(display);
	glutMainLoop();
	return 0;
}