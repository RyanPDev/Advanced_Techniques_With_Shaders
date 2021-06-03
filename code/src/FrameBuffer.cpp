#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
}

FrameBuffer::FrameBuffer(const char* vertexPath, const char* fragmentPath)
{
	// setup GB texture
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glm::vec3 vertexPos = { 0, 0, 0 };
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), &texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// we create the texture
	glGenTextures(1, &fbo_tex);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// if we need a depth or stencil buffer, we do it here

	//we bind texture (or renderbuffer) to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);
	
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader = Shader(vertexPath, fragmentPath);
}
void FrameBuffer::Update()
{	
}

void FrameBuffer::DrawCubeFBOTex(glm::vec3 carPosition, glm::vec3 carRotation)
{
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	objMat = glm::mat4(1);
	glm::mat4 t = glm::translate(glm::mat4(), mirrorPos);
	glm::mat4 r2 = glm::rotate(glm::mat4(), -carRotation.y, glm::vec3(0, 1, 0));
	glm::mat4 t2 = glm::translate(glm::mat4(), carPosition);
	objMat = t * r2 * t2;

	//Draw
	/*objMat = glm::mat4(1);
	objMat = glm::translate(objMat, mirrorPos);
	objMat = glm::rotate(objMat, -carRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	objMat = glm::translate(objMat, carPosition);*/
	//glm::mat4 auxMat = carObjMatrix - objMat;
	glm::vec3 forwardVector = glm::vec3((carPosition.x + mirrorPos.x)-carPosition.x, 0, (carPosition.z + mirrorPos.z) - carPosition.z);

	shader.SetFloat3( "forwardVector", forwardVector);
	shader.SetMat4("mvp", 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
	shader.SetMat4("projection", 1, GL_FALSE, glm::value_ptr(RenderVars::_projection));
	shader.SetMat4("objm,", 1, GL_FALSE, glm::value_ptr(objMat));
	shader.SetMat4("view", 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
	shader.SetFloat("width", 5);
	shader.SetFloat("height", 5);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
	glBindVertexArray(0);
}

void FrameBuffer::CleanUp()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, vbo);
	shader.CleanUpShader();
	glDeleteTextures(1, &fbo_tex);
}