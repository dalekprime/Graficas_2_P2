#pragma once
#ifndef VAO_H
#define VAO_H

#include <glad/gl.h>
#include "VBO.h"
#include "EBO.h"

class VAO {
public:
	GLuint ID = 0;
	VAO();
	~VAO();
	GLvoid LinkAttrib(VBO& VBO, GLuint layout, GLint numComponents,
		GLenum type, GLsizei stride, GLuint offset);
	GLvoid LinkEBO(EBO& EBO);
	GLvoid Bind();
	GLvoid Unbind();
	GLvoid Delete();
};

#endif
