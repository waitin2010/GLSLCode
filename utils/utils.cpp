#include "ObjModel.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
void Obj::load(char *filename)
{
	FILE *fp;
	unsigned int v_count,vt_count,vn_count,f_count;
	float *vertexs;
	int *vertexs_index;
	float *texture;
	int *texture_index;
	float *normals;
	int *normals_index;

	int v_index, vi_index;
	int vt_index, vti_index;
	int vn_index, vni_index;

	int count;

	fp  = fopen(filename,"r");
	assert(fp!=NULL);

	v_count = vt_count = vn_count = f_count = 0;
	while(1)
	{
		char str[128];
		int result;
		result = fscanf(fp,"%s\n",str);
		if(result == EOF)
			break;
		if(strcmp(str,"v")==0)
		{
			v_count++;
		}else if (strcmp(str,"vt")==0){
			vt_count++;
		}else if (strcmp(str,"vn")==0){
			vn_count++;
		}else if (strcmp(str,"f")==0){
			f_count++;
		}else {
			;
		}

	}
	{
		fseek(fp,0,SEEK_SET);
		vertexs = new float[3*v_count];
		texture = new float[2*vt_count];
		normals = new float[3*vn_count];
		vertexs_index = new int[3*f_count];
		texture_index = new int[3*f_count];
		normals_index = new int[3*f_count];

		v_index = vt_index = vn_index = 0;
		vi_index = vti_index = vni_index = 0;

		m_vertexs = new float[3*3*f_count];
		m_texture = new float[2*3*f_count];
		m_normals = new float[3*3*f_count];
		m_vertexs_count = 3 * f_count;
		m_texture_count = 3 * f_count;
		count = 0;
	}

	while(1)
	{
		char str[128];
		int result;
		result = fscanf(fp,"%s\n",str);
		if(result == EOF)
			break;
		if(strcmp(str,"v")==0)
		{
			int index = v_index;
			v_index += 3;
			fscanf(fp,"%f %f %f\n",&vertexs[index],&vertexs[index+1],&vertexs[index+2]);
		}else if (strcmp(str,"vt")==0){
			int index = vt_index;
			vt_index += 2;
			fscanf(fp,"%f %f\n",&texture[index],&texture[index+1]);
		}else if (strcmp(str,"vn")==0){
			int index = vn_index;
			vn_index += 3;
			fscanf(fp,"%f %f %f\n",&normals[index],&normals[index+1],&normals[index+2]);
		}else if (strcmp(str,"f")==0){
			int v1,v2,v3;
			int t1,t2,t3;
			int n1,n2,n3;
#if 1
			int count = fscanf(fp,"%d/%d/%d %d/%d/%d %d/%d/%d\n",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3);
			assert(count==9);
			vertexs_index[vi_index++]=v1 -1;
			texture_index[vti_index++]=t1-1;
			normals_index[vni_index++]=n1-1;
			vertexs_index[vi_index++]=v2-1;
			texture_index[vti_index++]=t2-1;
			normals_index[vni_index++]=n2-1;
			vertexs_index[vi_index++]=v3-1;
			texture_index[vti_index++]=t3-1;
			normals_index[vni_index++]=n3-1;
#else 
			int count = fscanf(fp,"%d/%d %d/%d %d/%d\n",&v1,&t1,&v2,&t2,&v3,&t3);
			assert(count==6);
			vertexs_index[vi_index++]=v1 -1;
			texture_index[vti_index++]=t1-1;
			// normals_index[vni_index++]=n1-1;
			vertexs_index[vi_index++]=v2-1;
			texture_index[vti_index++]=t2-1;
			//  normals_index[vni_index++]=n2-1;
			vertexs_index[vi_index++]=v3-1;
			texture_index[vti_index++]=t3-1;
			//  normals_index[vni_index++]=n3-1;
#endif 

		}else {
			;
		}

	}
	fclose(fp);

	int count_t = 0;
	for(int i = 0; i < f_count; i++) // one face 
	{
		for(int j = 0; j<3;j++)// one vertice
		{
			int v_index = vertexs_index[i*3+j];
			int t_index = texture_index[i*3+j];
			int n_index = normals_index[i*3+j];
			for(int k=0; k <3; k++) // one vertice
			{
				m_vertexs[count] = vertexs[3*v_index+k];
				if( k!=2)
				{
					m_texture[count_t] = texture[2*t_index+k];
					//assert(m_texture[count_t]>-0.1);
					count_t++;
				}

				m_normals[count] = normals[3*n_index+k];
				count++;
			}

		}
	}

#if 1
	{
		if(vertexs)
			delete[] vertexs;
		if(texture)
			delete[] texture;
		if(normals)
			delete[] normals;
		if(vertexs_index)
			delete[] vertexs_index;
		if(texture_index)
			delete[] texture_index;
		if(normals_index)
			delete[] normals_index;
	}
#endif 

}

void Obj::createVao()
{
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,getVertexsCount()*3*sizeof(float),getVertexsAdress(),GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);


	glGenBuffers(1,&nbo);
	glBindBuffer(GL_ARRAY_BUFFER,nbo);
	glBufferData(GL_ARRAY_BUFFER,getVertexsCount()*3*sizeof(float),getNormalAdress(),GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);

	GLuint tbo;
	glGenBuffers(1,&tbo);
	glBindBuffer(GL_ARRAY_BUFFER,tbo);
	glBufferData(GL_ARRAY_BUFFER,m_texture_count* 2 * sizeof(float),m_texture,GL_STATIC_DRAW);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,NULL);

	if(tangent_enable)
	{
		GLuint sbo;
		glGenBuffers(1,&sbo);
		glBindBuffer(GL_ARRAY_BUFFER,sbo);
		glBufferData(GL_ARRAY_BUFFER,m_vertexs_count * 3 *sizeof(float),m_tangent,GL_STATIC_DRAW);
		glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,0,NULL);
	}

}

void Obj::draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES,0,getVertexsCount());
}

void Obj::generateTangent()
{
	//float *tan1 = (float *)malloc(m_vertexs_count * 3 * 2);
//	float *tan2 = tan1 + m_vertexs_count;
	tangent_enable = true;
	m_tangent = (float *) malloc(m_vertexs_count * 3 *sizeof(float));
	int i;
	for(i=0; i < m_vertexs_count; i++)
	{
		float p1[3];
		float p2[3];
		float p3[3];
		int j;

		float uv1[2];
		float uv2[2];
		float uv3[2];

		for(j = 0; j< 3; j++)
		{
			p1[j] = m_vertexs[3*i+j];
			p2[j] = m_vertexs[3*i+j];
			p3[j] = m_vertexs[3*i+j];
		}

		for(j=0; j<2; j++)
		{
			uv1[j] = m_texture[2*i+j];
			uv2[j] = m_texture[2*i+j];
			uv3[j] = m_texture[2*i+j];
		}
		
		float q1[3],q2[3];
		for(j=0; j<3; j++)
		{
			q1[j] = p2[j] - p1[j];
			q2[j] = p3[j] - p1[j];
		}

		float t1,t2, s1, s2;
		s1 = uv2[0] - uv1[0];
		s2 = uv3[0] - uv1[0];
		t1 = uv2[1] - uv1[1];
		t2 = uv3[1] - uv1[1];

		float r;

		r = 1/(s1*t2 - s2 * t1);

		for(j = 0; j <3 ; j++)
		{
			m_tangent[3*i+j] = r * t2 * q1[j] - r * t1 * q2[j];
		}
	


	}
}