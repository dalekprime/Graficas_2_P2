#include "Gizmos.h"

Gizmos::Gizmos() {
    std::vector<Vertex> aabbVertices(8, Vertex{glm::vec3(0.0f)});
    std::vector<GLuint> aabbIndices = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    aabbVao = std::make_unique<VAO>();
    aabbVbo = std::make_unique<VBO>(aabbVertices);
    aabbEbo = std::make_unique<EBO>(aabbIndices);
    aabbVao->LinkAttrib(*aabbVbo, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    aabbVao->LinkEBO(*aabbEbo);
    //Axes
    std::vector<Vertex> axesVertices = {
        Vertex{glm::vec3(0.0f, 0.0f, 0.0f)}, Vertex{glm::vec3(1.0f, 0.0f, 0.0f)},
        Vertex{glm::vec3(0.0f, 0.0f, 0.0f)}, Vertex{glm::vec3(0.0f, 1.0f, 0.0f)},
        Vertex{glm::vec3(0.0f, 0.0f, 0.0f)}, Vertex{glm::vec3(0.0f, 0.0f, 1.0f)}
    };
    axesVao = std::make_unique<VAO>();
    axesVbo = std::make_unique<VBO>(axesVertices);
    axesVao->LinkAttrib(*axesVbo, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
}

GLvoid Gizmos::UpdateAABB(const glm::vec3& min, const glm::vec3& max) {
    std::vector<Vertex> aabbVertices = {
        Vertex{glm::vec3(min.x, min.y, min.z)},
        Vertex{glm::vec3(max.x, min.y, min.z)},
        Vertex{glm::vec3(max.x, min.y, max.z)},
        Vertex{glm::vec3(min.x, min.y, max.z)},
        Vertex{glm::vec3(min.x, max.y, min.z)},
        Vertex{glm::vec3(max.x, max.y, min.z)},
        Vertex{glm::vec3(max.x, max.y, max.z)},
        Vertex{glm::vec3(min.x, max.y, max.z)}
    };
    aabbVbo->Update(aabbVertices);
}

GLvoid Gizmos::DrawAABB(ShaderProgram& shader) {
    shader.Activate();
    shader.SetVec4("uColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    aabbVao->Bind();
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
}

GLvoid Gizmos::DrawAxes(ShaderProgram& shader) {
    shader.Activate();
    axesVao->Bind();
    //X Axis
    shader.SetVec4("uColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    glDrawArrays(GL_LINES, 0, 2);
    //Y Axis
    shader.SetVec4("uColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    glDrawArrays(GL_LINES, 2, 2);
    //Z Axis
    shader.SetVec4("uColor", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    glDrawArrays(GL_LINES, 4, 2);
}