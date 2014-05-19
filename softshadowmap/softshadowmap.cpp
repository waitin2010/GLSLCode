#include <gl/glew.h>
#include <gl/freeglut.h>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLShader.h"
#include "ObjModel.h"
#include "SOIL.h"
#include "Directory.h"
#pragma  comment(lib,"soil")

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

/// obj model
Obj *model;
GLuint vao;
GLuint vbo;
GLuint nbo;
/// shadow map
GLuint render_light_prog;
GLuint render_scene_prog;
GLuint depth_texture;
GLuint depth_fbo;
GLShader *light_shader;
GLShader *scene_shader;
#define DEPTH_TEXTURE_SIZE 512

/// light shader 
GLuint mvp_matrix_loc;

/// scene shader
GLuint projection_matrix_loc;
GLuint view_matrix_loc;
GLuint model_matrix_loc;
GLuint shadow_matrix_loc;
GLuint  depth_texture_loc;
GLuint light_position_loc;

/// shadowmap display
GLuint quad_vertexbuffer;
GLuint quad_texturecoord;
GLuint quad_VertexArrayID;
GLuint tex_loc;
int texSize = 8;
int samplesU = 8;
int samplesV = 4;
GLuint offsetTexID;
/// texture
GLuint texture[1];
int LoadGLTextures(char *file)									// Load Bitmaps And Convert To Textures
	{
		/* load an image file directly as a new OpenGL texture */
		texture[0] = SOIL_load_OGL_texture
			(
			file,
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
			);

		if(texture[0] == 0)
			return false;


		// Typical Texture Generation Using Data From The Bitmap
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		return true;										// Return Success
	}
bool checkFramebufferStatus()
{
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        std::cout << "Framebuffer complete." << std::endl;
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
        return false;
/*
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
        std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
        return false;
*/
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
        return false;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
        return false;

    default:
        std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
        return false;
    }
}
float jitter()
{
	return ((float)rand() / RAND_MAX) - 0.5;
}
void buildOffsetTex(int texSize, int samplesU, int samplesV)
{
	const float TWOPI_F = 2 * 3.14;
	int size = texSize;
	int samples = samplesU * samplesV;
	int bufSize = size * size * samples * 2;
	float *data = new float[bufSize];

	for( int i = 0; i < size; i++ ) {
		for(int j = 0; j < size; j++ ) {
			for( int k = 0; k < samples; k += 2 ) {
				int x1,y1,x2,y2;
				x1 = k % (samplesU);
				y1 = (samples - 1 - k) / samplesU;
				x2 = (k+1) % samplesU;
				y2 = (samples - 1 - k - 1) / samplesU;

				vec4 v;
				// Center on grid and jitter
				v.x = (x1 + 0.5f) + jitter();
				v.y = (y1 + 0.5f) + jitter();
				v.z = (x2 + 0.5f) + jitter();
				v.w = (y2 + 0.5f) + jitter();

				// Scale between 0 and 1
				v.x /= samplesU;
				v.y /= samplesV;
				v.z /= samplesU;
				v.w /= samplesV;

				// Warp to disk
				int cell = ((k/2) * size * size + j * size + i) * 4;
				data[cell+0] = sqrtf(v.y) * cosf( TWOPI_F * v.x );
				data[cell+1] = sqrtf(v.y) * sinf( TWOPI_F * v.x );
				data[cell+2] = sqrtf(v.w) * cosf( TWOPI_F * v.z );
				data[cell+3] = sqrtf(v.w) * sinf( TWOPI_F * v.z );
			}
		}
	}

	glActiveTexture(GL_TEXTURE1);
	GLuint texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_3D, texID);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, size, size, samples/2, 0, GL_RGBA, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	delete [] data;
}
void init()
{
	//LoadGLTextures("data/hello.jpg");
	/// shadowmap display
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	static const GLfloat g_quad_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	static const GLfloat g_quad_vertex_texture_coord[] = {
		0.0f,0.0f,
		1.0f,0.0f,
		0.0f,1.0f,
		0.0f,1.0f,
		1.0f,0.0f,
		1.0f,1.0f,
	};
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1,&quad_texturecoord);
	glBindBuffer(GL_ARRAY_BUFFER,quad_texturecoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_texture_coord), g_quad_vertex_texture_coord, GL_STATIC_DRAW);
	//// shader init
	shader = new GLShader("shader/softshadowmap.vert","shader/softshadowmap.frag");
	ps = shader->getProgram();
	tex_loc = glGetUniformLocation(ps,"tex");

	light_shader = new GLShader("shader/light.vert","shader/light.frag");
	render_light_prog = light_shader->getProgram();
    mvp_matrix_loc = glGetUniformLocation(render_light_prog,"mvp_matrix");

	scene_shader = new GLShader("shader/scene.vert","shader/scene.frag");
	render_scene_prog = scene_shader->getProgram();
	projection_matrix_loc = glGetUniformLocation(render_scene_prog,"projection_matrix");
	view_matrix_loc = glGetUniformLocation(render_scene_prog,"view_matrix");
	model_matrix_loc = glGetUniformLocation(render_scene_prog,"model_matrix");
	shadow_matrix_loc = glGetUniformLocation(render_scene_prog,"shadow_matrix");
	depth_texture_loc = glGetUniformLocation(render_scene_prog,"depth_texture");
	light_position_loc = glGetUniformLocation(render_scene_prog,"light_position");

	glUseProgram(render_light_prog);
	glClearColor(0.0,0.0,0.0,1.0);
	

	glGenFramebuffers(1,&depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER,depth_fbo);
	
	glGenTextures(1,&depth_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,depth_texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,
		DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,depth_texture,0);

	glDrawBuffer(GL_NONE);

	bool fabo = checkFramebufferStatus();
	assert(fabo != false);
	/// init obj model
	glBindTexture(GL_TEXTURE_2D,0);
	model = new Obj();
	char dir[256];
	sprintf(dir,"%s%s",RelativeDir,"data/room_thickwalls.obj");
	model->load("data/room_thickwalls.obj");
	glUseProgram(render_scene_prog);
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,model->getVertexsCount()*3*sizeof(float),model->getVertexsAdress(),GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1,&nbo);
	glBindBuffer(GL_ARRAY_BUFFER,nbo);
	glBufferData(GL_ARRAY_BUFFER,model->getVertexsCount()*3*sizeof(float),model->getNormalAdress(),GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);
	glUseProgram(0);

	buildOffsetTex(texSize,samplesU, samplesV);
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
void drawScene()
{

	glDrawArrays(GL_TRIANGLES,0,model->getVertexsCount());
	
	//glutSolidSphere(1.0,20,20);

}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	vec3 light_position = vec3(0.0,5.0,- 5.0);
	static float angle = 0.0;
	angle += 0.5;
	
	//light_position =  mat3(rotate(mat4(1.0),angle,vec3(0.0,1.0,0.0))) * light_position;
	glBindFramebuffer(GL_FRAMEBUFFER,depth_fbo);
	glViewport(0,0,DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f,4.0f);

	glUseProgram(render_light_prog);
	mat4 scene_model_matrix = mat4(1.0);
	mat4 light_view_matrix = lookAt(light_position,vec3(0.0),vec3(0.0,1.0,0.0));
	mat4 light_projection_matrix(frustum(-1.0,1.0,-1.0,1.0,1.0,20.0));

	glUniformMatrix4fv(mvp_matrix_loc,1, GL_FALSE,value_ptr(light_projection_matrix * light_view_matrix * scene_model_matrix));

	
	//glBindFramebuffer(GL_FRAMEBUFFER,0);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);




	drawScene();
	/*glBindTexture(GL_TEXTURE_2D,depth_texture);
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,DEPTH_TEXTURE_SIZE,DEPTH_TEXTURE_SIZE);*/

	//glCullFace(GL_BACK);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisableVertexAttribArray(0);



	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,width,height);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
