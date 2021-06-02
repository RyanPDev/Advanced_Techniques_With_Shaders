#define STB_IMAGE_IMPLEMENTATION

#include "GL_framework.h"

#include "Object.h"
#include "Billboard.h"
#include "Constants.h"
#include "Texture.h"
#include "Model.h"

// Declaració de la funció del loadCubemap
extern unsigned int loadCubemap(std::vector<std::string> faces);

Light light;
Scene scene;
std::vector<Object> objects; //--> Vector que emmagatzema els objectes que s'instancien a l'escena.
std::vector<Billboard> billboards; //--> Vector que emmagatzema les billboards que s'instancien a l'escena.
std::string s; //--> String declarat global per no redeclarar-lo a cada frame. S'usa pels noms del ImGui.

namespace RenderVars {
	float FOV = glm::radians(90.f);
	float zNear = 1.f;
	float zFar = 5000.f;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	struct prevMouse {
		float lastx, lasty;
		MouseEvent::Button button = MouseEvent::Button::None;
		bool waspressed = false;
	} prevMouse;

	float panv[3] = { 0.f, -5.f, -15.f };
	float rota[2] = { 0.f, 0.f };
}
namespace RV = RenderVars;

namespace GeometryShadersInfo
{
	// Explosion animation
	bool explosionAnim = false;
	float currentTime = 0;
	float auxTime = 0;
	float magnitude = 5;
	bool subDivide = false;

	// Billboards
	float width = 5.0f;
	float height = 10.0f;
}
namespace GSI = GeometryShadersInfo;

bool isFirstPerson = false;
glm::vec3 cameraOffset = glm::vec3(1.25, -4.5, 1);

///////// fw decl
namespace ImGui {
	void Render();
}
namespace Axis {
	void setupAxis();
	void cleanupAxis();
	void draw();
}
//////////////

void GLResize(int width, int height) {
	glViewport(0, 0, width, height);
	if (height != 0) RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	else RV::_projection = glm::perspective(RV::FOV, 0.f, RV::zNear, RV::zFar);
}

void GLmousecb(MouseEvent ev) {
	if (RV::prevMouse.waspressed && RV::prevMouse.button == ev.button) {
		float diffx = ev.posx - RV::prevMouse.lastx;
		float diffy = ev.posy - RV::prevMouse.lasty;
		switch (ev.button) {
		case MouseEvent::Button::Left: // ROTATE
			RV::rota[0] += diffx * 0.005f;
			RV::rota[1] += diffy * 0.005f;
			break;
		case MouseEvent::Button::Right: // MOVE XY
			RV::panv[0] += diffx * 0.03f;
			RV::panv[1] -= diffy * 0.03f;
			break;
		case MouseEvent::Button::Middle: // MOVE Z
			RV::panv[2] += diffy * 0.05f;
			break;
		default: break;
		}
	}
	else {
		RV::prevMouse.button = ev.button;
		RV::prevMouse.waspressed = true;
	}
	RV::prevMouse.lastx = ev.posx;
	RV::prevMouse.lasty = ev.posy;
}

//////////////////////////////////////////////// AXIS
namespace Axis {
	Shader axisShader;
	GLuint AxisVao;
	GLuint AxisVbo[3];

