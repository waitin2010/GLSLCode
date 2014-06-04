#include "Dependencies.h"
#include "RenderSystem.h"




// the innerColor variable
vec3 innerColor = vec3(125.0,10.0,125.0);//default black.
GLShader *shader;
GLint ps;
int width =320;
int height =320;

/// for camera
RenderSystem::Camera camera;
RenderSystem::Terrain terrain;
RenderSystem::QuadTree quadTree;
RenderSystem::Frustum frustums;

vec3 eye_position = vec3(125.0,10.0,125.0);
vec3 eye_direction = vec3(0.0,0.0,1.0);
vec3 eye_up = vec3(0.0,1.0,0.0);
int xOrigin = -1;
int yOrigin = -1;
float xAngle;
float yAngle;
GLuint quadVao;
int count =0;
GLuint eb;

GLuint textureHandle;
struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec3 direction;
};
struct Light light;

unsigned char * LoadHeightMap(int &width, int &height)
{
	/* load an image as a heightmap, forcing greyscale (so channels should be 1) */
	int channels;
	unsigned char* ht_map = SOIL_load_image
		(
		"data/heightmap01.bmp",
		&width, &height, &channels,
		SOIL_LOAD_L
		);
	printf("heightmap\n");
	return ht_map;
}
GLuint getTextureHandle(char* filename)
{
	/* load another image, but into the same texture ID, overwriting the last one */
	GLuint tex_2d = 0;
	tex_2d = SOIL_load_OGL_texture
		(
		filename,
		SOIL_LOAD_AUTO,
		tex_2d,
		SOIL_FLAG_DDS_LOAD_DIRECT
		);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D,tex_2d);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	return tex_2d;
}
void init()
{
	light.ambient = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	light.diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light.direction = vec3(-0.5f, -1.0f, 0.0f);
	camera.perspective(45.0f,(float)width/height,1.0f,1000.0f);
	camera.lookat(vec3(125.0,10.0,125.0),glm::vec3(0.0),glm::vec3(0.0,1.0,0.0));

	frustums.constructFrustum(1.0,camera.getProjectionMatrix(),camera.getViewMatrix());

	shader = new GLShader("shader/TerrainTexturing.vert","shader/TerrainTexturing.frag");

	int width, height;
	unsigned char * ht_mp = LoadHeightMap(width, height);
	terrain.initialize(ht_mp,width,height,"");
	
	vec3 *positionOut,*normalOut;
	vec2 *textureCoordOut;
	
	int vertexCount = terrain.getVertexCount();
	positionOut = new vec3[vertexCount];
	normalOut = new vec3[vertexCount];
	textureCoordOut = new vec2[vertexCount];

	terrain.copyVertexArray(positionOut,normalOut,textureCoordOut);
	quadTree.initialize(vertexCount,positionOut,normalOut,textureCoordOut);
	textureHandle = getTextureHandle("data/dirt01.dds");
	ps = shader->getProgram();
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
		camera.moveForward(1.0);
		break;
	case 's':
		camera.moveForward(-1.0);
		break;
	case 'a':
		camera.rotate(1);
		break;
	case 'd':
		camera.rotate(-1);
		break;
	}
	/// step 5: handle events and window size changes
	TwEventKeyboardGLUT(key,x,y);
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
	/// step 5: handle events and window size changes
	TwEventMouseButtonGLUT(button,state,x,y);
}
void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		xAngle = x - xOrigin;
		xOrigin = x;
		yAngle = y - yOrigin;
		yOrigin = y;
	}
		/// step 5: handle events and window size changes
	TwEventMouseMotionGLUT(x,y);
	glutPostRedisplay();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 projection_matrix = camera.getProjectionMatrix();
	glm::mat4 view_matrix = camera.getViewMatrix();//,vec3(0.0,0.0,0.0),vec3(0.0,1.0,0.0));
	frustums.constructFrustum(1.0,projection_matrix,view_matrix);
	glFrontFace(GL_CCW);
	shader->begin();
	shader->setUniform("light.ambientColor",light.ambient);
	shader->setUniform("light.diffuseColor",light.diffuse);
	shader->setUniform("light.lightDirection",light.direction);
	shader->setUniform("projection_matrix",projection_matrix);
	shader->setUniform("world_matrix",mat4(1.0));
	shader->setUniform("view_matrix",view_matrix);
	glActiveTexture(0);
	shader->setUniform("terrain",0);
	//terrain.render();
	quadTree.render(&frustums);
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
	//glutIdleFunc(display);
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
	TwAddVarRW(myBar,"diffuse",TW_TYPE_COLOR4F,&light.diffuse,"");
	TwAddVarRW(myBar,"ambient",TW_TYPE_COLOR4F,&light.ambient, "");
	TwAddVarRW(myBar,"direction",TW_TYPE_COLOR3F,&light.direction, "");
	//glutIdleFunc(display);
	glutMainLoop();
	return 0;
}