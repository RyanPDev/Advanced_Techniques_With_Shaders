#pragma once
#include "Shader.h"

class FrameBuffer
{
private:
	unsigned int fbo_tex;
	glm::vec3 localRotation;
	glm::vec3 localScale;
	glm::vec3 currentPos;
	glm::mat4 objMat;

	// positions
	float quadVertices[18] = {
		-0.3f,  1.0f, 0.0f,
		-0.3f,  0.7f, 0.0f,
		 0.3f,  0.7f, 0.0f,
		
		-0.3f,  1.0f, 0.0f,
		 0.3f,  0.7f, 0.0f,
		 0.3f,  1.0f, 0.0f
	};

	// texCoord
	float texCoords[12] = {
	 0.0f, 1.0f,
	 0.0f, 0.0f,
	 1.0f, 0.0f,

	 0.0f, 1.0f,
	 1.0f, 0.0f,
	 1.0f, 1.0f
	};
	GLuint vao;
	GLuint vbo[2];

	Shader shader;
public:
	glm::vec3 mirrorPos = { 0, 4.5f, 4 };
	glm::vec3 localPosition = { 0, 4.5f, -4 };
	GLuint fbo;
	FrameBuffer();
	FrameBuffer(const char*, const char*);
	void Update();
	void CleanUp();
	void DrawCubeFBOTex(glm::vec3, glm::vec3);
};