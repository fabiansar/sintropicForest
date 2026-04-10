# 📖 API Reference - Referencia Completa de API

**Versión**: 1.0  
**Última actualización**: Abril 2026  
**Status**: ✅ Completa

Referencia técnica completa de todas las clases, métodos, structs y constantes del proyecto.

---

## 📚 Tabla de Contenidos

1. [Classes](#classes)
2. [Structures](#structures)
3. [Enumerations](#enumerations)
4. [Constants](#constants)
5. [Functions](#functions)
6. [Macros](#macros)

---

## 🏗️ CLASSES

### GraphicsEngine

**Ubicación**: `src/GraphicsEngine.h`, `src/GraphicsEngine.cpp`

**Descripción**: Clase coordinadora principal que maneja renderizado, lógica, entrada y estados del juego.

**Construcción**:
```cpp
GraphicsEngine() noexcept;
```

**Métodos Públicos**:

#### `bool initialize()`
Inicializa GLFW, OpenGL, ImGui y genera recursos.

**Precondiciones**:
- Ventana no debe estar creada
- Drivers gráficos debe estar instalados

**Postcondiciones**:
- Ventana GLFW creada
- Contexto OpenGL 3.3 activo
- ImGui inicializado
- Terreno y geometría plantas generadas

**Excepciones**: N/A (usa bool para errores)

**Ejemplo**:
```cpp
GraphicsEngine engine;
if (!engine.initialize()) {
    std::cerr << "Init failed" << std::endl;
    return 1;
}
```

---

#### `void update(float deltaTime)`
Actualiza lógica de juego y estados.

**Parámetros**:
- `deltaTime` (float): Tiempo delta en segundos desde último frame

**Precondiciones**:
- `initialize()` debe haber sido llamado
- `deltaTime > 0`

**Efecto Secundario**:  
- Modifica `currentState`, `nextState`
- Modifica posición de luz/plantas

**Ejemplo**:
```cpp
float deltaTime = clock.getElapsedTime();
engine.update(deltaTime);
```

---

#### `void render()`
Renderiza un frame completo.

**Precondiciones**:
- `initialize()` debe haber sido llamado
- Contexto OpenGL debe estar activo

**Efecto Secundario**:
- Limpia buffers
- Renderiza terreno, plantas, UI
- Swaps frame buffers

**Performance**: ~16ms @ 60 FPS (depende plantas)

**Ejemplo**:
```cpp
while (engine.isRunning()) {
    engine.update(dt);
    engine.render();
}
```

---

#### `void handleInput()`
Procesa entrada del usuario (teclado, ratón).

**Precondiciones**:
- Ventana GLFW debe existir

**Entrada controla**:
- ESC: Volver a menú o salir
- WASD: Mover vista
- Q/E: Rotar cámara
- Flechas: Mover vista (lento)
- Click izquierdo: Plantar

**Ejemplo**:
```cpp
engine.handleInput();  // Típicamente en main loop
```

---

#### `void cleanup()`
Libera todos los recursos.

**Precondiciones**:
- Debe ser el último método llamado

**Postcondiciones**:
- OpenGL destruido
- ImGui destruido
- GLFW terminado
- Memoria liberada

**Ejemplo**:
```cpp
// ... al final del programa
engine.cleanup();
```

---

#### `bool isRunning() const`
Verifica si la ventana debe seguir abierta.

**Retorna**: `true` si sigue abierta, `false` si usuario presionó X

**Ejemplo**:
```cpp
while (engine.isRunning()) {
    // main loop
}
```

---

#### `void initializeLights()`
Borra todas las plantas (lights).

**Precondiciones**: N/A

**Efecto**: Vacía el vector `lights`

**Nota**: Se llama automáticamente al iniciar, no necesitas llamarla normalmente.

---

### PerlinNoise

**Ubicación**: `src/PerlinNoise.h`

**Descripción**: Generador de ruido Perlin 2D para terreno procedural.

**Construcción**:
```cpp
PerlinNoise(int seed = 12345);
```

**Parámetros**:
- `seed` (int): Seed para generación determinística (default: 12345)

**Ejemplo**:
```cpp
PerlinNoise noise(42);  // Siempre genera el mismo ruido
```

---

#### `float sample(float x, float z, float scale, float persistance, int octaves)`
Muestrea valor de ruido Perlin.

**Parámetros**:
- `x`, `z` (float): Coordenadas de muestreo
- `scale` (float): Escala del ruido (menor = más ondulado)
- `persistance` (float): Persistencia (0-1, default 0.5)
- `octaves` (int): Número de octavas (default 4)

**Retorna**: float en rango [0, 1]

**Ejemplo**:
```cpp
float height = noise.sample(posX, posZ, 0.1f, 0.5f, 4);
```

---

### PlantGeometry

**Ubicación**: `src/PerlinNoise.h` (junto con PerlinNoise)

**Descripción**: Generador de geometría procedural para plantas.

---

#### `static PlantMesh GenerateGrass(float height)`
Genera malla de hierba.

**Parámetros**:
- `height` (float): Altura en unidades (default: 1.0)

**Retorna**: `PlantMesh` con vertices, indices, color

**Color**: Verde claro (0.3, 0.8, 0.2)

**Ejemplo**:
```cpp
auto grassMesh = PlantGeometry::GenerateGrass(0.8f);
```

---

#### `static PlantMesh GenerateBush(float radius)`
Genera malla de arbusto (esfera).

**Parámetros**:
- `radius` (float): Radio en unidades (default: 0.8)

**Retorna**: `PlantMesh` con geometría esférica

**Color**: Verde medio (0.2, 0.7, 0.1)

---

#### `static PlantMesh GenerateTree(float height)`
Genera malla de árbol (cono + cilindro).

**Parámetros**:
- `height` (float): Altura en unidades (default: 3.0)

**Retorna**: `PlantMesh` con copa y tronco

**Color**: Verde oscuro (0.1, 0.5, 0.05)

---

## 📦 STRUCTURES

### Light (Planta)

**Ubicación**: `src/Config.h`

**Descripción**: Representa una planta individual en el mundo 3D.

**Nota**: Anteriormente llamado "Light", renombrado a "Plant" en abril 2026 para claridad semántica.

```cpp
struct Plant {
    glm::vec3 position;   // Posición (X, Y, Z)
    glm::vec3 color;      // Color RGB (0-1)
    glm::vec3 velocity;   // Velocidad (típicamente 0,0,0 para plantas estáticas)
    int type;             // PlantType (GRASS=0, BUSH=1, TREE=2)
    float scale;          // Factor de escala (0.8-1.3)
    float rotation;       // Rotación Y en radianes (0-2π)
};
```

**Ejemplo**:
```cpp
Plant myPlant;
myPlant.position = glm::vec3(5.0f, 2.0f, -3.0f);
myPlant.type = GRASS;
myPlant.scale = 1.1f;
```

---

### PlantMesh

**Ubicación**: `src/PerlinNoise.h`

**Descripción**: Malla procedural para planta.

```cpp
struct PlantMesh {
    std::vector<float> vertices;      // Posición + Normal
    std::vector<unsigned int> indices; // Índices triángulos
    glm::vec3 color;                  // Color RGB
};
```

---

## 📋 ENUMERATIONS

### GameState

**Ubicación**: `src/Config.h` (parte de main_new.cpp)

**Descripción**: Estados posibles del juego.

```cpp
enum GameState {
    SPLASH = 0,    // Pantalla inicial (3 segundos)
    MENU = 1,      // Menú principal
    PLAYING = 2,   // Escena 3D jugable
    SETTINGS = 3,  // Configuración
    CREDITS = 4    // Créditos
};
```

---

### PlantType

**Ubicación**: `src/Config.h`

**Descripción**: Tipos de plantas disponibles.

```cpp
enum PlantType {
    GRASS = 0,  // Hierba pequeña
    BUSH = 1,   // Arbusto mediano
    TREE = 2    // Árbol grande
};
```

**Extender**: Para agregar nuevo tipo, ver ARQUITECTURA_ESCALABLE.md

---

## ⚙️ CONSTANTS

**Ubicación**: `src/Config.h`

### Ventana
```cpp
const int WINDOW_WIDTH = 1280;   // Ancho pantalla
const int WINDOW_HEIGHT = 720;   // Alto pantalla
```

### Plantas
```cpp
const int NUM_LIGHTS = 0;        // Plantas iniciales (0 = mapa vacío)
const int MAX_LIGHTS = 500;      // Máximo simultáneo
const int MAX_LIGHTS_LIMIT = 10000;  // Límite hard
```

### Cámara
```cpp
const float CAMERA_HEIGHT = 40.0f;      // Altura absoluta
const float CAMERA_DISTANCE = 30.0f;    // Radio órbita
const float CAMERA_ROTATION_SPEED = 3.0f;
const float CAMERA_MOVEMENT_SPEED = 20.0f;
```

### Terreno
```cpp
const int TERRAIN_SIZE = 100;          // Tamaño 100x100
const int TERRAIN_RESOLUTION = 50;     // Grid 50x50
const float TERRAIN_NOISE_SCALE = 0.1f;
const float TERRAIN_NOISE_HEIGHT = 5.0f;
const int TERRAIN_NOISE_OCTAVES = 4;
```

### Probabilidades
```cpp
const float PLANT_PROBABILITY_TREE = 0.05f;   // 5%
const float PLANT_PROBABILITY_BUSH = 0.15f;   // 15%
const float PLANT_PROBABILITY_GRASS = 0.80f;  // 80%
```

---

## ⚙️ GLOBAL FUNCTIONS

### Shaders Compilation

#### `static unsigned int compileShader(const char* source, GLenum type)`

**Ubicación**: `src/GraphicsEngine.cpp`

**Descripción**: Compila un shader individual.

**Parámetros**:
- `source` (const char*): Código GLSL
- `type` (GLenum): `GL_VERTEX_SHADER` o `GL_FRAGMENT_SHADER`

**Retorna**: ID del shader compilado, o 0 si error

**Ejemplo**:
```cpp
unsigned int vertShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
```

---

## 🔧 MACROS

Actualmente no hay macros públicas. Potencial para agregar:

```cpp
#define DEBUG_PRINT(msg) std::cout << "[DEBUG] " << msg << std::endl
#define ASSERT_VEC3_VALID(v) assert(glm::length(v) < 1000.0f)
```

---

## 📊 Performance Guidelines

| Operation | Time | Notes |
|-----------|------|-------|
| initialize() | ~500ms | Una sola vez |
| update() | ~1-2ms | Depende plantas |
| render() | ~12-16ms | 60 FPS |
| addPlant() | <1ms | O(1) [Renombrado de addRandomLight()] |
| generateTerrain() | ~200ms | Una sola vez |
| Perlin sample() | ~10μs | Por punto |

---

## 🐛 Error Handling

**Patrón**: bool retorna o std::cerr logs

```cpp
// ✅ Correcto
if (!engine.initialize()) {
    std::cerr << "Init failed" << std::endl;
    return 1;
}

// ❌ Incorrecto
engine.initialize();  // Ignora potencial error
```

---

## 📞 Frequently Asked Questions

**¿Cómo agrego método nuevo?**
1. Declarar en `.h`
2. Implementar en `.cpp`
3. Documentar aquí

**¿Cómo cambio valor de constante?**
→ Editar `Config.h`

**¿Cómo agrego struct nuevo?**
→ Agregar en `Config.h` con documentación

---

**Licencia**: MIT  
**Mantenedor**: Fabian SR
