#pragma once

const int NUM_BILLBOARDS{ 10 };

const float MIRROR_FOV{ 70 };

const float PI{ glm::pi<float>() };

#pragma region Obj

// Cat
const char* catObj = "obj/cat.obj";

// Car
const char* carObj = "obj/camaro.obj";

// Floor
const char* floorObj = "obj/floor.obj";

// Tree
const char* treeObj = "obj/deadTree.obj";

#pragma endregion

#pragma region Texture Paths

// Cube
const char* cubeTextureC = "materials/checker_box.jpg";

const char* cubeTexture[6]{ "materials/cubeTexture2.jpg",
							"materials/cubeTexture1.jpg",
							"materials/cubeTexture0.jpg",
							"materials/cubeTexture3.jpg",
							"materials/cubeTexture4.jpg",
							"materials/cubeTexture5.jpg" };

// Billboards
const char* treeTexture1 = "materials/tree_texture.png";
const char* treeTexture2 = "materials/tree_texture2.png";
const char* treeTexture3 = "materials/tree_texture3.png";

// Models
const char* catTexture = "materials/cat_texture.jpg";
const char* carTexture = "materials/Camaro_AlbedoTransparency_alt.png";
const char* floorTexture = "materials/floor_texture.jpg";
const char* treeTexture = "materials/treeTexture.jpg";

#pragma endregion

#pragma region Shader Paths

// Axis
const char* axisVS = "shaders/axis/axisShader.vs";
const char* axisFS = "shaders/axis/axisShader.fs";

// Cube
const char* cubeVS = "shaders/cube/cubeShader.vs";
const char* cubeFS = "shaders/cube/cubeShader.fs";

// Imported Models
const char* modelVS = "shaders/models/shader.vs";
const char* modelFS = "shaders/models/shader.fs";
const char* carFS = "shaders/models/carShader.fs";

// Billboard
const char* bbVS = "shaders/billboard/bbshader.vs";
const char* bbFS = "shaders/billboard/bbshader.fs";
const char* bbGS = "shaders/billboard/bbshader.gs";

// Explosion
const char* explosionVS = "shaders/explosion/exshader.vs";
const char* explosionFS = "shaders/explosion/exshader.fs";
const char* explosionGS = "shaders/explosion/exshader.gs";

// Skybox
const char* skyBoxVS = "shaders/skybox/skyBoxShader.vs";
const char* skyBoxFS = "shaders/skybox/skyBoxShader.fs";

// FrameBuffer
const char* frameBufferVS = "shaders/frameBuffer/frameBuffer.vs";
const char* frameBufferFS = "shaders/frameBuffer/frameBuffer.fs";
const char* frameBufferGS = "shaders/frameBuffer/frameBuffer.gs";

#pragma endregion
