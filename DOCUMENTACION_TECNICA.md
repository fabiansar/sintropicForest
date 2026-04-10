# 📖 Documentación Técnica Detallada

## El Programa Explicado Paso a Paso

---

## 1️⃣ `initialize()` - El Punto de Partida

### Qué hace:
Prepara TODO lo que necesita la aplicación para funcionar.

### Pasos que ejecuta:

```
1. glfwInit()
   ↓ Inicializa la librería GLFW (para ventanas)

2. glfwWindowHint() [3 veces]
   ↓ Configura que queremos OpenGL 3.3 Core Profile

3. glfwCreateWindow()
   ↓ Crea la ventana de 1280x720 píxeles

4. glfwMakeContextCurrent()
   ↓ Hace que esta ventana sea el contexto activo
   
5. glfwSwapInterval(1)
   ↓ Activa VSync (limita a 60 FPS)

6. gladLoadGL()
   ↓ Carga las funciones de OpenGL

7. ImGui::CreateContext()
   ↓ Crea el sistema de interfaz ImGui

8. ImGui_ImplGlfw_InitForOpenGL()
   ↓ Conecta ImGui con GLFW

9. ImGui_ImplOpenGL3_Init()
   ↓ Conecta ImGui con OpenGL

10. glViewport(), glClearColor(), glEnable()
    ↓ Configura OpenGL (área de renderizado, color fondo, etc.)

11. compileShader() [dos veces]
    ↓ Compila los shaders de vértices y fragmentos

12. glCreateProgram()
    ↓ Crea el programa de shaders

13. initializeLights()
    ↓ Crea 75 luces con posiciones aleatorias

14. glGenVertexArrays() y glGenBuffers()
    ↓ Prepara los buffers para enviar datos a la GPU
```

---

## 2️⃣ `initializeLights()` - Crear 75 Puntos Aleatorios

### Qué sucede:

```cpp
for (int i = 0; i < NUM_LIGHTS; ++i) {
    // Para cada una de las 75 luces...
    
    Light light;
    
    // Generar posición aleatoria
    light.position = glm::vec3(
        randomFloat(-20, 20),  // X: de -20 a +20
        randomFloat(-15, 15),  // Y: de -15 a +15
        randomFloat(-40, -5)   // Z: de -40 a -5
    );
    
    // Generar color aleatorio (blanco con variación)
    light.color = glm::vec3(
        randomFloat(0.4, 1.0),  // Rojo
        randomFloat(0.4, 1.0),  // Verde
        randomFloat(0.4, 1.0)   // Azul
    );
    
    // Generar velocidad aleatoria
    light.velocity = glm::vec3(
        randomFloat(-0.05, 0.05),      // Vel X
        randomFloat(-0.05, 0.05),      // Vel Y
        randomFloat(-0.05, 0.05) * 0.1 // Vel Z (más lenta)
    );
    
    lights.push_back(light);  // Agregar a la lista
}
```

**Resultado:** Una lista de 75 estructuras Light con datos aleatorios.

---

## 3️⃣ `update()` - Actualizar Posiciones (Cada Fotograma)

### Movimiento básico:

```cpp
light.position += light.velocity;
```

Esto simplemente suma la velocidad a la posición. Si velocidad es (0.01, 0, 0), el punto se mueve 0.01 unidades a la derecha.

### Detección de colisiones:

```cpp
if (light.position.x < -20 || light.position.x > 20) {
    light.velocity.x *= -1;  // Invertir dirección (rebotar)
}
```

Si la luz salir del límite -20 a +20 en X, se invierte la velocidad en X.

---

## 4️⃣ `render()` - Dibujar Un Fotograma

### Flujo:

```
glClear()
  ↓ Limpiar la pantalla (borrar fotograma anterior)

if (currentState == SPLASH)
  ↓ Llamar a renderSplashScreen()
else if (currentState == MENU)
  ↓ Llamar a renderMenu()
else if (currentState == PLAYING)
  ↓ Llamar a renderGameScene()
  (aquí es donde se dibujan los 75 puntos)

glfwSwapBuffers()
  ↓ Mostrar la imagen en pantalla
```

---

## 5️⃣ `renderGameScene()` - Dibujar 75 Luces

### El proceso técnico:

```cpp
// 1. Usar el programa de shaders
glUseProgram(shaderProgram);

// 2. Crear matrices de transformación
glm::mat4 projection = ...;  // Cómo ver el espacio 3D
glm::mat4 view = ...;        // Dónde están mis ojos
glm::mat4 model = ...;       // Dónde está el objeto

// 3. Enviar matrices al shader
glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

// 4. Preparar datos (posición + color) de cada luz
std::vector<float> vertices;
for (const auto& light : lights) {
    vertices.push_back(light.position.x);
    vertices.push_back(light.position.y);
    vertices.push_back(light.position.z);
    vertices.push_back(light.color.r);
    vertices.push_back(light.color.g);
    vertices.push_back(light.color.b);
}

// 5. Enviar datos a la GPU
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

// 6. Dibujar todos los puntos
glDrawArrays(GL_POINTS, 0, NUM_LIGHTS);  // Dibuja 75 puntos
```

---

