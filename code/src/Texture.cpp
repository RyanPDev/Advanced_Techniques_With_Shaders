#include "Texture.h"

Texture::Texture(ETYPE type, const char* texturePath)
{
	data = stbi_load(texturePath, &texWidth, &texHeight, &nrChannels, 4);

	glGenTextures(1, &textureID); //TEXTURES
	glBindTexture(GL_TEXTURE_2D, textureID); //TEXTURES

	if (data) {  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); } //TEXTURES
	else std::cout << "Failed to load texture" << std::endl;

	if (type == ETYPE::OBJ) glGenerateMipmap(GL_TEXTURE_2D);
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	stbi_image_free(data);
}