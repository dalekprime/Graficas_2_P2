#include "Camera.h"
#include <imgui.h>

Camera::Camera(GLuint width, GLuint height, glm::vec3 position, glm::vec3 orientation, glm::vec3 up) {
	//tamano de la ventana
	this->width = width;
	this->height = height;
	//Posicion de la camaras
	this->position = position;
	//Orientacion de la camara
	this->orientation = orientation;
	//Vector up de la camara (usado para rotar la camara)
	this->up = up;
}

GLvoid Camera::matrix(ShaderProgram& shader) {
	shader.SetVec3("uViewPos", position);
	shader.SetVec3("uViewDir", orientation);
	shader.SetVec3("uViewUp", up);
	shader.SetMatrix4("uViewMatrix", view);
	shader.SetMatrix4("uProjMatrix", projection);
}

GLvoid Camera::updateMatrix(GLfloat FOVdeg, GLfloat nearPlane, GLfloat farPlane) {
	//Crear la matriz de vista y proyeccion
	view = glm::lookAt(position, position + orientation, up);
	projection = glm::perspective(glm::radians(FOVdeg), (GLfloat)width / height, nearPlane, farPlane);
	cameraMatrix = projection * view;
}

GLvoid Camera::Inputs(GLFWwindow* window, GLfloat deltaTime) {
	GLfloat realSpeed = speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += realSpeed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= realSpeed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= glm::normalize(glm::cross(orientation, up)) * realSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += glm::normalize(glm::cross(orientation, up)) * realSpeed;
	}
	//Mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (ImGui::GetIO().WantCaptureMouse) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstClick = true;
			return;
		}
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (firstClick) {
			glfwSetCursorPos(window, (width / 2.0), (height / 2.0));
			firstClick = false;
		}
		GLdouble mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		GLfloat rotX = sensitivity * (GLfloat)(mouseY - (height / 2.0)) / height;
		GLfloat rotY = sensitivity * (GLfloat)(mouseX - (width / 2.0)) / width;
		glm::vec3 right = glm::normalize(glm::cross(orientation, up));
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), right);
		glm::vec3 newPosition = glm::rotate(position, glm::radians(-rotX), right);
		if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) ||
			(glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
			orientation = newOrientation;
			position = newPosition;
		}
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);
		position = glm::rotate(position, glm::radians(-rotY), up);
		glfwSetCursorPos(window, (width / 2.0), (height / 2.0));
	} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}
