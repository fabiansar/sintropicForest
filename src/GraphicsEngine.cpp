#include "GraphicsEngine.h"
#include "Shaders.h"
#include "Config.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
    : currentState(SPLASH), nextState(SPLASH), window(nullptr), 
      shaderProgram(0), terrainShaderProgram(0), VAO(0), VBO(0),
      terrainVAO(0), terrainVBO(0), terrainEBO(0), terrainVertexCount(0),
      masterVolume(0.8f),
      cameraPos(0.0f, CAMERA_HEIGHT, 0.0f),
      cameraTarget(0.0f, 0.0f, 0.0f),
      cameraRotation(0.0f),
      leftMousePressed(false),
      lastMouseX(0.0), lastMouseY(0.0),
      elapsedTime(0.0f),
      perlinNoise(nullptr),
      projectionDirty(true),
      viewDirty(true) {
    splashStartTime = std::chrono::steady_clock::now();
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

    return true;
}

// ============================================================================
// INICIALIZAR LUCES
// ============================================================================

void GraphicsEngine::initializeLights() {
    plants.clear();
    
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
    // Normalizar coordenadas del mouse a -1 a 1
    float nx = (2.0f * mouseX) / WINDOW_WIDTH - 1.0f;
    float ny = 1.0f - (2.0f * mouseY) / WINDOW_HEIGHT;

    // Crear matrices
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    
    glm::vec3 camPos = cameraTarget + glm::vec3(
        cos(glm::radians(cameraRotation)) * CAMERA_DISTANCE,
        CAMERA_HEIGHT,
        sin(glm::radians(cameraRotation)) * CAMERA_DISTANCE
    );
    
    glm::mat4 view = glm::lookAt(camPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
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

    // Raycast al plano Y=0
    float t = -camPos.y / rayWorld.y;
    glm::vec3 hitPos = camPos + rayWorld * t;

    // Clampear a límites del terreno
    hitPos.x = glm::clamp(hitPos.x, -TERRAIN_SIZE/2.0f, TERRAIN_SIZE/2.0f);
    hitPos.z = glm::clamp(hitPos.z, -TERRAIN_SIZE/2.0f, TERRAIN_SIZE/2.0f);
    hitPos.y = 2.0f;  // Altura de las luces

    return hitPos;
}

// ============================================================================
// AGREGAR LUZ ALEATORIA EN UNA POSICIÓN
// ============================================================================

void GraphicsEngine::addPlant(const glm::vec3& position) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist_rand(0.0f, 1.0f);
    std::uniform_real_distribution<float> dist_scale(0.8f, 1.3f);  // Escala procedural
    std::uniform_real_distribution<float> dist_rotation(0.0f, glm::two_pi<float>());  // Rotación

    Plant newPlant;
    newPlant.position = position;
    newPlant.velocity = glm::vec3(0.0f, 0.0f, 0.0f);  // Estática
    newPlant.scale = dist_scale(gen);  // Variedad procedural
    newPlant.rotation = dist_rotation(gen);  // Rotación random

    // Determinar tipo de planta
    float rand = dist_rand(gen);
    if (rand < PLANT_PROBABILITY_TREE) {
        newPlant.type = TREE;
        newPlant.color = glm::vec3(0.1f, 0.5f, 0.05f);  // Verde oscuro para árbol
    } else if (rand < PLANT_PROBABILITY_TREE + PLANT_PROBABILITY_BUSH) {
        newPlant.type = BUSH;
        newPlant.color = glm::vec3(0.2f, 0.7f, 0.1f);  // Verde medio para arbusto
    } else {
        newPlant.type = GRASS;
        newPlant.color = glm::vec3(0.3f, 0.8f, 0.2f);   // Verde claro para hierba
    }

    plants.push_back(newPlant);
}

// ============================================================================
// ENTRADA Y ACTUALIZACIÓN
// ============================================================================