#if 1
	glUseProgram(render_scene_prog);

	vec3 offsetTexSize = vec3(texSize,texSize,samplesU * samplesV/2);
	float radius = 7.0f;
	
	glUniform3fv(glGetUniformLocation(render_scene_prog,"offsetTexSize"),1,&offsetTexSize[0]);
	glUniform1f(glGetUniformLocation(render_scene_prog,"radius"),radius/512.0);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_3D,offsetTexID);
	glUniform1i(glGetUniformLocation(render_scene_prog,"offsetTex"),1);
	mat4 projection_matrix = perspective(45.0f,(float)width/height,0.1f,100.0f);
	glUniformMatrix4fv(projection_matrix_loc,1,GL_FALSE,value_ptr(projection_matrix));
	mat4 view_matrix;

	glUniform3fv(light_position_loc,1,value_ptr(light_position));
	view_matrix = lookAt(vec3(10.0,10.0,10.0),vec3(0.0,0.0,0.0),vec3(0.0,1.0,0.0));
	view_matrix = rotate(view_matrix,xAngle,vec3(0.0,1.0,0.0));
	view_matrix = rotate(view_matrix,yAngle,vec3(1.0,0.0,0.0));
	glUniformMatrix4fv(view_matrix_loc,1,GL_FALSE,value_ptr(view_matrix));

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glUniform1i(depth_texture_loc,0);

	mat4 scale_bias_matrix = mat4(vec4(0.5,0.0,0.0,0.0),
		vec4(0.0,0.5,0.0,0.0),
		vec4(0.0,0.0,0.5,0.0),
		vec4(0.5,0.5,0.5,1.0));
	mat4 shadow_matrix = scale_bias_matrix * light_projection_matrix * light_view_matrix;
	glUniformMatrix4fv(shadow_matrix_loc,1,GL_FALSE,value_ptr(shadow_matrix));
	mat4 cone_matrix;
	cone_matrix = translate(mat4(1.0),vec3(0.0,0.0,0.0));
	glUniformMatrix4fv(model_matrix_loc,1,GL_FALSE,value_ptr(cone_matrix));

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,nbo);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
	drawScene();
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
#endif 
#if 1

	glViewport(0,0,256,256);
	glUseProgram(ps);
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
	// Set our "renderedTexture" sampler to user Texture Unit 0
	glUniform1i(tex_loc, 0);

	// 1rst attribute buffer : vertices 
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,quad_texturecoord);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL
		);
	// Draw the triangle !
	// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
//	glPolygonMode(GL_FRONT,GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	//glPolygonMode(GL_FRONT,GL_FILL);
#endif 
	glUseProgram(0);
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	//glutInitContextVersion (4, 0);
//	glutInitContextProfile (GLUT_CORE_PROFILE );
//	glutInitContextFlags(GLUT_DEBUG);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("OpenGL Shading Langue Demo");

	glutDisplayFunc(display);
	glutReshapeFunc(shape);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	glewInit();
	init();
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	glutMainLoop();
	return 0;
}