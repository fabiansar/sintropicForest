/**
 * Motor Gráfico 3D - Aplicación OpenGL con Sistema de Menús
 * 
 * Punto de entrada principal del programa
 * 
 * Estructura del proyecto:
 * - main.cpp:         Función principal y loop del juego
 * - GraphicsEngine.*: Clase principal que controla la aplicación
 * - Config.h:         Configuración y estructuras de datos
 * - Shaders.h:        Código GLSL de los shaders
 * 
 * Tecnologías utilizadas:
 * - OpenGL 3.3+ para gráficos
 * - GLFW para gestión de ventanas
 * - ImGui para interfaz de usuario
 * - GLM para operaciones matemáticas
*/

#include "GraphicsEngine.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

/**
 * FUNCIÓN PRINCIPAL - PUNTO DE ENTRADA DEL PROGRAMA
 * 
 * Flujo principal:
 * 1. Crear instancia del motor gráfico
 * 2. Inicializar todos los recursos
 * 3. LOOP INFINITO:
 *    - Calcular delta time
 *    - Procesar entrada
 *    - Actualizar lógica
 *    - Renderizar
 *    - Procesar eventos
 * 4. Limpiar recursos
 * 
 * @return 0 si todo fue ok, -1 si hay error
*/
int main() {
    // Crear instancia del motor gráfico
    GraphicsEngine engine;

    // Inicializar: ventana, OpenGL, ImGui, shaders
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize graphics engine" << std::endl;
        return -1;
    }

    // Registrar la hora actual para calcular delta time
    auto lastTime = std::chrono::high_resolution_clock::now();

    // LOOP PRINCIPAL
    while (engine.isRunning()) {
        // Calcular tiempo transcurrido
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Procesar entrada
        engine.handleInput();
        
        // Actualizar
        engine.update(deltaTime);
        
        // Renderizar
        engine.render();

        // Procesar eventos
        glfwPollEvents();
    }

    // Limpiar recursos
    engine.cleanup();
    
    return 0;
}
