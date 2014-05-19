#include <gl/freeglut.h>
GLuint quadVao;
GLuint quadMeshVao;
void initQuad()
{
	static GLfloat quad[] = {
		-1.0, -1.0, 0.0,
		1.0, -1.0,  0.0,
		-1.0, 1.0,  0.0,
		1.0, -1.0, 0.0,
		1.0, 1.0, 0.0,
		-1.0, 1.0, 0.0
	};
	static GLfloat quad_normal[] = {
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,1.0
	};
	static GLfloat quad_tex_coord[] = {
		0.0, 0.0, 
		1.0, 0.0,
		0.0, 1.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};
	GLuint quadID[3];
	glGenBuffers(3,quadID);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[0]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad),quad,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[1]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad_normal),quad_normal,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[2]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quad_tex_coord),quad_tex_coord,GL_STATIC_DRAW);

	glGenVertexArrays(1,&quadVao);
	glBindVertexArray(quadVao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[0]);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[1]);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);

	glBindBuffer(GL_ARRAY_BUFFER,quadID[2]);
	glVertexAttribPointer(2,2, GL_FLOAT,GL_FALSE,0, NULL);
}

void drawQuad()
{
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLES,0,6);
}

void initQuadMesh(int width, int height)
{
	int i, j;
	float dx = 1.0/ width;
	float dy = 1.0 / height;
	int count = 0;
	float *data = new float[width * height * 2];
	int *element = new int[(width-1) * height];
	for(i = 0; i < width; i++)
		for(j=0; j < height; j++)
		{
			data[(j+i*width)*2] =   i * dx;
			data[(j+i*width)*2+1] = j * dy;
		}
	for(i =0; i < width -1; i++)
		for(j=0; j < height; j++)
		{
			element[count++] = j + i * width;
			element[count++] = j+1 + i*width;
			element[count++] = j+1 + (i+1)*width;
			element[count++] = j + (i+1)*width;
		}
	
	glGenVertexArrays(1,&quadMeshVao);
	glBindVertexArray(quadMeshVao);
	
	glEnableVertexAttribArray(0);
	

	GLuint quadMeshID[2];
	glGenBuffers(2,quadMeshID);

	glBindBuffer(GL_ARRAY_BUFFER,quadMeshID[0]);
	glBufferData(GL_ARRAY_BUFFER,width * height * 2, data, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,quadMeshID[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,(width-1) * height * 4, element,GL_STATIC_DRAW);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
}

void drawQuadMesh()
{
	glBindVertexArray(quadMeshVao);
	glDrawElements(GL_QUADS,4,GL_UNSIGNED_BYTE,NULL);
}