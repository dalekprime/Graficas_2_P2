#pragma once
#ifndef VOLUMENTEXTURE_H
#define VOLUMENTEXTURE_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "FastNoiseLite.h"
#include "glad/gl.h"
//Librerias Internas
#include "../Shader.h"

enum TextureType {
	NOISE3D,
	NOISE2D,
	RAW,
	NRRD,
	TRANSFER
};

struct RGBA_D{
	GLfloat r, g, b, a;
	GLfloat density;
	bool active = true;
};

class VolumenTexture {
public:
	GLuint ID = 0;
	std::string path = "";
	GLuint width = 0, height = 0, depth = 0;
	std::shared_ptr<std::vector<RGBA_D>> colorsWithDensity;
	VolumenTexture(TextureType type, GLuint width, GLuint height, GLuint depth, std::string path);
	~VolumenTexture();
	GLvoid CreateNoise3D();
	GLvoid CreateNoise2D();
	GLvoid LoadRaw();
	GLvoid LoadNRRD();
	GLvoid LoadTransfer();
	GLvoid Bind(GLuint unit);
	GLvoid Unbind(GLuint unit);
	GLvoid Delete();
};

#endif