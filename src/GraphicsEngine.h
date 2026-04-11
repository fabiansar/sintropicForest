#pragma once

#include <vector>
#include <memory>
#include "Config.h"
#include "PerlinNoise.h"
#include "InputManager.h"
#include "CameraSystem.h"
#include "GameLogic.h"
#include "StateManager.h"
#include "AudioManager.h"

// Forward declaration para evitar incluir GLFW aquí
struct GLFWwindow;

/**
 * CLASE MOTOR GRÁFICO
 * 
 * Esta es la clase principal que controla toda la aplicación.
 * Maneja:
 * - La ventana y contexto OpenGL
 * - Los estados de la aplicación
 * - El renderizado de cada pantalla
 * - La lógica de actualización
 * - Los puntos de luz dinámicos
*/
class GraphicsEngine {
private:
    // SISTEMAS MODULARES (delegados)
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<CameraSystem> cameraSystem;
    std::unique_ptr<GameLogic> gameLogic;
    std::unique_ptr<StateManager> stateManager;
    std::unique_ptr<AudioManager> audioManager;
    
    // VARIABLES GRÁFICAS
    GLFWwindow* window;
    unsigned int shaderProgram;      // Programa para renderizar plantas
    unsigned int terrainShaderProgram; // Programa para el terreno
    unsigned int VAO, VBO;           // Para plantas
    unsigned int terrainVAO, terrainVBO, terrainEBO; // Para terreno
    
    // ✅ OPTIMIZACIÓN: Pre-allocate vertex buffer
    std::vector<float> vertexBuffer;  // Buffer reutilizable para cada frame
    
    // VARIABLES DEL TERRENO
    int terrainVertexCount;
    
    // VARIABLES DE CONFIGURACIÓN
    float masterVolume;
    
    // CONFIGURACIÓN DE PLANTAS (EDITABLE EN SETTINGS)
    float plantProbabilityTree = PLANT_PROBABILITY_TREE;
    float plantProbabilityBush = PLANT_PROBABILITY_BUSH;
    float plantSizeGrass = PLANT_SIZE_GRASS;
    float plantSizeBush = PLANT_SIZE_BUSH;
    float plantSizeTree = PLANT_SIZE_TREE;
    
    // VARIABLE DE TIEMPO PARA ANIMACIÓN DEL TERRENO
    float elapsedTime;           // Tiempo total transcurrido en segundos
    
    // PERLIN NOISE PARA TERRENO PROCEDURAL
    PerlinNoise* perlinNoise;
    
    // MÉTODOS PRIVADOS
    void generateTerrain();
    void generatePlantGeometry();  // NEW: Generar geometría procedural de plantas
    glm::vec3 getRaycastHit(double mouseX, double mouseY);
    void renderTerrain();
    void renderPlants();  // Renderizar plantas con geometría procedural
    void addPlant(const glm::vec3& position);  // Agregar nueva planta en posición
    
    // MÉTODOS PRIVADOS DE RENDERIZADO
    void renderSplashScreen();
    void renderMenu();
    void renderGameScene();
    void renderSettings();
    void renderCredits();

public:
    GraphicsEngine();
    ~GraphicsEngine() = default;

    // Métodos principales
    bool initialize();
    void update(float deltaTime);
    void render();
    void handleInput();
    void cleanup();
    bool isRunning();

    // Métodos de utilidad
    void initializeLights();
};
