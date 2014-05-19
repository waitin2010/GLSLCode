#include "GLShader.h"
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#pragma comment(lib,"glew32.lib")
GLShader::GLShader()
{

}
GLShader::~GLShader()
{
	glDeleteProgram(ps);
}
GLShader::GLShader(char *vsfilename, char *fsfilename)
{
	const char * vsSource = (const GLchar *)readShader(vsfilename);
	const char * fsSource = (const GLchar *)readShader(fsfilename);
	assert(vsSource!=NULL&& fsSource!=NULL);

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs,1,&vsSource,NULL);
	glCompileShader(vs);
	GLint result;
	glGetShaderiv(vs,GL_COMPILE_STATUS,&result);
	if(GL_FALSE == result)
	{
		fprintf(stdout,"vs errro!\n");
		GLint logLen;
		glGetShaderiv(vs,GL_INFO_LOG_LENGTH,&logLen);
		if(logLen >0)
		{
			char *log = (char*)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(vs,logLen,&written,log);
			fprintf(stdout,"%s\n",log);
			free(log);
		}
	}
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs,1,&fsSource,NULL);
	glCompileShader(fs);

	ps = glCreateProgram();
	glAttachShader(ps,vs);
	glAttachShader(ps,fs);


	glLinkProgram(ps);
	initUniformLocation();
	int status;
	glGetProgramiv(ps, GL_LINK_STATUS, &status );
	if( GL_FALSE == status ) {

		fprintf( stderr, "Failed to link shader program!\n" );

		GLint logLen;
		glGetProgramiv( ps, GL_INFO_LOG_LENGTH, &logLen );

		if( logLen > 0 )
		{
			char * log = (char *)malloc(logLen);

			GLsizei written;
			glGetProgramInfoLog(ps, logLen, &written, log);

			fprintf(stderr, "Program log: \n%s", log);

			free(log);
		}
	}
}
void GLShader::begin()
{
	glUseProgram(ps);
}
void GLShader::end()
{
	glUseProgram(0);
}
GLuint GLShader::getProgram()
{
	return ps;
}
char * GLShader::readShader(char *filename)
{
	char *str;
	FILE * fp;
	int len;

	fp = fopen(filename,"rb");
	assert(fp!=NULL);

	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	str = new char[len+1];

	fread(str,1,len,fp);
	str[len] = '\0';
	//fscanf(fp,"%s",str);
	fclose(fp);
	return str;
}

void GLShader::setUniform(char *name, bool b)
{
	glUniform1i(uniformLocation[name],b);
}

void GLShader::setUniform(char *name, int i)
{
	glUniform1i(uniformLocation[name],i);
}

void GLShader::setUniform(char *name,float f)
{
	glUniform1f(uniformLocation[name],f);
}

void GLShader::setUniform(char *name, float x, float y, float z)
{
	glUniform3f(uniformLocation[name],x,y,z);
}

void GLShader::setUniform(char *name , vec3 v)
{
	glUniform3fv(uniformLocation[name],1,&v[0]);
}

void GLShader::setUniform(char *name, vec4 v)
{
	glUniform4fv(uniformLocation[name],1,&v[0]);
}

void GLShader::setUniform(char *name, mat3 m)
{
	glUniformMatrix3fv(uniformLocation[name],1,GL_FALSE,&m[0][0]);
}

void GLShader::setUniform(char *name, mat4 m)
{
	glUniformMatrix4fv(uniformLocation[name],1,GL_FALSE,&m[0][0]);
}

void GLShader::initUniformLocation()
{
	GLint nUniform, maxLen;
	glGetProgramiv(ps,GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLen);
	glGetProgramiv(ps,GL_ACTIVE_UNIFORMS,&nUniform);

	if(nUniform > 0)
	{

		GLint size,written;
		GLenum type;
		char *name = (char *)malloc(maxLen);
		for(int i=0; i<nUniform; i++)
		{

			glGetActiveUniform(ps,i,maxLen,&written,&size,&type,name);
			GLint location = glGetUniformLocation(ps,name);
			uniformLocation.insert(std::make_pair((std::string)name,location));
			printf("%s ------------ %d\n",name,uniformLocation[name]);
		}
		free(name);
	}
}