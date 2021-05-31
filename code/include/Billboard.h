#pragma once
#include "Shader.h"

class Billboard
{
private:
	GLuint BillboardVao;
	unsigned int textureID;
	GLuint BillboardVbo;

	Shader shader;

public:
	Billboard(glm::vec3, unsigned int, int, int, const char*, const char*, const char* = nullptr);

	glm::vec3 vertexPos;

	void Draw(float, float);
	void CleanUp();
};