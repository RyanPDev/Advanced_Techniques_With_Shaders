#pragma once
#include "Shader.h"
#include "Model.h"

// Classe "Objecte": Agrupa els atributs necessaris dels objectes que carguem i instanciem a l'escena.
class Object
{
private:
	unsigned int textureID;
	GLuint ObjVao;
	GLuint ObjVbo[3];
	std::string name;
	glm::vec3 initPos, initRot, initScale;

	int numVertices;
	Shader shader;
public:
	Object();
	Object(Model*, unsigned int, glm::vec3, glm::vec3, glm::vec3, Shader);

	glm::vec3 position, rotation, scale;
	glm::mat4 objMat;
	bool usingStencil = false;

	void Update();
	void Draw(Light);
	void Draw(float, float, float, bool, bool);
	void CleanUp();

	std::string GetName() { return name; } //--> Retorna el nom de l'objecte corresponent per poder "printar-lo" per pantalla
};