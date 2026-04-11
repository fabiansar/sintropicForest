#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GraphicsEngine.h"
#include "Shaders.h"
#include "Config.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <random>
#include <vector>
#include <cmath>

/**
 * COMPILAR UN SHADER
 * 
 * Pasos:
 * 1. Crea un nuevo shader del tipo especificado (vértice o fragmento)
 * 2. Carga el código fuente del shader
 * 3. Lo compila
 * 4. Verifica si hubo errores de compilación
 * 5. Si hay errores, los imprime en la consola
 * 6. Devuelve el ID del shader compilado
*/
static unsigned int compileShader(const char* source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

// ============================================================================
// CONSTRUCTOR
// ============================================================================

GraphicsEngine::GraphicsEngine() 
    : window(nullptr), 
      shaderProgram(0), terrainShaderProgram(0), VAO(0), VBO(0),
      terrainVAO(0), terrainVBO(0), terrainEBO(0), terrainVertexCount(0),
      masterVolume(0.8f),
      elapsedTime(0.0f),
      perlinNoise(nullptr) {
    // Sistemas modulares se inicializarán después de que GLFW esté listo
    perlinNoise = new PerlinNoise(42);  // Seed para consistencia
}

// ============================================================================
// INICIALIZACIÓN
// ============================================================================

bool GraphicsEngine::initialize() {
    // PASO 1: Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // PASO 2: Configurar contexto OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // PASO 3: Crear la ventana principal
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
        "3D Graphics Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // PASO 4: Inicializar GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return false;
    }

    // PASO 5: Configurar ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // PASO 6: Configurar OpenGL
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glPointSize(5.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // PASO 7: Compilar shaders
    unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // PASO 7B: Compilar shaders del terreno
    unsigned int terrainVertexShader = compileShader(terrainVertexShaderSource, GL_VERTEX_SHADER);
    unsigned int terrainFragmentShader = compileShader(terrainFragmentShaderSource, GL_FRAGMENT_SHADER);

    terrainShaderProgram = glCreateProgram();
    glAttachShader(terrainShaderProgram, terrainVertexShader);
    glAttachShader(terrainShaderProgram, terrainFragmentShader);
    glLinkProgram(terrainShaderProgram);

    glGetProgramiv(terrainShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(terrainShaderProgram, 512, nullptr, infoLog);
        std::cerr << "Terrain program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(terrainVertexShader);
    glDeleteShader(terrainFragmentShader);

    // PASO 8: Inicializar puntos de luz
    initializeLights();

    // PASO 9: Generar terreno
    generateTerrain();
    
    // PASO 9B: Generar geometría procedural de plantas (NUEVO)
    generatePlantGeometry();

    // PASO 10: Configurar buffers OpenGL para puntos de luz
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // PASO 11: Inicializar sistemas modulares
    inputManager = std::make_unique<InputManager>(window);
    cameraSystem = std::make_unique<CameraSystem>();
    gameLogic = std::make_unique<GameLogic>();
    stateManager = std::make_unique<StateManager>();
    audioManager = std::make_unique<AudioManager>();

    return true;
}

// ============================================================================
// INICIALIZAR LUCES
// ============================================================================

void GraphicsEngine::initializeLights() {
    if (gameLogic) {
        gameLogic->reset();
    }
    // No inicializar plantas - el mapa comienza vacío
    // El usuario creará plantas con clicks del ratón
}

// ============================================================================
// GENERAR TERRENO
// ============================================================================

void GraphicsEngine::generateTerrain() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float step = TERRAIN_SIZE / TERRAIN_RESOLUTION;

    // Generar vértices del grid CON PERLIN NOISE
    for (int z = 0; z <= TERRAIN_RESOLUTION; ++z) {
        for (int x = 0; x <= TERRAIN_RESOLUTION; ++x) {
            // Posición base
            float posX = -TERRAIN_SIZE/2.0f + x * step;
            float posZ = -TERRAIN_SIZE/2.0f + z * step;
            
            // Altura usando Perlin Noise para terreno realista
            float noiseValue = perlinNoise->sample(posX, posZ, 
                TERRAIN_NOISE_SCALE, TERRAIN_NOISE_PERSISTENCE, TERRAIN_NOISE_OCTAVES);
            float posY = (noiseValue - 0.5f) * TERRAIN_NOISE_HEIGHT;  // Rangocentrado en 0
            
            vertices.push_back(posX);
            vertices.push_back(posY);
            vertices.push_back(posZ);
            
            // Normal aproximada (apunta hacia arriba con ligera variación)
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
        }
    }

    // Generar índices de los triángulos
    for (int z = 0; z < TERRAIN_RESOLUTION; ++z) {
        for (int x = 0; x < TERRAIN_RESOLUTION; ++x) {
            int a = z * (TERRAIN_RESOLUTION + 1) + x;
            int b = a + 1;
            int c = a + (TERRAIN_RESOLUTION + 1);
            int d = c + 1;

            // Primer triángulo
            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(b);

            // Segundo triángulo
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    terrainVertexCount = indices.size();

    // Crear VAO y VBO
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Atributo 0: Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

// ============================================================================
// RAYCAST: Convertir coordenadas 2D de pantalla a 3D del mundo
// ============================================================================

glm::vec3 GraphicsEngine::getRaycastHit(double mouseX, double mouseY) {
    if (!cameraSystem || !perlinNoise) {
        // Fallback: devolver posición en el centro del terreno
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // Normalizar coordenadas del mouse a -1 a 1
    float nx = (2.0f * (float)mouseX) / WINDOW_WIDTH - 1.0f;
    float ny = 1.0f - (2.0f * (float)mouseY) / WINDOW_HEIGHT;

    // Crear matrices usando CameraSystem
    glm::mat4 projection = CameraSystem::getProjectionMatrix(WINDOW_WIDTH, WINDOW_HEIGHT);
    glm::mat4 view = cameraSystem->getViewMatrix();
    
    glm::vec3 camPos = cameraSystem->getPosition();
    
    glm::mat4 invView = glm::inverse(view);
    glm::mat4 invProj = glm::inverse(projection);

    // Ray en espacio clip
    glm::vec4 rayClip = glm::vec4(nx, ny, -1.0f, 1.0f);
    
    // Ray en espacio vista
    glm::vec4 rayEye = invProj * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // Ray en espacio mundo
    glm::vec3 rayWorld = glm::vec3(invView * rayEye);
    rayWorld = glm::normalize(rayWorld);

    // Prevent division by zero
    if (glm::abs(rayWorld.y) < 0.0001f) {
        return glm::vec3(camPos.x, 0.0f, camPos.z);  // Fallback
    }

    // Raycast al plano Y=0
    float t = -camPos.y / rayWorld.y;
    
    // Prevent negative t
    if (t < 0.0f) {
        return glm::vec3(camPos.x, 0.0f, camPos.z);  // Fallback
    }
    
    glm::vec3 hitPos = camPos + rayWorld * t;

    // Clampear a límites del terreno
    hitPos.x = glm::clamp(hitPos.x, -TERRAIN_SIZE/2.0f, TERRAIN_SIZE/2.0f);
    hitPos.z = glm::clamp(hitPos.z, -TERRAIN_SIZE/2.0f, TERRAIN_SIZE/2.0f);
    
    // ✅ FIX: Calcular altura desde terreno usando Perlin Noise
    float noiseValue = perlinNoise->sample(hitPos.x, hitPos.z, 
        TERRAIN_NOISE_SCALE, TERRAIN_NOISE_PERSISTENCE, TERRAIN_NOISE_OCTAVES);
    hitPos.y = (noiseValue - 0.5f) * TERRAIN_NOISE_HEIGHT;  // Altura real del terreno

    return hitPos;
}

// ============================================================================
// AGREGAR PLANTA ALEATORIA EN UNA POSICIÓN
// ============================================================================

void GraphicsEngine::addPlant(const glm::vec3& position) {
    if (gameLogic) {
        gameLogic->addPlant(position);
    }
}

// ============================================================================
// ENTRADA Y ACTUALIZACIÓN
// ============================================================================

void GraphicsEngine::handleInput() {
    if (!inputManager || !cameraSystem || !gameLogic || !stateManager || !audioManager) {
        return;
    }

    // Actualizar estado de inputs
    inputManager->update();
    const auto& inputState = inputManager->getState();

    // SIEMPRE procesable: scroll wheel para zoom (en cualquier estado)
    if (inputState.scrollY != 0.0) {
        float zoomDelta = inputState.scrollY * 2.0f;  // 2 unidades por scroll
        cameraSystem->adjustDistance(-zoomDelta);  // Negativo porque scroll up = zoom in
    }

    // Manejar ESC - transición a menú
    if (inputState.keyEscape) {
        if (stateManager->isInState(PLAYING)) {
            stateManager->requestTransition(MENU);
            cameraSystem->reset();
        } else if (stateManager->isInState(SETTINGS)) {
            stateManager->requestTransition(MENU);
        }
    }

    // Controles solo en estado PLAYING
    if (stateManager->isInState(PLAYING)) {
        // ✅ Rotación de cámara con botón derecho del ratón (drag)
        if (inputState.mouseRightPressed && (inputState.mouseDeltaX != 0.0 || inputState.mouseDeltaY != 0.0)) {
            float rotationSpeed = 0.5f;  // Rotación horizontal
            float zoomSpeed = 0.01f;     // Zoom vertical
            
            // Mouse X → rotación horizontal
            cameraSystem->rotate(inputState.mouseDeltaX * rotationSpeed);
            
            // Mouse Y → ajustar distancia (zoom con drag)
            cameraSystem->adjustDistance(-inputState.mouseDeltaY * zoomSpeed);
        }

        // Movimiento de cámara con FLECHAS (lento, sin rotación)
        if (inputState.keyUp) {
            cameraSystem->panTarget(0.0f, -0.3f);
        }
        if (inputState.keyDown) {
            cameraSystem->panTarget(0.0f, 0.3f);
        }
        if (inputState.keyLeft) {
            cameraSystem->panTarget(-0.3f, 0.0f);
        }
        if (inputState.keyRight) {
            cameraSystem->panTarget(0.3f, 0.0f);
        }

        // Movimiento WASD (RÁPIDO, ROTADO según rotación de cámara)
        glm::vec2 moveInput = glm::vec2(0.0f, 0.0f);
        
        if (inputState.keyW) {
            moveInput.y -= 1.0f;  // Adelante
        }
        if (inputState.keyS) {
            moveInput.y += 1.0f;  // Atrás
        }
        if (inputState.keyA) {
            moveInput.x -= 1.0f;  // Izquierda
        }
        if (inputState.keyD) {
            moveInput.x += 1.0f;  // Derecha
        }

        if (glm::length(moveInput) > 0.0f) {
            float moveSpeed = CAMERA_MOVEMENT_SPEED * 0.016f;
            // ✅ FIX: Invertir moveInput.x para corregir dirección A/D
            cameraSystem->panTargetRotated(moveInput.y * moveSpeed, -moveInput.x * moveSpeed);
        }

        // LEFT CLICK - Agregar planta
        if (inputState.mouseLeftClick) {
            glm::vec3 hitPos = getRaycastHit(inputState.mouseX, inputState.mouseY);
            // ✅ FIX: Permitir plantas sin límite de MAX_LIGHTS
            addPlant(hitPos);
            audioManager->playSound(AudioManager::SOUND_PLACE_PLANT);
        }

        // RIGHT CLICK - Eliminar planta más cercana
        if (inputState.mouseRightClick) {
            glm::vec3 hitPos = getRaycastHit(inputState.mouseX, inputState.mouseY);
            if (gameLogic->removeNearestPlant(hitPos, 3.0f)) {
                audioManager->playSound(AudioManager::SOUND_DELETE_PLANT);
            }
        }
    }
}

void GraphicsEngine::update(float deltaTime) {
    if (!stateManager || !gameLogic) {
        return;
    }

    // Actualizar tiempo transcurrido
    elapsedTime += deltaTime;

    // Actualizar estado
    stateManager->update();

    // Actualizar lógica del juego
    gameLogic->update(deltaTime);
}

// ============================================================================
// RENDERIZACIÓN
// ============================================================================

void GraphicsEngine::render() {
    if (!stateManager) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GameState state = stateManager->getCurrentState();
    
    if (state == SPLASH) {
        renderSplashScreen();
    } else if (state == MENU) {
        renderMenu();
    } else if (state == PLAYING) {
        renderGameScene();
    } else if (state == SETTINGS) {
        renderSettings();
    } else if (state == CREDITS) {
        renderCredits();
    }

    glfwSwapBuffers(window);
}

void GraphicsEngine::renderSplashScreen() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.9f);

    ImGui::Begin("##Splash", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

    ImGui::Spacing();
    ImGui::Spacing();
    
    ImVec2 textSize = ImGui::CalcTextSize("Made by");
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
    ImGui::Text("Made by");
    
    textSize = ImGui::CalcTextSize("Fabian");
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Fabian");

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsEngine::renderMenu() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);

    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImVec2 textSize = ImGui::CalcTextSize("3D Graphics Engine");
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
    ImGui::Text("3D Graphics Engine");
    
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Play", ImVec2(250, 50))) {
        if (stateManager) {
            stateManager->requestTransition(PLAYING);
            initializeLights();
            cameraSystem->reset();
        }
    }

    ImGui::Spacing();

    if (ImGui::Button("Settings", ImVec2(250, 50))) {
        if (stateManager) {
            stateManager->requestTransition(SETTINGS);
        }
    }

    ImGui::Spacing();

    if (ImGui::Button("Credits", ImVec2(250, 50))) {
        if (stateManager) {
            stateManager->requestTransition(CREDITS);
        }
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsEngine::renderGameScene() {
    if (!cameraSystem || !gameLogic) {
        return;
    }

    // Obtener matrices de CameraSystem
    glm::mat4 projection = CameraSystem::getProjectionMatrix(WINDOW_WIDTH, WINDOW_HEIGHT);
    glm::mat4 view = cameraSystem->getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    // ===== RENDERIZAR TERRENO =====
    glUseProgram(terrainShaderProgram);
    
    GLint projLoc = glGetUniformLocation(terrainShaderProgram, "uProjection");
    GLint viewLoc = glGetUniformLocation(terrainShaderProgram, "uView");
    GLint modelLoc = glGetUniformLocation(terrainShaderProgram, "uModel");
    GLint timeLoc = glGetUniformLocation(terrainShaderProgram, "uTime");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(timeLoc, elapsedTime);

    // Pasar posiciones y colores de plantas al shader del terreno
    const auto& plants = gameLogic->getPlants();
    int plantCount = (int)plants.size();
    GLint plantCountLoc = glGetUniformLocation(terrainShaderProgram, "uPlantCount");
    glUniform1i(plantCountLoc, plantCount);

    for (int i = 0; i < plantCount && i < MAX_LIGHTS; ++i) {
        std::string posName = "uPlantPositions[" + std::to_string(i) + "]";
        std::string colName = "uPlantColors[" + std::to_string(i) + "]";
        
        glm::vec3 color;
        if (plants[i].type == GRASS) color = PLANT_COLOR_GRASS;
        else if (plants[i].type == BUSH) color = PLANT_COLOR_BUSH;
        else color = PLANT_COLOR_TREE;
        
        glUniform3fv(glGetUniformLocation(terrainShaderProgram, posName.c_str()), 1, glm::value_ptr(plants[i].position));
        glUniform3fv(glGetUniformLocation(terrainShaderProgram, colName.c_str()), 1, glm::value_ptr(color));
    }

    glBindVertexArray(terrainVAO);
    glDrawElements(GL_TRIANGLES, terrainVertexCount, GL_UNSIGNED_INT, 0);

    // ===== RENDERIZAR PLANTAS (PUNTOS DE LUZ) =====
    glUseProgram(shaderProgram);

    projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    viewLoc = glGetUniformLocation(shaderProgram, "uView");
    modelLoc = glGetUniformLocation(shaderProgram, "uModel");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    renderPlants();

    // ===== UI INFORMACIÓN =====
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Game Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("ESC: Back to Menu");
    
    // Contar plantas por tipo
    int grassCount = 0, bushCount = 0, treeCount = 0;
    gameLogic->getPlantCounts(grassCount, bushCount, treeCount);
    
    ImGui::Text("Plants: %zu / %d", gameLogic->getPlantCount(), MAX_LIGHTS);
    ImGui::Text("  Grass: %d", grassCount);
    ImGui::Text("  Bush: %d", bushCount);
    ImGui::Text("  Tree: %d", treeCount);
    ImGui::Separator();
    ImGui::Text("Q/E: Rotate | WASD: Move");
    ImGui::Text("Arrows: Slow Move | Click: Plant");
    ImGui::Separator();
    glm::vec2 camXZ = cameraSystem->getTargetXZ();
    ImGui::Text("Camera Pos: %.1f, %.1f", camXZ.x, camXZ.y);
    ImGui::Text("Rotation: %.1f degrees", cameraSystem->getRotation());
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsEngine::renderTerrain() {
    // Esta función es solo un placeholder para mantener la estructura
    // El terreno se renderiza en renderGameScene()
}

void GraphicsEngine::renderSettings() {
    if (!stateManager || !gameLogic) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 450), ImGuiCond_Always);

    ImGui::Begin("Configuración", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 1.0f);
    if (audioManager) {
        audioManager->setMasterVolume(masterVolume);
    }
    
    ImGui::Separator();
    ImGui::Text("Configuracion de Plantas:");
    ImGui::Separator();
    
    // Probabilidades
    float treeProb = gameLogic->getTreeProbability();
    float bushProb = gameLogic->getBushProbability();
    
    ImGui::SliderFloat("Probabilidad Arboles (%%)", &treeProb, 0.0f, 0.5f, "%.2f");
    gameLogic->setTreeProbability(treeProb);
    
    ImGui::SliderFloat("Probabilidad Arbustos (%%)", &bushProb, 0.0f, 0.5f, "%.2f");
    gameLogic->setBushProbability(bushProb);
    
    ImGui::Text("Probabilidad Hierba: %.2f", gameLogic->getGrassProbability());
    
    ImGui::Separator();
    ImGui::Text("Tamanios de Punto:");
    ImGui::Separator();
    
    // Tamaños
    ImGui::SliderFloat("Hierba (px)", &plantSizeGrass, 1.0f, 15.0f);
    ImGui::SliderFloat("Arbusto (px)", &plantSizeBush, 1.0f, 15.0f);
    ImGui::SliderFloat("Arbol (px)", &plantSizeTree, 1.0f, 20.0f);
    
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Atras", ImVec2(350, 50))) {
        stateManager->requestTransition(MENU);
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsEngine::renderCredits() {
    if (!stateManager) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);

    ImGui::Begin("Credits", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImVec2 textSize = ImGui::CalcTextSize("Credits");
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
    ImGui::Text("Credits");
    
    ImGui::Spacing();

    ImGui::Text("Developer: Fabian");
    ImGui::Text("Graphics: OpenGL");
    ImGui::Text("UI: ImGui");
    ImGui::Text("Math: GLM");
    ImGui::Text("Window: GLFW");

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(350, 50))) {
        stateManager->requestTransition(MENU);
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ============================================================================
// GENERAR GEOMETRÍA DE PLANTAS (PROCEDURAL)
// ============================================================================

void GraphicsEngine::generatePlantGeometry() {
    // Ya no se usa - Las plantas se renderizan como puntos simples
}

void GraphicsEngine::renderPlants() {
    if (!gameLogic) {
        return;
    }

    glUseProgram(shaderProgram);
    
    // Preparar datos de vértices para todos los puntos
    std::vector<float> vertices;
    const auto& plants = gameLogic->getPlants();
    float gameTime = gameLogic->getElapsedTime();
    
    for (const auto& plant : plants) {
        // Calcular color con animación flash al crear
        glm::vec3 color;
        float timeSinceCreation = gameTime - plant.createdTime;
        
        // Determinar color base según tipo
        if (plant.type == GRASS) {
            color = PLANT_COLOR_GRASS;
        } else if (plant.type == BUSH) {
            color = PLANT_COLOR_BUSH;
        } else {
            color = PLANT_COLOR_TREE;
        }
        
        // Flash blanco al create (primeros 0.5 segundos)
        if (timeSinceCreation < PLANT_CREATION_FLASH_DURATION) {
            float flashIntensity = 1.0f - (timeSinceCreation / PLANT_CREATION_FLASH_DURATION);
            color = glm::mix(color, glm::vec3(1.0f, 1.0f, 1.0f), flashIntensity * 0.7f);
        }
        
        // Posición
        vertices.push_back(plant.position.x);
        vertices.push_back(plant.position.y);
        vertices.push_back(plant.position.z);
        
        // Color
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        
        // Tamaño del punto (codificado en el vértice) - CONFIGURABLE
        float pointSize;
        if (plant.type == GRASS) {
            pointSize = plantSizeGrass;
        } else if (plant.type == BUSH) {
            pointSize = plantSizeBush;
        } else {
            pointSize = plantSizeTree;
        }
        vertices.push_back(pointSize);
    }
    
    // Inicializar VAO/VBO si no existen
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Cargar datos
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    // Configurar atributos
    // Posición (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Tamaño (1 float)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Renderizar puntos
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);  // Tamaño base (se modifica en vertex shader)
    glDrawArrays(GL_POINTS, 0, plants.size());
    glDisable(GL_PROGRAM_POINT_SIZE);
}

// ============================================================================
// LIMPIEZA Y ESTADO
// ============================================================================

void GraphicsEngine::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
    glDeleteProgram(terrainShaderProgram);
    
    // ✅ FIX BUG-2: Limpiar módulos (unique_ptr se destruye automáticamente)
    inputManager.reset();
    cameraSystem.reset();
    gameLogic.reset();
    stateManager.reset();
    audioManager.reset();
    
    // Limpiar Perlin Noise
    if (perlinNoise) delete perlinNoise;

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool GraphicsEngine::isRunning() {
    return !glfwWindowShouldClose(window);
}
