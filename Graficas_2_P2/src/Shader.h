#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <iostream>

//Enum para identificar el tipo de shader
enum ShaderType {
    VERTEX,
    FRAGMENT,
    PROGRAM
};

//Función para leer el contenido de un archivo y devolverlo como una cadena
std::string get_file_contents(const GLchar* filename);
//Clase para manejar shaders
class ShaderProgram {
public:
    GLuint ID;
    ShaderProgram(const GLchar* vertexFile, const GLchar* fragmentFile);
    ~ShaderProgram();
    GLvoid CheckCompileErrors(GLuint shader, ShaderType type);
    GLvoid Activate();
    GLvoid Deactivate();
    GLvoid Delete();
	//Funciones para establecer uniformes
    GLvoid SetInt(const GLchar* name, const GLint num);
    GLvoid SetFloat(const GLchar* name, const GLfloat num);
    GLvoid SetFloat2(const GLchar* name, const GLfloat num1, const GLfloat num2);
    GLvoid SetVec2(const GLchar* name, const glm::vec2& vector);
    GLvoid SetVec3(const GLchar* name, const glm::vec3& vector);
    GLvoid SetVec4(const GLchar* name, const glm::vec4& vector);
    GLvoid SetMatrix4(const GLchar* name, const glm::mat4& matrix);
};

#endif
