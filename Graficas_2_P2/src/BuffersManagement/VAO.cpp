#include "VAO.h"

VAO::VAO() {
	glCreateVertexArrays(1, &ID);
}

VAO::~VAO() {
	Delete();
}

GLvoid VAO::LinkAttrib(VBO& VBO, GLuint layout, GLint numComponents, GLenum type,
	GLsizei stride, GLuint offset) {
	//Enlaza el VBO en el BindingIndex de el VAO
	glVertexArrayVertexBuffer(ID, 0, VBO.ID, 0, stride);
	//Define el Formato de Atributo
	glVertexArrayAttribFormat(ID, layout, numComponents, type, GL_FALSE, offset);
	//Conecta el Atributo con el VBO en el BindingIndex indicado
	glVertexArrayAttribBinding(ID, layout, 0);
	//Enciende el Atributo y lo deja listo para usar
	glEnableVertexArrayAttrib(ID, layout);
}

GLvoid VAO::LinkEBO(EBO& EBO) {
	glVertexArrayElementBuffer(ID, EBO.ID);
}

GLvoid VAO::Bind() {
	glBindVertexArray(ID);
}

GLvoid VAO::Unbind() {
	glBindVertexArray(0);
}

GLvoid VAO::Delete() {
	if (ID != 0) {
		glDeleteVertexArrays(1, &ID);
		ID = 0;
	}
}