	float AxisVerts[] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0
	};
	float AxisColors[] = {
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0
	};
	GLubyte AxisIdx[] = {
		0, 1,
		2, 3,
		4, 5
	};

	void setupAxis() {
		glGenVertexArrays(1, &AxisVao);
		glBindVertexArray(AxisVao);
		glGenBuffers(3, AxisVbo);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisColors, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AxisVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, AxisIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		axisShader = Shader(axisVS, axisFS);

	}
	void cleanupAxis() {
		glDeleteBuffers(3, AxisVbo);
		glDeleteVertexArrays(1, &AxisVao);

		axisShader.CleanUpShader();
	}
	void draw() {
		glBindVertexArray(AxisVao);
		axisShader.Use();
		axisShader.SetMat4("mvpMat", 1, GL_FALSE, glm::value_ptr(RV::_MVP));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// CUBE
namespace Cube {
	GLuint cubeVao;
	GLuint cubeVbo[5];
	GLuint textureID[6];
	Shader cubeShader;
	glm::mat4 objMat = glm::mat4(1.f);
	unsigned char* data[6];
	glm::vec3 position = glm::vec3(0, 3, 0), rotation = glm::vec3(0, 0, 0), scale = glm::vec3(4, 4, 4);
	extern const float halfW = 0.5f;

	int texWidth, texHeight, nrChannels;
	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART
						   //   4---------7
						   //  /|        /|
						   // / |       / |
						   //5---------6  |
						   //|  0------|--3
						   //| /       | /
						   //|/        |/
						   //1---------2
	glm::vec3 verts[] = {
		glm::vec3(-halfW, -halfW, -halfW),
		glm::vec3(-halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW, -halfW),
		glm::vec3(-halfW,  halfW, -halfW),
		glm::vec3(-halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW, -halfW)
	};
	glm::vec3 norms[] = {
		glm::vec3(0.f, -1.f,  0.f),
		glm::vec3(0.f,  1.f,  0.f),
		glm::vec3(-1.f,  0.f,  0.f),
		glm::vec3(1.f,  0.f,  0.f),
		glm::vec3(0.f,  0.f, -1.f),
		glm::vec3(0.f,  0.f,  1.f)
	};

	glm::vec2 texCoords[] = {
		glm::vec2(0.f, 0.f), // Abajo izquierda
		glm::vec2(0.f, 1.f), // Arriba izquierda
		glm::vec2(1.f, 0.f), // Abajo derecha
		glm::vec2(1.f, 1.f)  // Arriba derecha
	};

	glm::vec3 cubeVerts[] = {
		verts[1], verts[0], verts[2], verts[3],
		verts[5], verts[6], verts[4], verts[7],
		verts[1], verts[5], verts[0], verts[4],
		verts[2], verts[3], verts[6], verts[7],
		verts[0], verts[4], verts[3], verts[7],
		verts[1], verts[2], verts[5], verts[6]
	};
	glm::vec2 cubeTexCoords[] = { // Les uvs que pertanyeixen a cada vertex
		texCoords[1], texCoords[0], texCoords[3], texCoords[2],
		texCoords[0], texCoords[2], texCoords[1], texCoords[3],
		texCoords[2], texCoords[3], texCoords[0], texCoords[1],
		texCoords[0], texCoords[2], texCoords[1], texCoords[3],
		texCoords[2], texCoords[3], texCoords[0], texCoords[1],
		texCoords[0], texCoords[2], texCoords[1], texCoords[3]
	};
	glm::vec3 cubeNorms[] = {
		norms[0], norms[0], norms[0], norms[0],
		norms[1], norms[1], norms[1], norms[1],
		norms[2], norms[2], norms[2], norms[2],
		norms[3], norms[3], norms[3], norms[3],
		norms[4], norms[4], norms[4], norms[4],
		norms[5], norms[5], norms[5], norms[5]
	};

	GLubyte cubeIdx[] = {
		0, 1, 2, 3, UCHAR_MAX,
		4, 5, 6, 7, UCHAR_MAX,
		8, 9, 10, 11, UCHAR_MAX,
		12, 13, 14, 15, UCHAR_MAX,
		16, 17, 18, 19, UCHAR_MAX,
		20, 21, 22, 23, UCHAR_MAX
	};

	void setupCube() {
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);

		for (int i = 0; i < 6; i++)
		{
			data[i] = stbi_load(cubeTexture[i], &texWidth, &texHeight, &nrChannels, 0);
			glGenTextures(1, &textureID[i]); //TEXTURES
			glBindTexture(GL_TEXTURE_2D, textureID[i]); //TEXTURES

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (data[i]) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]); //TEXTURES
			else std::cout << "Failed to load texture" << std::endl;

			stbi_image_free(data[i]);
		}
		// Array que determina quina textura s'aplica a cada vertex
		int cubeTextures[] = {
		textureID[0],textureID[0],textureID[0],textureID[0],
		textureID[1],textureID[1],textureID[1],textureID[1],
		textureID[2],textureID[2],textureID[2],textureID[2],
		textureID[3],textureID[3],textureID[3],textureID[3],
		textureID[4],textureID[4],textureID[4],textureID[4],
		textureID[5],textureID[5],textureID[5],textureID[5]
		};

		glGenBuffers(5, cubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextures), cubeTextures, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)3, 1, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);

		glPrimitiveRestartIndex(UCHAR_MAX);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[4]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cubeShader = Shader(cubeVS, cubeFS);
	}

	void cleanupCube() {
		glDeleteBuffers(5, cubeVbo);
		glDeleteVertexArrays(1, &cubeVao);
		cubeShader.CleanUpShader();
		for (int i = 0; i < 6; i++) glDeleteTextures(1, &textureID[i]);
	}

	void updateCube()
	{
		glm::mat4 t = glm::translate(glm::mat4(), position);
		glm::mat4 r1 = glm::rotate(glm::mat4(), rotation.x, glm::vec3(1, 0, 0));
		glm::mat4 r2 = glm::rotate(glm::mat4(), rotation.y + (ImGui::GetTime() * 0.5f), glm::vec3(0, 1, 0));
		glm::mat4 r3 = glm::rotate(glm::mat4(), rotation.z, glm::vec3(0, 0, 1));
		glm::mat4 s = glm::scale(glm::mat4(), scale);
		objMat = t * r1 * r2 * r3 * s;
	}

	void draw() {
		cubeShader.Use();
		glEnable(GL_PRIMITIVE_RESTART);

		glBindVertexArray(cubeVao);

		cubeShader.SetMat4("objMat", 1, GL_FALSE, glm::value_ptr(objMat));
		cubeShader.SetMat4("mv_Mat", 1, GL_FALSE, glm::value_ptr(RV::_modelView));
		cubeShader.SetMat4("mvpMat", 1, GL_FALSE, glm::value_ptr(RV::_MVP));
		cubeShader.SetFloat3("color", glm::vec3(1, 1, 1));
		for (int i = 0; i < 6; i++)
		{
			cubeShader.SetInt("text" + std::to_string(i), i); // Setejem la unitat de textura
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureID[i]);
		}
		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);

		glDisable(GL_PRIMITIVE_RESTART);
	}
}

