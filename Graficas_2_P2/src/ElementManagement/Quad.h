#pragma once
#ifndef QUAD_H
#define QUAD_H

#include <memory>
//Librerias Internas
#include "glad/gl.h"
#include "../BuffersManagement/VAO.h"
#include "../BuffersManagement/VBO.h"
#include "../BuffersManagement/EBO.h"

class Quad {
    std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;
    std::vector<Vertex> vertex = {
		Vertex{glm::vec3{-1.0f, 1.0f, 0.0f}},
		Vertex{glm::vec3{-1.0f, -1.0f, 0.0f}},
		Vertex{glm::vec3{1.0f, 1.0f, 0.0f}},
		Vertex{glm::vec3{-1.0f, -1.0f, 0.0f}},
		Vertex{glm::vec3{1.0f, 1.0f, 0.0f}},
		Vertex{glm::vec3{1.0f, -1.0f, 0.0f}}
	};
public:
    Quad();
    GLvoid Draw();
};

#endif