void GraphicsEngine::handleInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (currentState == PLAYING) {
            nextState = MENU;
        } else {
            glfwSetWindowShouldClose(window, true);
        }
    }

    if (currentState == PLAYING) {
        // Rotación de cámara con Q y E (ahora más rápida)
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            cameraRotation += CAMERA_ROTATION_SPEED * 0.016f;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            cameraRotation -= CAMERA_ROTATION_SPEED * 0.016f;
        }

        // Movimiento de cámara con FLECHAS (lento, sin rotación)
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cameraTarget.z -= 0.3f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraTarget.z += 0.3f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            cameraTarget.x -= 0.3f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            cameraTarget.x += 0.3f;
        }

        // Movimiento WASD (RÁPIDO, ROTADO según la cámara)
        glm::vec2 moveInput = glm::vec2(0.0f, 0.0f);
        float moveSpeed = CAMERA_MOVEMENT_SPEED * 0.016f;
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            moveInput += glm::vec2(0.0f, -1.0f);  // Adelante
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            moveInput += glm::vec2(0.0f, 1.0f);   // Atrás
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            moveInput += glm::vec2(-1.0f, 0.0f);  // Izquierda
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            moveInput += glm::vec2(1.0f, 0.0f);   // Derecha
        }

        // Rotar el movimiento según la rotación de la cámara
        if (glm::length(moveInput) > 0.0f) {
            float angle = glm::radians(cameraRotation);
            float cosA = cos(angle);
            float sinA = sin(angle);
            
            float x = moveInput.x * cosA - moveInput.y * sinA;
            float z = moveInput.x * sinA + moveInput.y * cosA;
            
            cameraTarget.x += x * moveSpeed;
            cameraTarget.z += z * moveSpeed;
        }

        // Detectar clicks del ratón izquierdo
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS && !leftMousePressed) {
            leftMousePressed = true;
            
            // Obtener posición del ratón y hacer raycast
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            glm::vec3 hitPos = getRaycastHit(mouseX, mouseY);
            
            // Agregar planta en esa posición
            if (plants.size() < MAX_LIGHTS) {
                addPlant(hitPos);
            }
        } else if (state == GLFW_RELEASE) {
            leftMousePressed = false;
        }
    }
}

void GraphicsEngine::update(float deltaTime) {
    // Actualizar tiempo transcurrido
    elapsedTime += deltaTime;

    // Actualizar posición de plantas (aunque no se muevan, mantenemos la lógica)
    for (auto& plant : plants) {
        plant.position += plant.velocity;

        // Mantener en el terreno
        const float boundX = TERRAIN_SIZE/2.0f;
        const float boundZ = TERRAIN_SIZE/2.0f;
        
        // Rebotar en los límites
        if (plant.position.x < -boundX || plant.position.x > boundX) {
            plant.velocity.x *= -1.0f;
            plant.position.x = glm::clamp(plant.position.x, -boundX, boundX);
        }
        if (plant.position.z < -boundZ || plant.position.z > boundZ) {
            plant.velocity.z *= -1.0f;
            plant.position.z = glm::clamp(plant.position.z, -boundZ, boundZ);
        }
    }

    // Gestionar splash screen
    if (currentState == SPLASH) {
        auto elapsed = std::chrono::steady_clock::now() - splashStartTime;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 3) {
            currentState = MENU;
            nextState = MENU;
        }
    } else {
        currentState = nextState;
    }
}

// ============================================================================
// RENDERIZACIÓN
// ============================================================================

