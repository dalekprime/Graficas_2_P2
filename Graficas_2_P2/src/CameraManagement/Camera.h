#pragma once
#ifndef  CAMERA_H
#define  CAMERA_H

#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../shader.h"

class Camera {
public:
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	GLuint width;
	GLuint height;
	GLfloat sensitivity = 100.0f;
	GLfloat speed = 5.0f;
	GLboolean firstClick = true;
	GLfloat fov = 45.0f;
	GLfloat znear = 0.1f;
	GLfloat zfar = 100.0f;
	std::string name = "";
	Camera(GLuint width, GLuint height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up);
	GLvoid updateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane);
	GLvoid matrix(ShaderProgram& shader);
	GLvoid Inputs(GLFWwindow* window, GLfloat deltaTime);
};

#endif
