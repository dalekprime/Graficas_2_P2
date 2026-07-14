#include "Graficas_2_P2.h"

MainEngine::MainEngine(GLuint width, GLuint height) : width(width), height(height) {
	SetupWindow();
}

GLvoid MainEngine::SetupWindow() {
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
	//Inicializar ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
	//Inicializar Elementos
	quad = std::make_unique<Quad>();
	gizmos = std::make_unique<Gizmos>();
	//Crear los programas de shaders
	rayMarchingShader = std::make_unique<ShaderProgram>("assets/shaders/rayMarchingShader.vert", "assets/shaders/rayMarchingShader.frag");
	gizmosShader = std::make_unique<ShaderProgram>("assets/shaders/gizmosShader.vert", "assets/shaders/gizmosShader.frag");
	actualShader = rayMarchingShader.get();
	//Crear la cámara
	camera = std::make_unique<Camera>(width, height, 
		glm::vec3(0.0f, 0.0f, 3.0f), 
		glm::vec3(0.0f, 0.0f, -1.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f));
}

GLvoid MainEngine::MainLoop() {
	//Noise Texture
	noiseText = std::make_unique<VolumenTexture>(NOISE3D, 128, 128, 128, "");
	rawText = std::make_unique<VolumenTexture>(RAW, 128, 256, 256, "assets/models/vis_male_128x256x256_uint8.raw");
	nrrdText = std::make_unique<VolumenTexture>(NRRD, 0, 0, 0, "assets/models/bonsai_256x256x256_uint8.nrrd");
	//Transfer Function Texture
	transferFuncText = std::make_unique<VolumenTexture>(TRANSFER, 256, 0, 0, "");
	transferFuncText->colorsWithDensity = std::make_shared<std::vector<RGBA_D>>(std::vector<RGBA_D>{
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true },
		{ 0.75f, 1.0f, 1.0f, 0.05f, 0.33f, true },
		{ 1.0f, 0.25f, 0.3f, 0.25f, 0.66f, true },
		{ 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, true },
	});
	transferFuncText->LoadTransfer();
	//Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Update();
		DrawUI();
		glfwSwapBuffers(window);
	}
	Cleanup();
}

GLvoid MainEngine::Update() {
	srand(time(0));
	
	//Calculo de DeltaTime
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	//Limpiar Frame
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Camara
	camera->Inputs(window, deltaTime);
	camera->updateMatrix(45.0f, 0.1f, 100.0f);
	camera->matrix(*actualShader);

	//Draw Quad
	actualShader->Activate();
	//stepSize = 0.01f + ((sin(currentFrame * 0.2f) + 1.0f) * 0.5f) * 0.09f;
	actualShader->SetFloat("uStepSize", stepSize);
	actualShader->SetFloat("uFov", 45.0f);
	actualShader->SetVec3("uAABBMin", glm::vec3(-0.5f, -0.5f, -0.5f));
	actualShader->SetVec3("uAABBMax", glm::vec3(0.5f, 0.5f, 0.5f));
	actualShader->SetInt("uVolumenText", 0);
	actualShader->SetInt("uTransferFuncText", 1);
	actualShader->SetInt("uShadowOn", shadowOn);
	actualShader->SetFloat("uAspectRatio", (GLfloat) width / height);
	actualShader->SetFloat("uStartingPoint", startingPoint);
	//Enviar Texturas
	//rawText->Bind(0);
	noiseText->Bind(0);
	//nrrdText->Bind(0);
	transferFuncText->Bind(1);
	quad->Draw();

	//Draw Gizmos
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::vec3 aabbMin(-0.5f, -0.5f, -0.5f);
	glm::vec3 aabbMax(0.5f, 0.5f, 0.5f);
	gizmos->UpdateAABB(aabbMin, aabbMax);
	camera->matrix(*gizmosShader);
	gizmosShader->SetMatrix4("uModelMatrix", glm::mat4(1.0f));
	gizmos->DrawAABB(*gizmosShader);
	gizmos->DrawAxes(*gizmosShader);
}

GLvoid MainEngine::DrawUI() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SliderFloat("StepSize", &stepSize, 0.0001, 0.01, "%.4f");
	ImGui::SliderInt("Sombras", &shadowOn, 0, 1);
	ImGui::SliderFloat("Punto de Inicio", &startingPoint, 0, 1, "%.3f");
	if (ImGui::CollapsingHeader("Curva de Densidades")) {
		bool updateTransferFunction = false;
		auto& colors = *(transferFuncText->colorsWithDensity);
		if (ImGui::Button("Añadir Punto")) {
			colors.push_back({ 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, true });
			updateTransferFunction = true;
		}
		ImGui::Separator();
		int indexToDelete = -1;
		for (int i = 0; i < colors.size(); i++) {
			ImGui::PushID(i);
			if (ImGui::Checkbox("Activo", &colors[i].active)) {
				updateTransferFunction = true;
			}
			ImGui::SameLine();
			ImGui::Text("Punto %d", i);
			if (ImGui::SliderFloat("Densidad", &colors[i].density, 0.0f, 1.0f, "%.4f")) updateTransferFunction = true;
			float col[3] = { colors[i].r, colors[i].g, colors[i].b };
			if (ImGui::ColorEdit3("Color", col)) {
				colors[i].r = col[0];
				colors[i].g = col[1];
				colors[i].b = col[2];
				updateTransferFunction = true;
			}
			if (ImGui::SliderFloat("Alpha", &colors[i].a, 0.0f, 1.0f, "%.4f")) updateTransferFunction = true;
			ImGui::SameLine();
			if (ImGui::Button("X")) {
				if (colors.size() > 1) {
					indexToDelete = i;
				}
			}
			ImGui::Separator();
			ImGui::PopID();
		}
		if (indexToDelete != -1) {
			colors.erase(colors.begin() + indexToDelete);
			updateTransferFunction = true;
		}
		if (updateTransferFunction) {
			transferFuncText->LoadTransfer();
		}
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GLvoid MainEngine::Cleanup() {
	quad.reset();
	rayMarchingShader.reset();
	rayBandingShader.reset();
	terrainGenerationShader.reset();
	pbRayMarchingShader.reset();
	marchingCubesShader.reset();
	gizmosShader.reset();
	gizmos.reset();
	noiseText.reset();
	rawText.reset();
	nrrdText.reset();
	transferFuncText.reset();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main() {
	MainEngine engine(1200, 800);
	engine.MainLoop();
	return 0;
}