void GraphicsEngine::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentState == SPLASH) {
        renderSplashScreen();
    } else if (currentState == MENU) {
        renderMenu();
    } else if (currentState == PLAYING) {
        renderGameScene();
    } else if (currentState == SETTINGS) {
        renderSettings();
    } else if (currentState == CREDITS) {
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
        nextState = PLAYING;
        initializeLights();
        // Resetear cámara
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        cameraRotation = 0.0f;
    }

    ImGui::Spacing();

    if (ImGui::Button("Settings", ImVec2(250, 50))) {
        nextState = SETTINGS;
    }

    ImGui::Spacing();

    if (ImGui::Button("Credits", ImVec2(250, 50))) {
        nextState = CREDITS;
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsEngine::renderGameScene() {
    // CÁMARA TIPO JUEGO DE GESTIÓN/ESTRATEGIA
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
        (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    
    // Calcular posición de cámara en órbita isométrica
    glm::vec3 camPos = cameraTarget + glm::vec3(
        cos(glm::radians(cameraRotation)) * CAMERA_DISTANCE,
        CAMERA_HEIGHT,
        sin(glm::radians(cameraRotation)) * CAMERA_DISTANCE
    );
    
    glm::mat4 view = glm::lookAt(camPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
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
    glUniform1f(timeLoc, elapsedTime);  // Pasar tiempo para animación

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

    // ===== RENDERIZAR PLANTAS CON GEOMETRÍA PROCEDURAL (NUEVO) =====
    renderPlants();

    // ===== UI INFORMACIÓN =====
    renderTerrain();  // Esta función solo maneja ImGui

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Game Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("ESC: Back to Menu");
    
    // Contar plantas por tipo
    int grassCount = 0, bushCount = 0, treeCount = 0;
    for (const auto& plant : plants) {
        if (plant.type == GRASS) grassCount++;
        else if (plant.type == BUSH) bushCount++;
        else if (plant.type == TREE) treeCount++;
    }
    
    ImGui::Text("Plants: %d / %d", (int)plants.size(), MAX_LIGHTS);
    ImGui::Text("  Grass: %d (80%%)", grassCount);
    ImGui::Text("  Bush: %d (15%%)", bushCount);
    ImGui::Text("  Tree: %d (5%%)", treeCount);
    ImGui::Separator();
    ImGui::Text("Q/E: Rotate | WASD: Move");
    ImGui::Text("Arrows: Slow Move | Click: Plant");
    ImGui::Separator();
    ImGui::Text("Camera Pos: %.1f, %.1f", cameraTarget.x, cameraTarget.z);
    ImGui::Text("Rotation: %.1f degrees", cameraRotation);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsEngine::renderTerrain() {
    // Esta función es solo un placeholder para mantener la estructura
    // El terreno se renderiza en renderGameScene()
}

void GraphicsEngine::renderSettings() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(350, 250), ImGuiCond_Always);

    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 1.0f);
    
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Back", ImVec2(300, 50))) {
        nextState = MENU;
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsEngine::renderCredits() {
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
        nextState = MENU;
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ============================================================================
// GENERAR GEOMETRÍA DE PLANTAS (PROCEDURAL)
// ============================================================================

void GraphicsEngine::generatePlantGeometry() {
    // Generar geometría para cada tipo de planta y cachearla
    
    // GRASS - Pequeno cilindro
    auto grassMesh = PlantGeometry::GenerateGrass(0.8f);
    unsigned int grassVAO, grassVBO;
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, grassMesh.vertices.size() * sizeof(float), grassMesh.vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    plantVAOs[GRASS] = grassVAO;
    plantVBOs[GRASS] = grassVBO;
    plantIndexCounts[GRASS] = grassMesh.vertices.size() / 6;
    
    // BUSH - Esfera
    auto bushMesh = PlantGeometry::GenerateBush(0.6f);
    unsigned int bushVAO, bushVBO;
    glGenVertexArrays(1, &bushVAO);
    glGenBuffers(1, &bushVBO);
    glBindVertexArray(bushVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bushVBO);
    glBufferData(GL_ARRAY_BUFFER, bushMesh.vertices.size() * sizeof(float), bushMesh.vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    plantVAOs[BUSH] = bushVAO;
    plantVBOs[BUSH] = bushVBO;
    plantIndexCounts[BUSH] = bushMesh.vertices.size() / 6;
    
    // TREE - Cono + Cilindro
    auto treeMesh = PlantGeometry::GenerateTree(2.5f);
    unsigned int treeVAO, treeVBO;
    glGenVertexArrays(1, &treeVAO);
    glGenBuffers(1, &treeVBO);
    glBindVertexArray(treeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
    glBufferData(GL_ARRAY_BUFFER, treeMesh.vertices.size() * sizeof(float), treeMesh.vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    plantVAOs[TREE] = treeVAO;
    plantVBOs[TREE] = treeVBO;
    plantIndexCounts[TREE] = treeMesh.vertices.size() / 6;
}

void GraphicsEngine::renderPlants() {
    glUseProgram(shaderProgram);
    
    // Matrices cacheadas (OPTIMIZACIÓN)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    
    glm::vec3 camPos = cameraTarget + glm::vec3(
        cos(glm::radians(cameraRotation)) * CAMERA_DISTANCE,
        CAMERA_HEIGHT,
        sin(glm::radians(cameraRotation)) * CAMERA_DISTANCE
    );
    
    glm::mat4 view = glm::lookAt(camPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    
    // Renderizar cada planta con su geometría procedural
    for (const auto& plant : plants) {
        if (plantVAOs.find(plant.type) == plantVAOs.end()) continue;
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, plant.position);
        model = glm::rotate(model, plant.rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(plant.scale));
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(plantVAOs[plant.type]);
        glDrawArrays(GL_TRIANGLES, 0, plantIndexCounts[plant.type]);
    }
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
    
    // Limpiar geometría de plantas
    for (auto& pair : plantVAOs) {
        glDeleteVertexArrays(1, &pair.second);
    }
    for (auto& pair : plantVBOs) {
        glDeleteBuffers(1, &pair.second);
    }
    
    // Limpiar Perlin Noise
    if (perlinNoise) delete perlinNoise;

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool GraphicsEngine::isRunning() {
    return !glfwWindowShouldClose(window);
}
