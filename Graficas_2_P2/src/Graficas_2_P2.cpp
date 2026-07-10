#include "Graficas_2_P2.h"

MainEngine::MainEngine(int width, int height) : width(width), height(height) {
	SetupWindow();
}

void MainEngine::SetupWindow() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		exit(EXIT_FAILURE);
	}
	//Indica a GLFW que versión de OpenGL queremos usar
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//Indica a GLFW que queremos usar el perfil core de OpenGL
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Crear la ventana
	window = glfwCreateWindow(width, height, "Proyecto#2: Volumen Render", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		std::cerr << "Failed to create GLFW window\n";
		exit(EXIT_FAILURE);
	}
	//Hacer el contexto de la ventana actual
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	//Cargar las funciones de OpenGL usando GLAD
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	//Configurar el viewport
	glViewport(0, 0, width, height);
	//Habilitar el depth test para objetos 3D
	glEnable(GL_DEPTH_TEST);
	//Habilitar el blending para la transparencia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Inicializar ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
	//Crear los programas de shaders
	rayMarchingShader = std::make_unique<ShaderProgram>("assets/shaders/rayMarchingShader.vert", "assets/shaders/rayMarchingShader.frag");
	actualShader = rayMarchingShader.get();
	//Crear la cámara
	camera = std::make_unique<Camera>(width, height, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void MainEngine::MainLoop() {
	//Noise Texture
	std::vector<GLfloat> noiseData(128 * 128 * 128);
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(4);
	noise.SetSeed(time(0));
	int index = 0;
	for (int z = 0; z < 128; z++) {
		for (int y = 0; y < 128; y++) {
			for (int x = 0; x < 128; x++) {
				float rawNoise = noise.GetNoise((float)x, (float)y, (float)z);
				noiseData[index++] = (rawNoise + 1.0f) / 2.0f;
			}
		}
	}
	glGenTextures(1, &noiseText);
	glBindTexture(GL_TEXTURE_3D, noiseText);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, 128, 128, 128, 0, GL_RED, GL_FLOAT, noiseData.data());
	// Setup VAO and VBO
	vao = std::make_unique<VAO>();
	vbo = std::make_unique<VBO>(vertex);
	vao->LinkAttrib(*vbo, 0, 2, GL_FLOAT, sizeof(Vertex), 0);

	//Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Update();
		DrawUI();
		glfwSwapBuffers(window);
	}
	Cleanup();
}

void MainEngine::Update() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->Inputs(window, deltaTime);
	camera->updateMatrix(45.0f, 0.1f, 100.0f);
	camera->matrix(*actualShader);
	actualShader->Activate();
	actualShader->SetInt("uSteps", steps);
	actualShader->SetFloat("uStepSize", stepSize);
	vao->Bind();
	glBindTexture(GL_TEXTURE_3D, noiseText);
	glDrawArrays(GL_TRIANGLES, 0, vertex.size());
}

void MainEngine::DrawUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SliderInt("Steps", &steps, 0, 500);
	ImGui::SliderFloat("StepSize", &stepSize, 0, 0.1, "%.3f");
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainEngine::Cleanup() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main() {
	MainEngine engine(800, 600);
	engine.MainLoop();
	return 0;
}
