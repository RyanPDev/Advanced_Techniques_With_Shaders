#pragma once
#include "Utils.h"

class Model
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::string name;

public:
	Model(const char*);
	~Model();
	inline const std::vector<glm::vec3> GetVertices()const { return vertices; }
	inline const std::vector<glm::vec2> GetUvs()const { return uvs; }
	inline const std::vector<glm::vec3> GetNormals()const { return normals; }
	inline const std::string GetName()const { return name; }
};