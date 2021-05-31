#pragma once
#include "Utils.h"

class Texture
{
private:
	unsigned char* data;
	int texWidth, texHeight, nrChannels;
	unsigned int textureID;

public:
	enum class ETYPE { OBJ, BB, COUNT };
	Texture(ETYPE, const char* = nullptr);

	inline const unsigned int GetID()const { return textureID; }
};