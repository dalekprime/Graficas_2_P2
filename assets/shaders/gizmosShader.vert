#version 460 core

layout (location = 0) in vec3 aPos;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;

void main() {
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0f);
}