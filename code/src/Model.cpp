#include "Model.h"

// Declaració de la funció del load_obj.cpp que serveix per cargar els vertexs, uvs i normals dels models que importem
extern bool loadOBJ(const char* path, std::vector < glm::vec3 >& out_vertices, std::vector < glm::vec2 >& out_uvs, std::vector < glm::vec3 >& out_normals);

Model::Model(const char* _objPath)
{
	// Cambiem l'string del path que rebem de l'objecte per deixar només visible el seu nom per després fer-ho servir al ImGui
	name = _objPath;
	name.erase(name.size() - 4, name.size());
	name.erase(name.begin(), name.begin() + 4);

	bool res = loadOBJ(_objPath, vertices, uvs, normals);
}

Model::~Model()
{
	vertices.clear();
	uvs.clear();
	normals.clear();
}