#include "VolumenTexture.h"

VolumenTexture::VolumenTexture(TextureType type, GLuint width, GLuint height, GLuint depth, std::string path) : 
	width(width), height(height), depth(depth), path(path) {
	switch (type) {
		case 0:
			CreateNoise3D();
		break;
		case 1:
			CreateNoise2D();
		break;
		case 2:
			LoadRaw();
		break;
		case 3:
			LoadNRRD();
		break;
	}
}

VolumenTexture::~VolumenTexture() {
	Delete();
}

GLvoid VolumenTexture::CreateNoise3D() {
	std::vector<GLfloat> noiseData(width * height * depth);
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(4);
	noise.SetSeed(time(0));
	int index = 0;
	for (int z = 0; z < depth; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				GLfloat rawNoise = noise.GetNoise((GLfloat)x, (GLfloat)y, (GLfloat)z);
				noiseData[index++] = (rawNoise + 1.0f) / 2.0f;
			}
		}
	}
	//Crea el espacio para la textura
	glCreateTextures(GL_TEXTURE_3D, 1, &ID);
	//Reserva el espacio de memoria para la textura
	glTextureStorage3D(ID, 1, GL_R32F, width, height, depth);
	//Envia la textura a la GPU
	glTextureSubImage3D(ID, 0, 0, 0, 0, width, height, depth, GL_RED, GL_FLOAT, noiseData.data());
	//Colocan los parametros de textura
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

GLvoid VolumenTexture::CreateNoise2D() {
	std::vector<GLfloat> noiseData(width * height);
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(4);
	noise.SetSeed(time(0));
	int index = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			GLfloat rawNoise = noise.GetNoise((GLfloat)x, (GLfloat)y);
			noiseData[index++] = (rawNoise + 1.0f) / 2.0f;
		}
	}
	if (ID == 0) {
		// Crea el espacio para la textura 2D
		glCreateTextures(GL_TEXTURE_2D, 1, &ID);
		// Reserva el espacio de memoria usando un canal flotante
		glTextureStorage2D(ID, 1, GL_R32F, width, height);
	}
	//Envia la textura a la GPU
	glTextureSubImage2D(ID, 0, 0, 0, width, height, GL_RED, GL_FLOAT, noiseData.data());	
	//Colocan los parametros de textura
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

GLvoid VolumenTexture::LoadRaw() {
	//Abrir archivo en modo binario
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "ERROR: no pudo abrir el archivo RAW: " << path << std::endl;
		return;
	}
	//Asumimos 8 bits para los raw
	size_t numVoxels = width * height * depth;
	std::vector<GLubyte> rawData(numVoxels);
	//Lee todo el bloque de bytes
	file.read(reinterpret_cast<char*>(rawData.data()), numVoxels);
	file.close();
	//Crea el espacio para la textura
	glCreateTextures(GL_TEXTURE_3D, 1, &ID);
	//Reserva el espacio de memoria para la textura
	glTextureStorage3D(ID, 1, GL_R8, width, height, depth);
	//Envia la textura a la GPU
	glTextureSubImage3D(ID, 0, 0, 0, 0, width, height, depth, GL_RED, GL_UNSIGNED_BYTE, rawData.data());
	//Colocan los parametros de textura
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

