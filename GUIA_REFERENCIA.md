# 📚 Guía de Referencia para el Motor Gráfico 3D

## Para Inútiles Como Yo (Y Tú También)

---

## 📁 Estructura del Proyecto

```
IA/
├── CMakeLists.txt         ← Instrucciones de compilación
├── build.ps1             ← Script para compilar el proyecto
├── build/
│   └── 3DEngine.exe      ← El ejecutable (lo que corres)
├── src/
│   └── main.cpp          ← TODO EL CÓDIGO (altamente documentado)
└── README.md
```

---

## 🎮 Cómo Usar la Aplicación

1. **Ejecuta el programa:**
   ```
   C:\Users\elfab\Documents\IA\build\3DEngine.exe
   ```

2. **Qué ves:**
   - **Splash Screen (3 segundos):** "Made by Fabian"
   - **Menú Principal:** 3 botones
   - **Play:** Muestra 75 puntos de luz animándose en 3D
   - **Settings:** Control de volumen
   - **Credits:** Información del proyecto

3. **Controles:**
   - `ESC` en juego → volver al menú
   - `ESC` en menú → cerrar programa
   - Mouse para hacer click en botones

---

## 🏛️ Estructura del Código Principal

### Clase GraphicsEngine

Es la clase principal que controla TODO el programa.

**Variables importantes:**
```cpp
GameState currentState;     // Estado actual (SPLASH, MENU, PLAYING, etc.)
GLFWwindow* window;        // La ventana que ves en pantalla
unsigned int shaderProgram; // Programa que dice cómo pintar
std::vector<Light> lights; // Lista de 75 puntos de luz
float masterVolume;        // Volumen (0.0 = mudo, 1.0 = máximo)
```

### Funciones Clave

| Función | Qué hace |
|---------|----------|
| `initialize()` | Configura GLFW, OpenGL, ImGui, shaders |
| `initializeLights()` | Crea 75 luces con posiciones aleatorias |
| `update()` | Mueve las luces, detecta colisiones |
| `render()` | Dibuja el fotograma actual |
| `renderMenu()` | Dibuja el menú con botones |
| `renderGameScene()` | Dibuja los 75 puntos de luz en 3D |
| `cleanup()` | Libera memoria antes de cerrar |

---

## 📊 Estructura de un Punto de Luz (Light)

```cpp
struct Light {
    glm::vec3 position;  // Dónde está: (X, Y, Z) en el espacio
    glm::vec3 color;     // De qué color es: (Rojo, Verde, Azul)
    glm::vec3 velocity;  // Cómo se mueve cada fotograma
};
```

**Valores típicos:**
- X: de -20 a +20 (izquierda a derecha)
- Y: de -15 a +15 (arriba a abajo)
- Z: de -40 a -5 (cerca a lejos)
- Color: de 0.0 a 1.0 en cada componente

---

## 🔧 Cómo Compilar

**Opción 1 (Recomendado - VS Code):**
```
Ctrl + Shift + B
```

**Opción 2 (PowerShell):**
```powershell
powershell -ExecutionPolicy Bypass -File C:\Users\elfab\Documents\IA\build.ps1
```

---

## 🎨 Entendiendo los Shaders

### Vertex Shader (Sombreador de Vértices)
```glsl
// Transforma la posición 3D a la pantalla 2D
gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
```

**En palabras normales:** Toma un punto en el espacio 3D y lo convierte a píxeles en tu pantalla.

### Fragment Shader (Sombreador de Fragmentos)
```glsl
// Decide el color de cada píxel
// Crea un efecto de brillo mediendo la distancia al centro
```

**En palabras normales:** Pinta cada píxel con un color basado en cuán lejos está del centro del punto.

---

## 📐 Conceptos Matemáticos

### Matrices de Transformación

1. **Projection Matrix** - Cómo ves el espacio 3D (perspectiva)
2. **View Matrix** - Dónde está la cámara y a dónde mira
3. **Model Matrix** - Posición/rotación/escala del objeto

**Fórmula final:**
```
Posición en pantalla = Projection × View × Model × Posición 3D
```

---

## 🌈 Personalizaciones Fáciles

### Cambiar número de luces
Busca en `main.cpp`:
```cpp
const int NUM_LIGHTS = 75;  // Cambiar a 100, 50, etc.
```

### Cambiar tamaño de ventana
```cpp
const int WINDOW_WIDTH = 1280;   // Ancho
const int WINDOW_HEIGHT = 720;   // Alto
```

### Cambiar rango de movimiento
En `initializeLights()`:
```cpp
std::uniform_real_distribution<float> dist_x(-20.0f, 20.0f);  // -50 a 50, etc.
```

### Cambiar color de fondo
En la función `initialize()`:
```cpp
glClearColor(0.05f, 0.05f, 0.1f, 1.0f);  // R, G, B, Alpha
// Esto es azul muy oscuro. Prueba (1.0f, 1.0f, 1.0f, 1.0f) para blanco
```

---

## 🐛 Solución de Problemas

### "El programa no inicia"
1. Verificar que `3DEngine.exe` existe en `build/`
2. Cambiar la ruta e intentar de nuevo: `C:\Users\elfab\Documents\IA\build\3DEngine.exe`
3. Verificar que no hay otra instancia abierta (Task Manager)

### "Puntos de luz se mueven muy rápido/lento"
Cambiar velocidad en `initializeLights()`:
```cpp
std::uniform_real_distribution<float> dist_vel(-0.05f, 0.05f);
// -0.1f a 0.1f = más rápido
// -0.02f a 0.02f = más lento
```

### "ImGui no se ve"
Verificar que ImGui está correctamente inicializado en `initialize()`.

---

## 📚 Librerías Usadas

| Librería | Qué hace |
|----------|----------|
| **GLAD** | Cargador de funciones OpenGL |
| **GLFW** | Manejo de ventanas y entrada (teclado, mouse) |
| **GLM** | Matemáticas para gráficos (vectores, matrices) |
| **ImGui** | Interfaz de usuario (botones, menús, ventanas) |
| **OpenGL** | API para renderizar gráficos 3D |

---

## 🎯 Loop Principal (Corazón del Programa)

```cpp
while (engine.isRunning()) {  // Mientras la ventana esté abierta
    
    // 1. Calcular cuánto tiempo pasó
    float deltaTime = currentTime - lastTime;
    
    // 2. Procesar entrada (ESC, clicks, etc.)
    engine.handleInput();
    
    // 3. Actualizar lógica (mover luces)
    engine.update(deltaTime);
    
    // 4. Dibujar este fotograma
    engine.render();
    
    // 5. Procesar eventos de la ventana
    glfwPollEvents();
}
```

Esto se repite ~60 veces por segundo (VSync activado).

---

## 🚀 Pasos Siguientes

1. Experimenta cambiando valores (velocidad, colores, cantidad de luces)
2. Intenta agregar tu propio botón en el menú
3. Modifica los shaders para crear efectos visuales diferentes
4. Lee los comentarios en `main.cpp` para entender más detalles

---

## 📞 Notas Finales

- **No eres inútil**, solo necesitas aprender paso a paso
- Todos comenzamos así (sin entender nada)
- Experimenta, rompe cosas, aprende de los errores
- La documentación está en española y muy detallada en el código

¡Buena suerte! 🍀
