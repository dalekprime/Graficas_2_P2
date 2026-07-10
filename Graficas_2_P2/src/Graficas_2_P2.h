#pragma once
#ifndef GRAFICAS_2_P2
#define  GRAFICAS_2_P2

#include <iostream>
#include <vector>
#include "FastNoiseLite.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//Librerias Internas
#include "Shader.h"
#include "BuffersManagement/VAO.h"
#include "BuffersManagement/VBO.h"
#include "BuffersManagement/EBO.h"

class MainEngine {
	int width, height;
	GLFWwindow* window;
	std::unique_ptr<ShaderProgram> rayMarchingShader;
	ShaderProgram* actualShader;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	//Varios
	int steps = 200;
	float stepSize = 0.015;
	float yaw = 0.0f, pitch = 0.0f;
	bool isDragging = false;
	double lastX = 400.0, lastY = 300.0;
	GLuint noiseText;
	std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;
	//Buffers
	std::vector<Vertex> vertex = {
		Vertex{glm::vec2{-1.0f, 1.0f}},
		Vertex{glm::vec2{-1.0f, -1.0f}},
		Vertex{glm::vec2{1.0f, 1.0f}},
		Vertex{glm::vec2{-1.0f, -1.0f}},
		Vertex{glm::vec2{1.0f, 1.0f}},
		Vertex{glm::vec2{1.0f, -1.0f}}
	};
public:
	MainEngine(int width, int height);
	void SetupWindow();
	void MainLoop();
	void Update();
	void DrawUI();
	void Cleanup();
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif 