## 📊 Estados de la Aplicación (GameState)

```
SPLASH
  ↓ (después de 3 segundos)
MENU
  ↓ (usuario presiona "Play")
PLAYING
  ↓ (usuario presiona "ESC")
MENU
```

Flujo alternativo:
```
MENU
  ↓ (usuario presiona "Settings")
SETTINGS
  ↓ (usuario presiona "Back")
MENU
```

O:
```
MENU
  ↓ (usuario presiona "Credits")
CREDITS
  ↓ (usuario presiona "Back")
MENU
```

---

## 🔄 El Loop Principal

```cpp
while (engine.isRunning()) {  // Repetir mientras no cierres la ventana
    
    // Calcular tiempo desde último fotograma
    auto currentTime = high_resolution_clock::now();
    float deltaTime = (currentTime - lastTime).count();
    lastTime = currentTime;
    
    // Llamar funciones en orden
    engine.handleInput();     // Procesar entrada
    engine.update(deltaTime); // Actualizar lógica
    engine.render();          // Dibujar
    
    glfwPollEvents();         // Procesar eventos de ventana
}
```

Este loop se ejecuta aproximadamente 60 veces por segundo (60 FPS).

---

## 🎨 Sistema de Sombreadores (Shaders)

### Vertex Shader
```glsl
#version 330 core

layout (location = 0) in vec3 aPosition;  // Entrada: posición
layout (location = 1) in vec3 aColor;    // Entrada: color

out vec3 vertexColor;  // Salida: color para el fragment shader

uniform mat4 uProjection;  // Matriz de proyección
uniform mat4 uView;        // Matriz de vista
uniform mat4 uModel;       // Matriz de modelo

void main() {
    // Transformar posición 3D a 2D
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    
    // Pasar color al siguiente shader
    vertexColor = aColor;
}
```

**Qué hace:**
1. Recibe la posición 3D de cada vértice (punto de luz)
2. Aplica las matrices de transformación
3. Convierte a coordenadas 2D para la pantalla
4. Pasa el color al fragment shader

### Fragment Shader
```glsl
#version 330 core

in vec3 vertexColor;      // Color recibido del vertex shader
out vec4 FragColor;       // Color final del píxel

void main() {
    // Calcular distancia del píxel al centro del punto
    float distance = length(gl_PointCoord - vec2(0.5));
    
    if (distance > 0.5)
        discard;  // Descartar píxeles fuera del punto
    
    // Crear efecto de brillo (glow)
    float alpha = 1.0 - (distance * 2.0);
    
    // Output: color con transparencia
    FragColor = vec4(vertexColor, alpha);
}
```

**Qué hace:**
1. Recibe el color del vertex shader
2. Calcula si el píxel está dentro del punto circular
3. Crea un efecto de brillo usando la transparencia
4. Produce el color final

---

## 💾 Buffers de OpenGL

### VAO (Vertex Array Object)
```
Contenedor que almacena:
├── Cómo está organizado los datos
├── Qué atributos hay (posición, color)
└── Dónde está cada atributo
```

### VBO (Vertex Buffer Object)
```
Almacena los datos actuales:
├── Posiciones X, Y, Z de cada luz
├── Colores R, G, B de cada luz
└── Se actualiza cada fotograma
```

**Formato de datos en VBO:**
```
[Pos.X, Pos.Y, Pos.Z, Col.R, Col.G, Col.B, Pos.X, Pos.Y, Pos.Z, Col.R, Col.G, Col.B, ...]
 └─── Primera luz ──────────────────────────┘  └─── Segunda luz ──────────────────────┘
```

---

## 🔐 Gestión de Memoria

### Allocación (Crear recursos):
```cpp
glGenVertexArrays(1, &VAO);  // Reservar espacio en GPU
glGenBuffers(1, &VBO);
glCreateProgram();
```

### Liberación (Limpiar recursos):
```cpp
glDeleteVertexArrays(1, &VAO);  // Liberar memoria de GPU
glDeleteBuffers(1, &VBO);
glDeleteProgram(shaderProgram);

ImGui_ImplOpenGL3_Shutdown();   // Liberar ImGui resources
glfwDestroyWindow(window);       // Cerrar ventana
glfwTerminate();                 // Terminar GLFW
```

**Importancia:** Si no haces esto, tienes "memory leaks" (fugas de memoria).

---

## 🎛️ Configuración de OpenGL

```cpp
glViewport(0, 0, WIDTH, HEIGHT);
// Define el área rectangular donde se dibuja

glClearColor(R, G, B, A);
// Color que se usa para limpiar la pantalla

glEnable(GL_DEPTH_TEST);
// Habilita prueba de profundidad (objetos más cercanos ocultan lejanos)

glEnable(GL_BLEND);
// Habilita transparencia

glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// Define cómo mezclar colores con transparencia

glPointSize(5.0f);
// Tamaño de los puntos (5 píxeles)
```

---

## 🐛 Datos de Debug

Para ver información de compilación/errores:

```cpp
int success;
char infoLog[512];
glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "Error: " << infoLog << std::endl;  // Imprimir error
}
```

---

¡Espero que esto haya aclarado cómo funciona TODO! 🎉
