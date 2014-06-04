/* OpenGL(4.3) compute shader example
 * Date: 2014-05-23
 * reference: http://wili.cc/blog/opengl-cs.html
 * /

/* add compute shader this gallery
*/
#include "gl/glew.h"
#include "gl/freeglut.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLShader.h"

// step 1: AntTweakBar GUI head file
#include "AntTweakBar.h"


#define NUM_PARTICLES  1024 * 1024  // 1M particles to move
#define WORK_GROUP_SIZE  128  // # work items per work group
#define XMIN -500.0
#define XMAX 500.0
#define YMIN XMIN
#define YMAX XMAX
#define ZMIN XMIN
#define ZMAX XMAX

#define VXMIN 0.1
#define VXMAX 1.0
#define VYMIN VXMIN
#define VYMAX VXMAX
#define VZMIN VXMIN
#define VZMAX VXMAX

using namespace glm;

// Shader
GLShader *computeShader, *renderShader;
int width = 800;
int height = 600;


struct pos
{
	float x, y, z, w;  // positions
};
struct vel
{
	float vx, vy, vz, vw; // velocities
};
// need to do this for both the position and velocity of the particles:
GLuint  posSSbo;
GLuint  velSSbo;

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
GLuint genComputeProg(GLuint texHandle) {
	computeShader = new GLShader("shader/ComputeShader_ParticleSystem.comp");
	GLuint progHandle = computeShader->getProgram();
	glUseProgram(progHandle);

	glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);

	CheckError("Compute shader");
	return progHandle;
}

void init()
{
	renderShader = new GLShader("shader/ComputeShader_ParticleSystem.vert","shader/ComputeShader_ParticleSystem.frag");
	computeShader = new GLShader("shader/ComputeShader_ParticleSystem.comp");

	glViewport(0,0,width,height);

	glGenBuffers( 1, &posSSbo);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, posSSbo );
	glBufferData( GL_SHADER_STORAGE_BUFFER,    NUM_PARTICLES * sizeof(struct pos),    NULL, GL_STATIC_DRAW );
	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ;
	// the invalidate makes a big difference when rewriting
	struct pos *points = (struct pos *) glMapBufferRange(
		GL_SHADER_STORAGE_BUFFER, 0,
		NUM_PARTICLES * sizeof(struct pos), bufMask );
	for( int i = 0; i < NUM_PARTICLES; i++ )
	{
		points[ i ].x = Ranf( XMIN, XMAX );
		points[ i ].y = Ranf( YMIN, YMAX );
		points[ i ].z = Ranf( ZMIN, ZMAX );
		points[ i ].w = 1.;
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
	glGenBuffers( 1, &velSSbo );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, velSSbo );
	glBufferData( GL_SHADER_STORAGE_BUFFER,    NUM_PARTICLES * sizeof(struct vel),    NULL, GL_STATIC_DRAW );
	struct vel *vels = (struct vel *) glMapBufferRange(
		GL_SHADER_STORAGE_BUFFER, 0,
		NUM_PARTICLES * sizeof(struct vel), bufMask );
	for( int i = 0; i < NUM_PARTICLES; i++ )
	{
		vels[ i ].vx = Ranf( VXMIN, VXMAX );
		vels[ i ].vy = Ranf( VYMIN, VYMAX );
		vels[ i ].vz = Ranf( VZMIN, VZMAX );
		vels[ i ].vw = 0.;
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 4, posSSbo );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 5, velSSbo );
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
		glDispatchCompute(NUM_PARTICLES/WORK_GROUP_SIZE,1,1);
		glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
		computeShader->end();
		CheckError("compute Shader");
	}
	
	// render pass
	{
		renderShader->begin();

		glm::vec3 eyePos = glm::vec3(100.0,1500.0,zdistance);


		glm::mat4 projection_matrix = glm::perspective(45.0f,(float)width/height,1.0f,10000.0f);
		glm::mat4 view_matrix = glm::lookAt(eyePos,glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));
		view_matrix = glm::rotate(view_matrix,xAngle,glm::vec3(0.0,1.0,0.0));
		view_matrix = glm::rotate(view_matrix,yAngle,glm::vec3(1.0,0.0,0.0));

		glm::mat4 model_matrix = glm::mat4(1.0);

		renderShader->setUniform("view_matrix",view_matrix);
		renderShader->setUniform("projection_matrix",projection_matrix);
		renderShader->setUniform("model_matrix",model_matrix);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER,posSSbo);
		glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,0,NULL);

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER,velSSbo);
		glVertexAttribPointer(5,3,GL_FLOAT,GL_FALSE,0,NULL);
		//glPointSize(10.0);
		glDrawArrays(GL_POINTS,0,NUM_PARTICLES);


		glBindBuffer(GL_ARRAY_BUFFER,0);
		CheckError("Draw screen");
		//glutSolidSphere(600.0,20,20);
		renderShader->end();
	
		
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