namespace CubeMap {
	unsigned int skyboxVAO, skyboxVBO;
	Shader cubeMapShader;

	float skyboxVertices[] = {
		// positions          
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f, -10.0f,
		 10.0f,  10.0f,  10.0f,
		 10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f, -10.0f,
		 10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		 10.0f, -10.0f,  10.0f
	};

	std::vector<std::string> faces
	{
		"materials/right.jpg",
		"materials/left.jpg",
		"materials/top.jpg",
		"materials/bottom.jpg",
		"materials/front.jpg",
		"materials/back.jpg"
	};
	unsigned int cubemapTexture;

	void SetUp()
	{
		cubemapTexture = loadCubemap(faces);
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		cubeMapShader = Shader(skyBoxVs, skyBoxFs);
	}

	void cleanup() {
		glDeleteBuffers(1, &skyboxVBO);
		glDeleteVertexArrays(1, &skyboxVAO);
		cubeMapShader.CleanUpShader();
		glDeleteTextures(1, &cubemapTexture);
	}

	void draw() {
		glDepthFunc(GL_LEQUAL);
		cubeMapShader.Use();

		glm::mat4 cameraPoint = glm::translate(RV::_modelView, glm::vec3(glm::inverse(RV::_modelView)[3]));
		cubeMapShader.SetMat4("view", 1, GL_FALSE, glm::value_ptr(cameraPoint));
		cubeMapShader.SetMat4("projection", 1, GL_FALSE, glm::value_ptr(RV::_projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		//glUseProgram(0);

		glDepthFunc(GL_LESS);
	}
}

void GLinit(int width, int height) {
	srand(static_cast<unsigned>(time(nullptr)));
	stbi_set_flip_vertically_on_load(true);

	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);

	// Setup shaders & geometry
	light.type = Light::EType::DIRECTIONAL; //--> Inicialitzem el primer tipus d'iluminacó a direccional
	Axis::setupAxis();
	CubeMap::SetUp();

	// Carguem i generem les textures
	Texture camaroTexture(Texture::ETYPE::OBJ, carTexture);

	Texture treeText01(Texture::ETYPE::BB, treeTexture1);
	Texture treeText02(Texture::ETYPE::BB, treeTexture2);
	Texture treeText03(Texture::ETYPE::BB, treeTexture3);

	// Carguem i generem les models
	Model carModel(carObj);

	// Crida al constructor de la classe amb els diferents objectes
	Object camaro(carModel, camaroTexture.GetID(), glm::vec3(-3.11f, 1.6f, 2.71f), glm::vec3(0, 4.71f, 0), glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.1f, 0.1f, 0.1f), modelVS, modelFS, nullptr);


