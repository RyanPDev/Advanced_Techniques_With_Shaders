#pragma once
#include "Shader.h"

class FrameBuffer
{
public:
	GLuint fbo;

	glm::vec3 localPosition = { 0, -4.64f, 3.65f };
private:
	GLuint vao;
	GLuint vbo[2];
	GLuint fboTexture;

	Shader shader;

	glm::vec3 mirrorPos = { 0.45f, 3.7f, 1.5f };
	glm::vec3 localScale;
	glm::mat4 objMat;

	// Posicions dels vèrtexs del quad
	glm::vec3 quadVertices[6] = {
		{-0.3f,  1.0f, 0.0f},
		{-0.3f,  0.7f, 0.0f},
		{ 0.3f,  0.7f, 0.0f},
						   
		{-0.3f,  1.0f, 0.0f},
		{ 0.3f,  0.7f, 0.0f},
		{ 0.3f,  1.0f, 0.0f}
	};

	// Coordenades de textura
	glm::vec2 texCoords[6] = {
	 {0.0f, 1.0f},
	 {0.0f, 0.0f},
	 {1.0f, 0.0f},

	 {0.0f, 1.0f},
	 {1.0f, 0.0f},
	 {1.0f, 1.0f}
	};

public:
	FrameBuffer();
	FrameBuffer(const char*, const char*);

	void CleanUp();
	void DrawQuadFBOTex();
	void Update(glm::vec3, glm::vec3);
	glm::vec3 GetRearCameraPosition()const { return localPosition; }
};