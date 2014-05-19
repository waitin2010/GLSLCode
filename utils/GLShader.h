#pragma once
#include "gl/glew.h"
#include "gl/freeglut.h"
#include <map>
#include <string>
#include "glm/glm.hpp"
using  glm::vec3;
using  glm::vec4;
using  glm::mat4;
using  glm::mat3;
#include "glm/gtc/matrix_transform.hpp"

class   GLShader {
public:
	GLShader();
	~GLShader();
	GLShader(char *vsfilename, char *fsfilename);
	void begin();
	void end();
	GLuint getProgram();
	void setUniform(char *name,float x, float y, float z);
	void setUniform(char *name,float f);
	void setUniform(char *name, int i);
	void setUniform(char *name, bool b);
	void setUniform(char *name, vec3 v);
	void setUniform(char *name, vec4 v);
	void setUniform(char *name, mat3 m);
	void setUniform(char *name, mat4 m);

private:
	GLuint ps, vs,fs;
	std::map<std::string ,GLint> uniformLocation;
	char *readShader(char *filename);
	void initUniformLocation();
	char *infoLog;
};

