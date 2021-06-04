#pragma once
#include "Shader.h"

class Billboard
{
private:
	GLuint BillboardVao;
	GLuint BillboardVbo;
	unsigned int textureID;

	Shader shader;

public:
	Billboard(Shader, glm::vec3, unsigned int, int, int);

	glm::vec3 vertexPos;

	void Draw(float, float);
	void CleanUp();
};