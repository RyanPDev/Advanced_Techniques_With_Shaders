#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
}

FrameBuffer::FrameBuffer(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	// setup GB texture
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glm::vec3 vertexPos = { 0, 0, 0 };
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &vertexPos, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// we create the texture
	glGenTextures(1, &fbo_tex);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 200, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// if we need a depth or stencil buffer, we do it here

	//we bind texture (or renderbuffer) to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);
	
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 200, 100);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader = Shader(vertexPath, fragmentPath, geometryPath);
}

void FrameBuffer::DrawCubeFBOTex(glm::mat4 carObjMatrix)
{
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);

	//Draw
	objMat = carObjMatrix;
	objMat = glm::translate(objMat, localPosition);
	glm::mat4 auxMat = carObjMatrix - objMat;
	glm::vec3 forwardVector = glm::vec3(auxMat[0].x, 0, auxMat[0].z);	

	shader.SetFloat3( "forwardVector", forwardVector);
	shader.SetMat4("mvp", 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
	shader.SetMat4("view", 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
	shader.SetFloat("width", 5);
	shader.SetFloat("height", 5);

	glDrawArrays(GL_POINTS, 0, 1);

	glUseProgram(0);
	glBindVertexArray(0);
}