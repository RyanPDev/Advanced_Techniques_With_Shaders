#pragma once
#include "Shader.h"

class FrameBuffer
{
private:
	unsigned int fbo_tex;
	glm::vec3 localPosition = { 1, 0, 0 };
	glm::vec3 localRotation;
	glm::vec3 localScale;
	glm::mat4 objMat;
	float quadVertices[24] = {
	   // positions   // texCoords
	   -0.3f,  1.0f,  0.0f, 1.0f,
	   -0.3f,  0.7f,  0.0f, 0.0f,
		0.3f,  0.7f,  1.0f, 0.0f,

	   -0.3f,  1.0f,  0.0f, 1.0f,
		0.3f,  0.7f,  1.0f, 0.0f,
		0.3f,  1.0f,  1.0f, 1.0f
	};
	GLuint vao;
	GLuint vbo;

	Shader shader;
public:
	GLuint fbo;
	FrameBuffer();
	FrameBuffer(const char*, const char*, const char*);
	void DrawCubeFBOTex(glm::mat4);
};