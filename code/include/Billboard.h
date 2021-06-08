#pragma once
#include "Shader.h"

class Billboard
{
private:
	GLuint BillboardVao;
	GLuint BillboardVbo;
	Shader shader;
	unsigned int textureID;

public:
	Billboard(Shader, glm::vec3, unsigned int);

	glm::vec3 vertexPos;

	void Draw(float, float);
	void CleanUp();
};