#pragma once
#ifndef GIZMOS_H
#define GIZMOS_H

#include <memory>
#include <vector>
//Librerias Internas
#include "glad/gl.h"
#include "../Shader.h"
#include "../BuffersManagement/VAO.h"
#include "../BuffersManagement/VBO.h"
#include "../BuffersManagement/EBO.h"

class Gizmos {
    std::unique_ptr<VAO> aabbVao;
    std::unique_ptr<VBO> aabbVbo;
    std::unique_ptr<EBO> aabbEbo;
    std::unique_ptr<VAO> axesVao;
    std::unique_ptr<VBO> axesVbo;
public:
    Gizmos();
    GLvoid UpdateAABB(const glm::vec3& min, const glm::vec3& max);
    GLvoid DrawAABB(ShaderProgram& shader);
    GLvoid DrawAxes(ShaderProgram& shader);
};

#endif