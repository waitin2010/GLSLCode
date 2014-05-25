/* Subject: Convert Compute Shader from DX11 to OpenGL
 * Date: 2014-05-23
 * /

/* Using Compute Shader to achieve Array Sum Operation.
 * Array A + Array B = Array C, which ci = ai + bi
*/
#include "gl/glew.h"
#include "gl/freeglut.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLShader.h"

// step 1: AntTweakBar GUI head file
#include "AntTweakBar.h"



using namespace glm;

// The number of elements in a buffer to be tested.
const unsigned	NUM_ELEMNTS = 1024;

// Shader
GLShader *computeShader;
int width = 800;
int height = 600;

struct BufType
{
	int i;
	float f;
};
struct BufType g_vBuf0[NUM_ELEMNTS];
struct BufType g_vBuf1[NUM_ELEMNTS];
GLuint arrayASSbo;
GLuint arrayBSSbo;
GLuint arrayCSSbo;
void initArray()
{
	for(int i=0;i<NUM_ELEMNTS;++i)
	{
		g_vBuf0[i].i = i;
		g_vBuf0[i].f = (float)i;

		g_vBuf1[i].i = i;
		g_vBuf1[i].f = (float)i;
	}
}


/// for camera
int xOrigin = -1;
int yOrigin = -1;
float xAngle;
float yAngle;
GLuint quadVao;

float zdistance = 0;

float Ranf(float low, float hight)
{
	return low + (float)rand()/RAND_MAX * (hight - low);
}
void CheckError(std::string desc)
{
	GLenum e = glGetError();
	if(e != GL_NO_ERROR){
		fprintf(stderr, "OpenGL error in \"%s\": %s (%d)\n",desc.c_str(),gluErrorString(e),e);
		exit(20);
	}
}

void init()
{
	
	computeShader = new GLShader("shader/BasicCompute112GL.comp");

	glViewport(0,0,width,height);

	initArray();
	glGenBuffers(1,&arrayASSbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,arrayASSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER,NUM_ELEMNTS*sizeof(struct BufType),g_vBuf0,GL_STATIC_DRAW);

	glGenBuffers(1,&arrayBSSbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,arrayBSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER,NUM_ELEMNTS*sizeof(struct BufType),g_vBuf1,GL_STATIC_DRAW);

	glGenBuffers(1,&arrayCSSbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,arrayCSSbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER,NUM_ELEMNTS*sizeof(struct BufType),NULL,GL_STATIC_DRAW);

	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 4, arrayASSbo );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 5, arrayBSSbo );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 6, arrayCSSbo );
	CheckError("Gen texture");	
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
	case 'w':
		zdistance += 1.0;
			break;
	case 's':
		zdistance -= 1.0;
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
	static int frame = 0;
	frame++;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	/// init shader data
	
	// compute shader pass
	{	
		computeShader->begin();
		glDispatchCompute(NUM_ELEMNTS,1,1);
		glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
		computeShader->end();
		CheckError("compute Shader");
		glBindBuffer(GL_SHADER_STORAGE_BUFFER,arrayCSSbo);
		GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ;
		// the invalidate makes a big difference when rewriting
		struct BufType *arrayC = (struct BufType *) glMapBufferRange(
			GL_SHADER_STORAGE_BUFFER, 0,NUM_ELEMNTS * sizeof(struct BufType), bufMask );
		int i;
		for(  i = 0; i < NUM_ELEMNTS; i++ )
		{
			if(arrayC[i].i == g_vBuf0[i].i + g_vBuf1[i].i&&abs(arrayC[i].f-g_vBuf0[i].f-g_vBuf1[i].f)<1e-6)
				continue;
		}
		glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
		if(i<NUM_ELEMNTS)
		{
			printf("failure\n");
			exit(0);
		}else
		{
			printf("succeeded\n");
		}
	}
	
	
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
	glutIdleFunc(display);
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
	//TwAddVarRW(myBar,"width",TW_TYPE_INT32,&width, " min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");
	//TwAddVarRW(myBar,"innerColor",TW_TYPE_COLOR3F,&innerColor, "");

	//glutIdleFunc(display);
	glutMainLoop();
	return 0;
}