GLvoid VolumenTexture::LoadNRRD() {
	//Abrir archivo en modo binario
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "ERROR: no pudo abrir el archivo NRRD: " << path << std::endl;
		return;
	}
	std::string line;
	bool isRawEncoding = false;
	int bitDepth = 8;
	//Limpiar Saltos de Linea
	auto safeGetline = [](std::ifstream& is, std::string& t) -> std::istream& {
		t.clear();
		std::istream::sentry se(is, true);
		std::streambuf* sb = is.rdbuf();
		for (;;) {
			int c = sb->sbumpc();
			switch (c) {
			case '\n':
				return is;
			case '\r':
				if (sb->sgetc() == '\n') sb->sbumpc();
				return is;
			case std::streambuf::traits_type::eof():
				if (t.empty()) is.setstate(std::ios::eofbit);
				return is;
			default:
				t += (char)c;
			}
		}
	};
	//Extraer los Metadatos
	while (safeGetline(file, line)) {
		if (line.empty() || line == "\r") break;
		if (line.find("sizes:") != std::string::npos) {
			std::stringstream ss(line.substr(line.find("sizes:") + 6));
			ss >> width >> height >> depth;
		}
		else if (line.find("type:") != std::string::npos) {
			if (line.find("float") != std::string::npos) {
				bitDepth = 32;
			}
			else if (line.find("uint8") != std::string::npos || line.find("unsigned char") != std::string::npos) {
				bitDepth = 8;
			}
		}
		else if (line.find("encoding:") != std::string::npos) {
			if (line.find("raw") != std::string::npos) isRawEncoding = true;
		}
	}
	if (!isRawEncoding) {
		std::cerr << "ERROR: El archivo no es RAW." << std::endl;
		return;
	}
	//Extraer la Data
	size_t numVoxels = width * height * depth;
	glCreateTextures(GL_TEXTURE_3D, 1, &ID);
	if (bitDepth == 8) {
		std::vector<GLubyte> nrrdData(numVoxels);
		file.read(reinterpret_cast<char*>(nrrdData.data()), numVoxels);
		glTextureStorage3D(ID, 1, GL_R8, width, height, depth);
		glTextureSubImage3D(ID, 0, 0, 0, 0, width, height, depth, GL_RED, GL_UNSIGNED_BYTE, nrrdData.data());
	}
	else if (bitDepth == 32) {
		std::vector<GLfloat> nrrdData(numVoxels);
		file.read(reinterpret_cast<char*>(nrrdData.data()), numVoxels * sizeof(GLfloat));
		glTextureStorage3D(ID, 1, GL_R32F, width, height, depth);
		glTextureSubImage3D(ID, 0, 0, 0, 0, width, height, depth, GL_RED, GL_FLOAT, nrrdData.data());
	}
	file.close();
	//Colocan los parametros de textura
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

GLvoid VolumenTexture::LoadTransfer() {
	std::vector<GLfloat> tfData(width * 4, 0.0f);
	if (!colorsWithDensity) return;
	std::vector<RGBA_D> activeColors;
	for (const auto& color : *colorsWithDensity) {
		if (color.active) {
			RGBA_D normColor = color;
			activeColors.push_back(normColor);
		}
	}
	if (activeColors.size() == 1) {
		for (int i = 0; i < width; i++) {
			tfData[i * 4 + 0] = activeColors[0].r;
			tfData[i * 4 + 1] = activeColors[0].g;
			tfData[i * 4 + 2] = activeColors[0].b;
			tfData[i * 4 + 3] = activeColors[0].a;
		}
	} else if (activeColors.size() >= 2) {
		std::sort(activeColors.begin(), activeColors.end(), [](const RGBA_D& a, const RGBA_D& b) {
			return a.density < b.density;
		});
		for (int i = 0; i < width; i++) {
			float currentDensity = (float)i / (float)(width - 1);
			auto actualColor = activeColors.begin();
			while (actualColor != activeColors.end() - 1 && currentDensity > (actualColor + 1)->density) {
				actualColor++;
			}
			auto nextColor = actualColor + 1;
			if (nextColor == activeColors.end()) {
				nextColor = actualColor;
			}
			float range = nextColor->density - actualColor->density;
			float t = 0.0f;
			if(range > 0.0001f) {
				t = (currentDensity - actualColor->density) / range;
				t = std::max(0.0f, std::min(1.0f, t));
			}
			tfData[i * 4 + 0] = actualColor->r + t * (nextColor->r - actualColor->r);
			tfData[i * 4 + 1] = actualColor->g + t * (nextColor->g - actualColor->g);
			tfData[i * 4 + 2] = actualColor->b + t * (nextColor->b - actualColor->b);
			tfData[i * 4 + 3] = actualColor->a + t * (nextColor->a - actualColor->a);
		}
	}
	if (ID == 0) {
		//Crea el espacio para la textura
		glCreateTextures(GL_TEXTURE_1D, 1, &ID);
		//Reserva el espacio de memoria para la texturas
		glTextureStorage1D(ID, 1, GL_RGBA32F, width);
	}
	//Envia la textura a la GPU
	glTextureSubImage1D(ID, 0, 0, width, GL_RGBA, GL_FLOAT, tfData.data());
	//Colocan los parametros de textura
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

GLvoid VolumenTexture::Bind(GLuint unit) {
	glBindTextureUnit(unit, ID);
}

GLvoid VolumenTexture::Unbind(GLuint unit) {
	glBindTextureUnit(unit, 0);
}

GLvoid VolumenTexture::Delete() {
	if (ID != 0) {
		glDeleteTextures(1, &ID);
		ID = 0;
	}
}