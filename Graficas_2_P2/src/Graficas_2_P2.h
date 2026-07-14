#pragma once
#ifndef GRAFICAS_2_P2
#define  GRAFICAS_2_P2

#include <iostream>
#include <vector>
#include <memory>
#include <ctime>
#include "FastNoiseLite.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//Librerias Internas
#include "Shader.h"
#include "CameraManagement/Camera.h"
#include "ModelManagement/volumenTexture.h"
#include "ElementManagement/Quad.h"
#include "ElementManagement/Gizmos.h"

class MainEngine {
	GLuint width, height;
	GLFWwindow* window;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Quad> quad;
	std::unique_ptr<Gizmos> gizmos;
	std::unique_ptr<ShaderProgram> rayMarchingShader;
	std::unique_ptr<ShaderProgram> rayBandingShader;
	std::unique_ptr<ShaderProgram> terrainGenerationShader;
	std::unique_ptr<ShaderProgram> pbRayMarchingShader;
	std::unique_ptr<ShaderProgram> marchingCubesShader;
	std::unique_ptr<ShaderProgram> gizmosShader;
	ShaderProgram* actualShader;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;
	//Varios
	GLint shadowOn = true;
	GLfloat stepSize = 0.0039;
	GLboolean isDragging = false;
	GLdouble lastX = 400.0, lastY = 300.0;
	std::unique_ptr<VolumenTexture> noiseText;
	std::unique_ptr<VolumenTexture> rawText;
	std::unique_ptr<VolumenTexture> nrrdText;
	std::unique_ptr<VolumenTexture> transferFuncText;
	GLfloat startingPoint = 0.0f;
public:
	MainEngine(GLuint width, GLuint height);
	GLvoid SetupWindow();
	GLvoid MainLoop();
	GLvoid Update();
	GLvoid DrawUI();
	GLvoid Cleanup();
};

#endif 


