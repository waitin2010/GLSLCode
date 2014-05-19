#pragma  once
#include <gl/glew.h>
#include <gl/freeglut.h>

class Obj
{
public:
	Obj(){
		tangent_enable = false;
	}
	~Obj(){}

	void load(char *filename);
	float* getVertexsAdress()
	{
		return m_vertexs;
	}
	int getVertexsCount()
	{
		return m_vertexs_count;
	}
	float* getNormalAdress()
	{
		return m_normals;
	}
	float* getTextureAdree()
	{
		return m_texture;
	}
	int getTextureCount()
	{
		return m_texture_count;
	}
	void draw();

	void createVao();
	void Obj::generateTangent();
private:
	float *m_vertexs;
	float *m_normals;
	float *m_texture;
	float *m_tangent;

	int m_vertexs_count;
	int m_texture_count;
	bool tangent_enable;
	GLuint vbo;
	GLuint nbo;
	GLuint vao;
};