/**
 * Motor Gráfico 3D - Aplicación OpenGL con Sistema de Menús
 * 
 * Este programa crea una aplicación interactiva con:
 * - Pantalla de inicio (splash screen)
 * - Menú principal con 3 opciones
 * - Escena 3D con 75 puntos de luz dinámicos
 * - Pantallas de configuración y créditos
 * 
 * Tecnologías utilizadas:
 * - OpenGL 3.3+ para gráficos
 * - GLFW para gestión de ventanas
 * - ImGui para interfaz de usuario
 * - GLM para operaciones matemáticas
*/

#include <glad/glad.h>          // Cargador de funciones OpenGL
#include <GLFW/glfw3.h>         // Gestión de ventanas y entrada
#include <glm/glm.hpp>          // Librería de matemáticas vectoriales
#include <glm/gtc/matrix_transform.hpp>  // Transformaciones de matrices
#include <glm/gtc/type_ptr.hpp> // Conversión de tipos GLM a punteros
#include <imgui.h>              // Framework UI ImGui
#include <imgui_impl_glfw.h>    // Binding de ImGui para GLFW
#include <imgui_impl_opengl3.h> // Binding de ImGui para OpenGL3
#include <iostream>             // Entrada/salida estándar
#include <vector>               // Contenedor dinámico
#include <random>               // Generación de números aleatorios
#include <cmath>                // Funciones matemáticas
#include <chrono>               // Cronometraje del programa

#include "CameraSystem.h"       // Sistema de cámara orbital
#include "InputManager.h"       // Sistema de entrada de usuario

// CONSTANTES DE CONFIGURACIÓN
const int WINDOW_WIDTH = 1280;   // Ancho de la ventana en píxeles
const int WINDOW_HEIGHT = 720;   // Alto de la ventana en píxeles
const int NUM_LIGHTS = 75;       // Cantidad de puntos de luz a renderizar

/**
 * ESTADOS DEL JUEGO
 * 
 * Define los diferentes estados en que puede estar la aplicación.
 * El programa cambia entre estos estados basado en las acciones del usuario.
*/
enum GameState {
    SPLASH,    // Pantalla inicial "Made by Fabian" - dura 3 segundos
    MENU,      // Menú principal con 3 botones
    PLAYING,   // Escena 3D con puntos de luz animados
    SETTINGS,  // Pantalla de configuración (volumen, etc.)
    CREDITS    // Pantalla de créditos
};

/**
 * VERTEX SHADER (Sombreador de Vértices)
 * 
 * Este shader se ejecuta para cada vértice que enviamos a OpenGL.
 * Transforma la posición 3D del punto de luz usando las matrices de proyección, vista y modelo.
 * También pasa el color a través del proceso de renderizado.
*/
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    vertexColor = aColor;
}
)";

/**
 * FRAGMENT SHADER (Sombreador de Fragmentos/Píxeles)
 * 
 * Este shader determina el color final de cada píxel.
 * Crea un efecto de brillo (glow) para los puntos de luz
 * usando la distancia del píxel respecto al centro del punto.
*/
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    float distance = length(gl_PointCoord - vec2(0.5));
    if (distance > 0.5)
        discard;
    
    float alpha = 1.0 - (distance * 2.0);
    FragColor = vec4(vertexColor, alpha);
}
)";

/**
 * ESTRUCTURA DE DATO: PUNTO DE LUZ
 * 
 * Representa un punto de luz individual en la escena 3D.
 * Cada punto tiene:
 * - Posición: coordenadas X, Y, Z en el espacio 3D
 * - Color: componentes RGB (Rojo, Verde, Azul) entre 0.0 y 1.0
 * - Velocidad: vector de movimiento que se suma a la posición cada fotograma
*/
struct Light {
    glm::vec3 position;  // Posición actual en el espacio 3D (X, Y, Z)
    glm::vec3 color;     // Color RGB del punto (R, G, B) entre 0.0-1.0
    glm::vec3 velocity;  // Vector de velocidad (cambio por fotograma)
};

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
 * 
 * @param source - String conteniendo el código GLSL del shader
 * @param type   - Tipo de shader: GL_VERTEX_SHADER o GL_FRAGMENT_SHADER
 * @return       - ID del shader compilado (o con errores si fall)