	// Emmagatzema els objectes creats al vector
	objects.push_back(camaro);

	// Carreguem varies textures diferents per poder spawnejar billboards randomitzades

	int random = 0;

	// Creem i emmagatzemem billboards

	// Alliberem memoria de textures

	scene = Scene::PHONG; //--> Inicialitzem la primera escena
}

void GLcleanup() {
	Axis::cleanupAxis();
	CubeMap::cleanup();

	/////////////////////////////////////////////////////TODO
	// Do your cleanup code here

	//Cleanup per cada objecte dins del vector
	for (Object obj : objects) obj.CleanUp();
	objects.clear(); //--> Allibera memòria del vector d'objectes

	for (Billboard bb : billboards) bb.CleanUp();
	billboards.clear(); //--> Allibera memòria del vector de billboards
}

void GLrender(float dt) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RV::_modelView = glm::mat4(1.f);

	// Mover Coches
	objects[0].rotation.y += dt;

	if (!isFirstPerson)
	{
		RV::_modelView = glm::translate(RV::_modelView, glm::vec3(RV::panv[0], RV::panv[1], RV::panv[2]));
		RV::_modelView = glm::rotate(RV::_modelView, RV::rota[1], glm::vec3(1.f, 0.f, 0.f));
		RV::_modelView = glm::rotate(RV::_modelView, RV::rota[0], glm::vec3(0.f, 1.f, 0.f));
	}
	else {
		RV::_modelView = glm::rotate(RV::_modelView, RV::rota[1], glm::vec3(1.f, 0.f, 0.f));
		RV::_modelView = glm::rotate(RV::_modelView, RV::rota[0], glm::vec3(0.f, 1.f, 0.f));
		RV::_modelView = glm::translate(RV::_modelView, cameraOffset);
		RV::_modelView = glm::rotate(RV::_modelView, -objects[0].rotation.y - glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
		RV::_modelView = glm::translate(RV::_modelView, -objects[0].position);
	}
	RV::_MVP = RV::_projection * RV::_modelView;
	Axis::draw();
	CubeMap::draw();

	for (Object obj : objects) { obj.Update(); obj.Draw(light); }

	ImGui::Render();
}

void GUI()
{
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	/////////////////////////////////////////////////////TODO
	// Do your GUI code here....

	ImGui::Checkbox("First Person View", &isFirstPerson);
	ImGui::DragFloat3("Camera Offset", (float*)&cameraOffset, 0.01f, -50.f, 50.f);

	ImGui::DragFloat3("Car Position", (float*)&objects[0].position, 0.01f, -50.f, 50.f);
	ImGui::DragFloat3("Car Rotation", (float*)&objects[0].rotation, 0.01f, -360.f, 360.f);
	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	bool show_test_window = false;
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}