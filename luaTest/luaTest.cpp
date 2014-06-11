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
#include "Dependencies.h"
#include "RenderSystem.h"

#include <stdarg.h>


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
 void error (lua_State *L, const char *fmt, ...) {
      va_list argp;
      va_start(argp, fmt);
    //  vfprintf(stderr, argp);
      va_end(argp);
      lua_close(L);
      exit(EXIT_FAILURE);
   }

 void load (char *filename, int *width, int *height) {
      lua_State *L = luaL_newstate();
      luaopen_base(L);
      luaopen_io(L);
      luaopen_string(L);
      luaopen_math(L);
    
      if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
        error(L, "cannot run configuration file: %s",
                 lua_tostring(L, -1));
    
      lua_getglobal(L, "width");
      lua_getglobal(L, "height");
      if (!lua_isnumber(L, -2))
        error(L, "`width' should be a number\n");
      if (!lua_isnumber(L, -1))
        error(L, "`height' should be a number\n");
      *width = (int)lua_tonumber(L, -2);
      *height = (int)lua_tonumber(L, -1);
    
      lua_close(L);
    }

 

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

	/// step 5: handle events and window size changes
	TwWindowSize(w,h);
}
void keyboard(unsigned char key, int x, int y)
{
	switch(key){
	case 27:
		exit(0);
			break;
	case 'l':
		delete shader;
		shader = new GLShader("shader/FrameWork.vert","shader/FrameWork.frag");
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
	/*vec3 innerColor = vec3((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX);*/
	vec3 outerColor = vec3(0.0,0.0,0.0);
	float innerRadius = 0.1;
	float outerRadius = 0.6;
	shader->setUniform("innerColor",innerColor);
	shader->setUniform("outerColor",outerColor);
	shader->setUniform("innerRadius",innerRadius);
	shader->setUniform("outerRadius",outerRadius);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLES,0,6);
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

	load("script/window_configuration.lua",&width, &height);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(width,height);
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