#pragma once

#include <vector>
#include <chrono>
#include <map>
#include "Config.h"
#include "PerlinNoise.h"

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
    // VARIABLES DE ESTADO
    GameState currentState;
    GameState nextState;
    
    // VARIABLES GRÁFICAS
    GLFWwindow* window;
    unsigned int shaderProgram;      // Programa para puntos de luz
    unsigned int terrainShaderProgram; // Programa para el terreno
    unsigned int VAO, VBO;           // Para puntos de luz
    unsigned int terrainVAO, terrainVBO, terrainEBO; // Para terreno
    std::vector<Light> lights;
    
    // VARIABLES DEL TERRENO
    int terrainVertexCount;
    
    // VARIABLES DE TIMPIZACIÓN
    std::chrono::steady_clock::time_point splashStartTime;
    
    // VARIABLES DE CONFIGURACIÓN
    float masterVolume;
    
    // VARIABLES DE CÁMARA (Vista de Estrategia/Gestión)
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;      // Centro del mapa que está mirando
    float cameraRotation;        // Rotación alrededor del eje Y
    bool leftMousePressed;       // Flag para detectar clicks
    double lastMouseX, lastMouseY;
    
    // VARIABLE DE TIEMPO PARA ANIMACIÓN DEL TERRENO
    float elapsedTime;           // Tiempo total transcurrido en segundos
    
    // PERLIN NOISE PARA TERRENO PROCEDURAL
    PerlinNoise* perlinNoise;
    
    // CACHED MATRICES PARA OPTIMIZACIÓN
    glm::mat4 cachedProjection;
    glm::mat4 cachedView;
    glm::mat4 cachedModel;
    bool projectionDirty;
    bool viewDirty;
    
    // PLANT GEOMETRY MESHES (CACHEADAS)
    std::map<int, unsigned int> plantVAOs;      // VAO por tipo de planta
    std::map<int, unsigned int> plantVBOs;      // VBO por tipo de planta
    std::map<int, unsigned int> plantIndexCounts;  // Índices por tipo
    
    // MÉTODOS PRIVADOS
    void generateTerrain();
    void generatePlantGeometry();  // NEW: Generar geometría procedural de plantas
    glm::vec3 getRaycastHit(double mouseX, double mouseY);
    void renderTerrain();
    void renderPlants();  // NEW: Renderizar plantas con geometría
    void addRandomLight(const glm::vec3& position);
    
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
