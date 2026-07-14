#include "Quad.h"

Quad::Quad() {
    
    vao = std::make_unique<VAO>();
    vbo = std::make_unique<VBO>(vertex);
    vao->LinkAttrib(*vbo, 0, 2, GL_FLOAT, sizeof(Vertex), 0);
}

GLvoid Quad::Draw() {
    vao->Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertex.size());
}