*/
unsigned int compileShader(const char* source, GLenum type) {
    unsigned int shader = glCreateShader(type);  // Crear nuevo shader
    glShaderSource(shader, 1, &source, nullptr); // Cargar código fuente
    glCompileShader(shader);                     // Compilar

    // Verificar errores de compilación
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

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
    GameState currentState;   // Estado actual en que se encuentra la app
    GameState nextState;      // Siguiente estado a usar
    
    // VARIABLES GRÁFICAS
    GLFWwindow* window;       // Puntero a la ventana principal
    unsigned int shaderProgram; // ID del programa de shaders OpenGL compilado
    unsigned int VAO, VBO;    // VAO=contenedor, VBO=buffer de datos
    std::vector<Light> lights; // Lista de todos los puntos de luz
    
    // SISTEMAS
    CameraSystem camera;      // Sistema de cámara orbital
    InputManager* inputManager; // Sistema de entrada del usuario
    
    // VARIABLES DE TIMPIZACIÓN
    std::chrono::steady_clock::time_point splashStartTime; // Cuándo comenzó la splash
    
    // VARIABLES DE CONFIGURACIÓN
    float masterVolume;       // Volumen maestro (0.0 - 1.0)

public:
    /**
     * CONSTRUCTOR DEL MOTOR GRÁFICO
     * 
     * Inicializa todos los valores:
     * - Comienza en la pantalla SPLASH
     * - Todos los ID de OpenGL en 0 (nulo)
     * - El volumen en 80%
     * - Registra la hora actual para contar los 3 segundos de splash
    */
    GraphicsEngine() : currentState(SPLASH), nextState(SPLASH), window(nullptr), 
                       shaderProgram(0), VAO(0), VBO(0), masterVolume(0.8f) {
        splashStartTime = std::chrono::steady_clock::now();  // Registrar hora actual
    }

    /**
     * INICIALIZAR EL MOTOR GRÁFICO
     * 
     * Pasos principales:
     * 1. Inicializa GLFW (gestión de ventanas)
     * 2. Configura el contexto OpenGL 3.3
     * 3. Crea la ventana principal
     * 4. Inicializa GLAD (cargador de funciones OpenGL)
     * 5. Configura ImGui (interfaz de usuario)
     * 6. Compila los shaders
     * 7. Inicial los datos de los puntos de luz
     * 8. Configure los buffers de OpenGL
     * 
     * @return true si todo fue exitoso, false si hay error
    */
    bool initialize() {
        // PASO 1: Inicializar GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        // PASO 2: Configurar contexto OpenGL 3.3 Core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);    // Versión mayor = 3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);    // Versión menor = 3
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core = moderno

        // PASO 3: Crear la ventana principal
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
            "3D Graphics Engine", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);  // Hacer esta ventana el contexto activo
        glfwSwapInterval(1);             // Usar VSync (60 FPS)

        // PASO 4: Inicializar GLAD (cargador de funciones OpenGL)
        if (!gladLoadGL()) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            return false;
        }

        // PASO 5: Configurar ImGui para la interfaz de usuario
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();                         // Crear contexto ImGui
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();                       // Tema oscuro

        ImGui_ImplGlfw_InitForOpenGL(window, true);    // Conectar ImGui con GLFW
        ImGui_ImplOpenGL3_Init("#version 330");        // Conectar ImGui con OpenGL

        // Configurar OpenGL
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  // Área de renderizado
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);        // Color de fondo azul oscuro
        glEnable(GL_DEPTH_TEST);                       // Activar prueba de profundidad
        glEnable(GL_BLEND);                            // Activar transparencia
        glPointSize(5.0f);                             // Tamaño de puntos
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Modo de mezcla

        // PASO 6: Compilar los shaders y crear programa
        unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
        unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

        shaderProgram = glCreateProgram();      // Crear programa
        glAttachShader(shaderProgram, vertexShader);    // Adjuntar vertex shader
        glAttachShader(shaderProgram, fragmentShader);  // Adjuntar fragment shader
        glLinkProgram(shaderProgram);                   // Enlazar los shaders

        // Verificar errores de enlazado
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Program linking failed: " << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);           // Eliminar shader compilado
        glDeleteShader(fragmentShader);         // (ya no necesitamos el código)

        // PASO 7: Inicializar puntos de luz con posiciones aleatorias
        initializeLights();

        // PASO 8: Configurar buffers de OpenGL para almacenar datos de vértices
        glGenVertexArrays(1, &VAO);  // Crear contenedor de atributos
        glGenBuffers(1, &VBO);       // Crear buffer de datos

        glBindVertexArray(VAO);      // Activar el contenedor
        glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Activar el buffer
        // Reservar espacio: 6 floats * 75 luces = 450 floats
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * NUM_LIGHTS, nullptr, GL_DYNAMIC_DRAW);

        // Atributo 0: Posición (primeros 3 floats de cada vértice)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Atributo 1: Color (siguientes 3 floats de cada vértice)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Inicializar InputManager y CameraSystem
        inputManager = new InputManager(window);
        camera.reset();

        return true;
    }

    /**
     * INICIALIZAR PUNTOS DE LUZ
     * 
     * Crea 75 puntos de luz con:
     * - Posiciones aleatorias en el espacio 3D
     * - Colores aleatorios (RGB)
     * - Velocidades aleatorias para que se muevan de forma imprevisible
     * 
     * LOS RANGOS DE VALORES:
     * - X: -20 a +20 (izquierda a derecha)
     * - Y: -15 a +15 (arriba a abajo)
     * - Z: -40 a -5 (lejos a cerca de la cámara)
     * - Velocidad: -0.05 a +0.05 por fotograma
    */
    void initializeLights() {
        lights.clear();  // Limpiar luces anteriores
        
        // Crear generador de números aleatorios
        std::mt19937 gen(std::random_device{}());
        
        // Definir rangos de valores aleatorios
        std::uniform_real_distribution<float> dist_x(-20.0f, 20.0f);    // Posición X
        std::uniform_real_distribution<float> dist_y(-15.0f, 15.0f);    // Posición Y
        std::uniform_real_distribution<float> dist_z(-40.0f, -5.0f);    // Posición Z
        std::uniform_real_distribution<float> dist_vel(-0.05f, 0.05f);  // Velocidad
        std::uniform_real_distribution<float> dist_color(0.4f, 1.0f);   // Color (brillo)

        // Crear 75 puntos de luz
        for (int i = 0; i < NUM_LIGHTS; ++i) {
            Light light;
            // Generar posición aleatoria
            light.position = glm::vec3(dist_x(gen), dist_y(gen), dist_z(gen));
            // Generar color aleatorio (blanco con variación)
            light.color = glm::vec3(dist_color(gen), dist_color(gen), dist_color(gen));
            // Generar velocidad aleatoria (Z se mueve más lentamente)
            light.velocity = glm::vec3(dist_vel(gen), dist_vel(gen), dist_vel(gen) * 0.1f);
            
            lights.push_back(light);  // Agregar a la lista
        }
    }

    /**
     * PROCESAR ENTRADA DEL USUARIO
     * 
     * Actualmente solo maneja la tecla ESC:
     * - Si estamos en PLAYING: vuelve al MENU
     * - Si no: cierra la aplicación
    */
    void handleInput() {
        // Verificar si se presionó la tecla ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (currentState == PLAYING) {
                nextState = MENU;  // Volver al menú si estamos jugando
            } else {
                glfwSetWindowShouldClose(window, true);  // Cerrar la app
            }
        }
    }

    /**
     * ACTUALIZAR LÓGICA (se llama cada fotograma)
     * 
     * Responsabilidades:
     * 1. Mover los puntos de luz según su velocidad
     * 2. Hacer que reboten cuando toquen los límites
     * 3. Gestionar el temporizador de la pantalla splash
     * 4. Cambiar entre estados
     * 
     * @param deltaTime - Tiempo transcurrido desde el último fotograma
    */
    void update(float deltaTime) {
        // PROCESAR INPUTS (solo en estado PLAYING)
        if (currentState == PLAYING && inputManager) {
            inputManager->update();
            const auto& input = inputManager->getInputState();
            
            // MOVIMIENTO ROTADO DE CÁMARA
            float moveSpeed = 0.5f;
            float forward = 0.0f, right = 0.0f;
            
            if (input.keyW) forward += moveSpeed;
            if (input.keyS) forward -= moveSpeed;
            if (input.keyD) right += moveSpeed;
            if (input.keyA) right -= moveSpeed;
            
            if (forward != 0.0f || right != 0.0f) {
                camera.panTargetRotated(forward, right);
            }
            
            // ROTACIÓN DE CÁMARA CON RATÓN (botón derecho + movimiento horizontal)
            if (input.mouseRightPressed) {
                float rotationSpeed = 0.1f;  // Grados por píxel de movimiento del ratón
                camera.rotate((float)input.mouseDeltaX * rotationSpeed);
            }
            
            // ZOOM
            float zoomSpeed = 0.5f;
            if (input.keyUp) camera.adjustDistance(-zoomSpeed);
            if (input.keyDown) camera.adjustDistance(zoomSpeed);
        }

        // ACTUALIZAR POSICIÓN DE LUCES
        for (auto& light : lights) {
            // Mover la luz según su velocidad
            light.position += light.velocity;

            // Definir límites del espacio
            const float boundX = 20.0f, boundY = 15.0f, boundZ = 35.0f;
            
            // Detectar colisión con límites y hacer rebotar (invertir velocidad)
            if (light.position.x < -boundX || light.position.x > boundX) {
                light.velocity.x *= -1.0f;  // Invertir dirección X
                light.position.x = glm::clamp(light.position.x, -boundX, boundX);  // Ajustar posición
            }
            if (light.position.y < -boundY || light.position.y > boundY) {
                light.velocity.y *= -1.0f;  // Invertir dirección Y
                light.position.y = glm::clamp(light.position.y, -boundY, boundY);
            }
            if (light.position.z < -40.0f || light.position.z > -5.0f) {
                light.velocity.z *= -1.0f;  // Invertir dirección Z
                light.position.z = glm::clamp(light.position.z, -40.0f, -5.0f);
            }
        }

        // GESTIONAR SPLASH SCREEN (mostrar solo 3 segundos)
        if (currentState == SPLASH) {
            auto elapsed = std::chrono::steady_clock::now() - splashStartTime;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 3) {
                currentState = MENU;  // Cambiar a menú después de 3 segundos
                nextState = MENU;     // Sincronizar el siguiente estado
            }
        } else {
            // CAMBIAR ESTADO (solo si NO estamos en splash)
            currentState = nextState;
        }
    }

    /**
     * RENDERIZAR (dibujar cada fotograma)
     * 
     * Pasos:
     * 1. Limpiar la pantalla (color y profundidad)
     * 2. Llamar a la función de renderizado según el estado actual
     * 3. Mostrar la imagen en pantalla
    */
    void render() {
        // Limpiar el buffer de color y profundidad
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Renderizar la pantalla correspondiente al estado actual
        if (currentState == SPLASH) {
            renderSplashScreen();  // Pantalla inicial
        } else if (currentState == MENU) {
            renderMenu();          // Menú principal
        } else if (currentState == PLAYING) {
            renderGameScene();     // Escena 3D con luces
        } else if (currentState == SETTINGS) {
            renderSettings();      // Pantalla de configuración
        } else if (currentState == CREDITS) {
            renderCredits();       // Pantalla de créditos
        }

        // Mostrar la imagen renderizada en pantalla
        glfwSwapBuffers(window);
    }

    void renderSplashScreen() {
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
        
        // Center the text
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

    /**
     * RENDERIZAR MENÚ PRINCIPAL
     * 
     * Menú con 3 botones:
     * 1. PLAY - Inicia la escena 3D
     * 2. SETTINGS - Abre la pantalla de configuración
     * 3. CREDITS - Muestra los créditos
    */
    void renderMenu() {
        // Preparar ImGui para este fotograma
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Centrar ventana en pantalla
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);

        ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // Titulo centrado
        ImVec2 textSize = ImGui::CalcTextSize("3D Graphics Engine");
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
        ImGui::Text("3D Graphics Engine");
        
        ImGui::Spacing();
        ImGui::Spacing();

        // BOTÓN: PLAY
        if (ImGui::Button("Play", ImVec2(250, 50))) {
            nextState = PLAYING;        // Cambiar a escena de juego
            initializeLights();          // Crear nuevas luces aleatorias
        }

        ImGui::Spacing();

        // BOTÓN: SETTINGS
        if (ImGui::Button("Settings", ImVec2(250, 50))) {
            nextState = SETTINGS;  // Cambiar a pantalla de configuración
        }

        ImGui::Spacing();

        // BOTÓN: CREDITS
        if (ImGui::Button("Credits", ImVec2(250, 50))) {
            nextState = CREDITS;  // Cambiar a pantalla de créditos
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * RENDERIZAR ESCENA DE JUEGO (75 LUCES DINÁMICAS)
     * 
     * Esta es la escena principal 3D donde se muestran los puntos de luz.
     * Proceso:
     * 1. Configurar matrices de proyección, vista y modelo
     * 2. Actualizar datos de vértices en GPU
     * 3. Dibujar todos los puntos
     * 4. Mostrar información en pantalla con ImGui
    */
    void renderGameScene() {
        // Usar el programa de shaders compilado
        glUseProgram(shaderProgram);

        // CREAR MATRICES DE TRANSFORMACIÓN
        // Matriz de proyección: cómo convertir 3D a 2D (perspectiva)
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
            (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
        
        // Matriz de vista: USAR LA CÁMARA EN LUGAR DE HARDCODEAR
        glm::mat4 view = camera.getViewMatrix();
        
        // Matriz de modelo: transformaciones del objeto (no usamos en este caso)
        glm::mat4 model = glm::mat4(1.0f);

        // Obtener ubicaciones de las variables uniform en el shader
        GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "uView");
        GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");

        // Enviar matrices al shader
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // PREPARAR DATOS DE VÉRTICES
        // Crear lista: [pos.x, pos.y, pos.z, col.r, col.g, col.b, ...]
        std::vector<float> vertices;
        for (const auto& light : lights) {
            // Posición
            vertices.push_back(light.position.x);
            vertices.push_back(light.position.y);
            vertices.push_back(light.position.z);
            // Color
            vertices.push_back(light.color.r);
            vertices.push_back(light.color.g);
            vertices.push_back(light.color.b);
        }

        // Enviar datos al GPU
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

        // Renderizar todos los puntos
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, NUM_LIGHTS);  // Dibujar 75 puntos

        // MOSTRAR INFORMACIÓN EN PANTALLA
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Panel de información en la esquina superior izquierda
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.35f);  // Semi-transparente
        ImGui::Begin("Game Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("ESC: Back to Menu");
        ImGui::Text("W/A/S/D: Move Camera");
        ImGui::Text("Right Mouse + Drag: Rotate Camera");
        ImGui::Text("UP/DOWN: Zoom");
        ImGui::Text("Lights: %d", NUM_LIGHTS);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * RENDERIZAR PANTALLA DE CONFIGURACIÓN
     * 
     * Opciones disponibles:
     * - Control deslizable de volumen maestro (0% a 100%)
     * - Botón de regreso al menú
    */
    void renderSettings() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Centrar ventana
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(350, 250), ImGuiCond_Always);

        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // Control deslizable de volumen
        // masterVolume: 0.0 = silencio, 1.0 = máximo
        ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 1.0f);
        
        ImGui::Spacing();
        ImGui::Spacing();

        // Botón para volver al menú
        if (ImGui::Button("Back", ImVec2(300, 50))) {
            nextState = MENU;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * RENDERIZAR PANTALLA DE CRÉDITOS
     * 
     * Muestra información sobre las tecnologías y desarrolladores
     * utilizados en el proyecto.
    */
    void renderCredits() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Centrar ventana
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);

        ImGui::Begin("Credits", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // Título centrado
        ImVec2 textSize = ImGui::CalcTextSize("Credits");
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
        ImGui::Text("Credits");
        
        ImGui::Spacing();

        // Lista de créditos
        ImGui::Text("Developer: Fabian");       // Creador del proyecto
        ImGui::Text("Graphics: OpenGL");        // API de gráficos
        ImGui::Text("UI: ImGui");               // Framework de interfaz
        ImGui::Text("Math: GLM");               // Librería de matemáticas
        ImGui::Text("Window: GLFW");            // Gestión de ventanas

        ImGui::Spacing();
        ImGui::Spacing();

        // Botón de regreso
        if (ImGui::Button("Back", ImVec2(350, 50))) {
            nextState = MENU;
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /**
     * VERIFICAR SI LA APLICACIÓN ESTÁ CORRIENDO
     * 
     * Devuelve true mientras la ventana esté abierta.
     * Devuelve false cuando el usuario cierra la ventana o presiona ESC en el menú.
     * 
     * @return true si la app sigue corriendo, false si debe cerrarse
    */
    bool isRunning() {
        return !glfwWindowShouldClose(window);
    }

    /**
     * LIMPIAR RECURSOS
     * 
     * Libera toda la memoria y recursos antes de cerrar la aplicación.
     * IMPORTANTE: Hacer esto evita memory leaks (pérdida de memoria).
     * 
     * Pasos:
     * 1. Apagar ImGui
     * 2. Eliminar buffers de OpenGL
     * 3. Eliminar programa de shaders
     * 4. Cerrar ventana
     * 5. Terminar GLFW
    */
    void cleanup() {
        // Apagar ImGui
        ImGui_ImplOpenGL3_Shutdown();  // Limpiar backend OpenGL
        ImGui_ImplGlfw_Shutdown();     // Limpiar backend GLFW
        ImGui::DestroyContext();       // Destruir contexto ImGui

        // Eliminar recursos de OpenGL
        glDeleteVertexArrays(1, &VAO);   // Eliminar contenedor de atributos
        glDeleteBuffers(1, &VBO);        // Eliminar buffer de datos
        glDeleteProgram(shaderProgram);  // Eliminar programa de shaders

        // Cerrar ventana y GLFW
        glfwDestroyWindow(window);  // Destruir la ventana
        glfwTerminate();            // Terminar GLFW
    }
};

/**
 * FUNCIÓN PRINCIPAL - PUNTO DE ENTRADA DEL PROGRAMA
 * 
 * Este es el código que se ejecuta cuando inicias la aplicación.
 * 
 * Flujo principal:
 * 1. Crear instancia del motor gráfico
 * 2. Inicializar todos los recursos (ventana, OpenGL, ImGui)
 * 3. LOOP INFINITO hasta que el usuario cierre:
 *    a. Calcular delta time (tiempo desde el último fotograma)
 *    b. Procesar entrada del usuario (teclado, ratón)
 *    c. Actualizar lógica del juego
 *    d. Renderizar (dibujar) un fotograma
 *    e. Procesar eventos de GLFW
 * 4. Limpiar recursos antes de salir
 * 
 * @return 0 si todo fue ok, -1 si hay error
*/
int main() {
    // Crear una instancia del motor gráfico
    GraphicsEngine engine;

    // Inicializar: configurar ventana, OpenGL, ImGui, shaders
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize graphics engine" << std::endl;
        return -1;  // Error: salir del programa
    }

    // Registrar la hora actual para calcular delta time
    auto lastTime = std::chrono::high_resolution_clock::now();

    // LOOP PRINCIPAL - Se ejecuta hasta que se cierre la ventana
    while (engine.isRunning()) {
        // Calcular el tiempo transcurrido desde el último fotograma
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Procesar entrada del usuario (ESC, etc.)
        engine.handleInput();
        
        // Actualizar lógica: mover luces, cambiar estados, etc.
        engine.update(deltaTime);
        
        // Renderizar (dibujar) este fotograma
        engine.render();

        // Procesar eventos de GLFW (clics, redimensionar ventana, etc.)
        glfwPollEvents();
    }

    // Liberar todos los recursos antes de salir
    engine.cleanup();
    
    return 0;  // Retornar 0 = éxito
}