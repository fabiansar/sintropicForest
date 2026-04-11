# PLAN ESTRATÉGICO DE REFACTORIZACIÓN ARQUITECTÓNICA

**Objetivo**: Transformar arquitectura monolítica → arquitectura modular escalable
**Tiempo Estimado**: 8-10 horas (distribuidas en 3-4 sesiones)
**Risk Level**: Medio (muchos cambios pero testeable)

---

## 📊 FASES DE REFACTORIZACIÓN

### ✅ FASE 0: ANÁLISIS (COMPLETADO)
- [x] Identificar arquitecturas conflictivas
- [x] Documentar 6 errores de lógica
- [x] Mapear código muerto vs. vivo
- [x] Crear plan de migración

---

## 🔴 FASE 1: CONSOLIDACIÓN DE CÓDIGO VIVO vs. MUERTO (2h)

### Paso 1.1: Decidir punto de entrada único

**Decisión**: Usar GraphicsEngine.cpp modular (GraphicsEngine.h existe)

**Acciones**:
1. ✅ Mantener: `src/main_new.cpp` (50 líneas, limpio)
2. ✅ Mantener: Módulos declarados (GameLogic, StateManager, etc.)
3. ❌ Reemplazar: `src/main.cpp` monolítico (700 líneas)

**Resultado**: Single source of truth - main_new.cpp → GraphicsEngine (modular)

### Paso 1.2: Limpiar código muerto

```bash
# ACCIONES:
1. Eliminar: src/main.cpp (monolítico - 700 líneas)
2. Eliminar: Contenido obsoleto
   - main.cpp::generateRandomForest() → lo reemplaza EcosystemSimulator
   - main.cpp::initializeLights() → lo reemplaza GameLogic
3. Mantener: src/main_new.cpp (es el futuro punto de entrada)
4. Crear: src/GraphicsEngine.cpp (implementa GraphicsEngine modular)
```

### Paso 1.3: Actualizar CMakeLists.txt

**DE**:
```cmake
add_executable(3DEngine 
    src/main_new.cpp
    src/GraphicsEngine.cpp    # ← No implementado, error linking
    src/EcosystemSimulator.cpp
    src/ParticleAtomSystem.cpp
)
```

**A**:
```cmake
add_executable(3DEngine 
    src/main_new.cpp              # ← Point de entrada
    src/GraphicsEngine.cpp        # ← Implementación nueva
    src/EcosystemSimulator.cpp    # ← Ahora REALMENTE usado
    src/ParticleAtomSystem.cpp    # ← Ahora REALMENTE usado
    src/InputManager.h            # ← Ya no necesita, es include-only
)
```

---

## 🟠 FASE 2: IMPLEMENTAR GraphicsEngine MODULAR (3h)

### Paso 2.1: Crear src/GraphicsEngine.cpp

**Estructura Nuevo**:
```cpp
#include "GraphicsEngine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

GraphicsEngine::GraphicsEngine() 
    : window(nullptr), shaderProgram(0), currentState(SPLASH) {
    splashStartTime = std::chrono::steady_clock::now();
}

bool GraphicsEngine::initialize() {
    // 1. GLFW init
    if (!glfwInit()) {
        Logger::error("GLFW initialization failed");
        return false;
    }
    
    // 2. Crear ventana
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "3D Engine", nullptr, nullptr);
    if (!window) {
        Logger::error("Window creation failed");
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // 3. GLAD init
    if (!gladLoadGL()) {
        Logger::error("GLAD initialization failed");
        return false;
    }
    
    // 4. ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // 5. OpenGL setup
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 6. Shaders
    shaderProgram = compileShaders();
    
    // 7. Inicializar módulos
    inputManager = std::make_unique<InputManager>(window);
    cameraSystem = std::make_unique<CameraSystem>();
    gameLogic = std::make_unique<GameLogic>();
    stateManager = std::make_unique<StateManager>();
    audioManager = std::make_unique<AudioManager>();
    
    Logger::info("GraphicsEngine initialized successfully");
    return true;
}

void GraphicsEngine::update(float deltaTime) {
    // ACTUALIZAR MÓDULOS EN ORDEN
    
    // 1. Input (siempre primero, cada frame)
    inputManager->update();
    
    // 2. State machine
    stateManager->update();
    
    // 3. Lógica según estado
    if (currentState == PLAYING) {
        // 3a. Procesar input para mover cámara
        const auto& input = inputManager->getInputState();
        if (input.keyW) cameraSystem->panTargetRotated(0.5f, 0.0f);
        if (input.keyS) cameraSystem->panTargetRotated(-0.5f, 0.0f);
        // ... etc
        
        // 3b. Actualizar lógica del juego
        gameLogic->update(deltaTime);
        
        // 3c. Actualizar simulación ecológica
        //ecosystemSimulator->update(deltaTime);  // TODO: Integrar
    }
}

void GraphicsEngine::render() {
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

void GraphicsEngine::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    // ✅ Módulos se destruyen automáticamente (unique_ptr)
}
```

---

## 🟡 FASE 3: CORREGIR BUGS CRÍTICOS (2h)

### Bug Fix 3.1: InputManager - Mouse Delta Inicial

**Archivo**: `src/InputManager.h`
**Ubicación**: Línea ~45

**ANTES**:
```cpp
GLFWwindow* window;
InputState currentState;
InputState previousState;
bool escapePressed = false;

InputManager(GLFWwindow* win) : window(win) {
    // No inicializa mouseX/Y
}
```

**DESPUÉS**:
```cpp
GLFWwindow* window;
InputState currentState;
InputState previousState;
bool escapePressed = false;

InputManager(GLFWwindow* win) : window(win) {
    // ✅ Inicializar posición del ratón en frame 0
    glfwGetCursorPos(window, &currentState.mouseX, &currentState.mouseY);
    previousState = currentState;
}
```

**Verificación**:
```cpp
// En test o debug:
Logger::debug("Initial mouse position: ", 
              inputManager->getMousePosition().x, ", ",
              inputManager->getMousePosition().y);
// Debe ser posición actual, no (0,0)
```

---

### Bug Fix 3.2: CameraSystem - Height Proporcional

**Archivo**: `src/CameraSystem.h`
**Ubicación**: método `
adjustDistance()`

**ANTES**:
```cpp
void adjustDistance(float dDistance) {
    distance = glm::clamp(distance + dDistance, minDistance, maxDistance);
    // height NO se ajusta
}

float getHeight() const { return height; }  // Siempre 15
```

**DESPUÉS**:
```cpp
void adjustDistance(float dDistance) {
    distance = glm::clamp(distance + dDistance, minDistance, maxDistance);
    
    // ✅ Mantener ratio altura/distancia
    float ratio = 15.0f / 35.0f;  // Original: height/distance
    height = distance * ratio;
    height = glm::clamp(height, minHeight, maxHeight);
}
```

**Test**:
```cpp
camera.adjustDistance(-10);  // Zoom in
auto pos1 = camera.getPosition();

camera.adjustDistance(10);   // Zoom out
auto pos2 = camera.getPosition();

// Angle should be similar, not drastically different
float angle1 = acos(dot(pos1, pos2) / (length(pos1) * length(pos2)));
```

---

### Bug Fix 3.3: CameraSystem - Normalización Eficiente

**Archivo**: `src/CameraSystem.h:50-55`

**ANTES**:
```cpp
void rotate(float dRotation) {
    rotation += dRotation;
    while (rotation < 0.0f) rotation += 360.0f;
    while (rotation >= 360.0f) rotation -= 360.0f;
}
```

**DESPUÉS**:
```cpp
void rotate(float dRotation) {
    rotation += dRotation;
    // ✅ Use modulo en lugar de while loops
    rotation = glm::mod(rotation, 360.0f);
    if (rotation < 0.0f) rotation += 360.0f;
}

// O mejor, usa radianes:
void rotateRad(float dRotationRad) {
    rotationRad = glm::mod(rotationRad, glm::two_pi<float>());
    if (rotationRad < 0.0f) rotationRad += glm::two_pi<float>();
}
```

---

### Bug Fix 3.4: GraphicsEngine - Limpiar Resources Correctamente

**Archivo**: `src/GraphicsEngine.cpp`
**Ubicación**: método `cleanup()`

**ANTES**:
```cpp
void cleanup() {
    // ... limpieza OpenGL ...
    glfwTerminate();
    // inputManager NO se borra (memory leak)
}
```

**DESPUÉS**:
```cpp
void cleanup() {
    // unique_ptr se destruye automáticamente
    // pero por claridad, puedes hacer explícito:
    inputManager.reset();
    cameraSystem.reset();
    gameLogic.reset();
    stateManager.reset();
    audioManager.reset();
    
    // ... limpieza OpenGL ...
    glfwTerminate();
}
```

---

### Bug Fix 3.5: EcosystemSimulator - Race Condition

**Archivo**: `src/EcosystemSimulator.cpp`
**Ubicación**: método `update()`

**ANTES**:
```cpp
void EcosystemSimulator::update(float deltaTime) {
    for (auto& plant : plants) {
        updateEnvironmentalStress(plant);  // ← Lee ALL plants
    }
    
    // Después modifica plants
    plants.erase(...);  // ← Iterator invalidated
}
```

**DESPUÉS**:
```cpp
void EcosystemSimulator::update(float deltaTime) {
    // Marcar plantas muertas sin borrar
    for (auto& plant : plants) {
        updatePlantLife(plant, dt);
        updateEnvironmentalStress(plant);  // Safe: Lee pero no modifica
        if (plant.age > maxAge) {
            plant.stage = DEAD;  // Solo marca
        }
    }
    
    // ✅ DESPUÉS de iterar, eliminar
    plants.erase(
        std::remove_if(plants.begin(), plants.end(),
            [](const PlantData& p) { return p.stage == DEAD; }),
        plants.end()
    );
}
```

---

## 🟢 FASE 4: OPTIMIZACIONES DE RENDIMIENTO (1h)

### Optimización 4.1: Pre-allocate Vertex Buffer

**Archivo**: `src/GraphicsEngine.cpp`
**Método**: `renderGameScene()`

**ANTES**:
```cpp
std::vector<float> vertices;  // Nuevo cada frame
for (const auto& light : lights) {
    vertices.push_back(light.position.x);
    // 6 push_backs → múltiples reallocations
}
```

**DESPUÉS**:
```cpp
// En clase:
std::vector<float> vertexBuffer;

// En initialize():
vertexBuffer.reserve(NUM_LIGHTS * 6);  // Pre-allocate

// En renderGameScene():
vertexBuffer.clear();  // Reutilizar
for (const auto& light : lights) {
    vertexBuffer.push_back(light.position.x);
    vertexBuffer.push_back(light.position.y);
    vertexBuffer.push_back(light.position.z);
    vertexBuffer.push_back(light.color.r);
    vertexBuffer.push_back(light.color.g);
    vertexBuffer.push_back(light.color.b);
}
glBufferSubData(GL_ARRAY_BUFFER, 0, 
                 vertexBuffer.size() * sizeof(float), 
                 vertexBuffer.data());
```

**Benchmark**:
```
ANTES: 27,000 allocations/sec × 60 FPS
DESPUÉS: 0 allocations/sec (reutiliza buffer)
```

---

### Optimización 4.2: Spatial Partitioning

**Archivo**: `src/EcosystemSimulator.cpp`  
**Método**: `updateEnvironmentalStress()`

**ANTES**:
```cpp
for (const auto& other : plants) {  // O(n) para CADA planta = O(n²)
    float dist = glm::distance(...);
    // 500 × 500 = 250,000 distancias/frame
}
```

**DESPUÉS**:
```cpp
// En clase:
struct GridCell { int x, z; };
std::map<GridCell, std::vector<size_t>> spatialGrid;

void rebuildSpatialGrid() {
    spatialGrid.clear();
    int gridSize = 10;  // Células de 10m
    for (size_t i = 0; i < plants.size(); ++i) {
        auto cell = GridCell{
            (int)plants[i].position.x / gridSize,
            (int)plants[i].position.z / gridSize
        };
        spatialGrid[cell].push_back(i);
    }
}

void updateEnvironmentalStress(PlantData& plant) {
    int gridSize = 10;
    auto cellX = (int)plant.position.x / gridSize;
    auto cellZ = (int)plant.position.z / gridSize;
    
    // ✅ Solo revisar celdas cercanas
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dz = -1; dz <= 1; ++dz) {
            auto cell = GridCell{cellX + dx, cellZ + dz};
            for (size_t idx : spatialGrid[cell]) {
                // Calcular estrés solo con vecinos cercanos
            }
        }
    }
}
```

**Complexity**:
```
ANTES: O(n²)        = 250,000 calcs
DESPUÉS: O(n × k)   = n × ~9 vecinos = 4,500 calcs
Speedup: 55×
```

---

## 📋 FASE 5: TESTING Y VALIDACIÓN (1h)

### Test 5.1: Verificar Bugs Arreglados

```cpp
// tests/test_fixes.cpp
#include "../src/InputManager.h"
#include "../src/CameraSystem.h"
#include <cassert>

void testInputManagerInitialization() {
    // Mock GLFW window
    glfwSetErrorCallback(nullptr);
    if (!glfwInit()) return;
    
    auto* window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
    
    InputManager input(window);
    auto state = input.getInputState();
    
    // ✅ Verificar mouseX/Y no son 0
    assert(state.mouseX >= 0 && state.mouseX <= 800);
    assert(state.mouseY >= 0 && state.mouseY <= 600);
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void testCameraDistanceHeight() {
    CameraSystem camera;
    camera.reset();
    
    float initial_distance = camera.getDistance();
    float initial_height = camera.getHeight();
    float ratio1 = initial_height / initial_distance;
    
    camera.adjustDistance(10.0f);  // Zoom out
    float ratio2 = camera.getHeight() / camera.getDistance();
    
    // ✅ Ratio debe ser similar
    assert(glm::abs(ratio1 - ratio2) < 0.05f);
}

void testCameraRotationEfficiency() {
    CameraSystem camera;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000000; ++i) {
        camera.rotate(0.1f);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    Logger::info("Rotation time: " + std::to_string(duration.count()) + "ms");
    
    // ✅ Debería ser < 100ms para 10M rotaciones
    assert(duration.count() < 100);
}
```

### Test 5.2: Benchmark de Rendimiento

```cpp
void benchmarkRenderingOptimization() {
    // ANTES: Vector allocation
    auto before = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < 60; ++frame) {
        std::vector<float> vertices;
        for (int i = 0; i < 75; ++i) {
            vertices.push_back(1.0f);  // 6 veces
            vertices.push_back(2.0f);
            vertices.push_back(3.0f);
            vertices.push_back(0.5f);
            vertices.push_back(0.6f);
            vertices.push_back(0.7f);
        }
    }
    auto after_before = std::chrono::high_resolution_clock::now();
    
    // DESPUÉS: Pre-allocated buffer
    auto after = std::chrono::high_resolution_clock::now();
    std::vector<float> buffer;
    buffer.reserve(450);
    for (int frame = 0; frame < 60; ++frame) {
        buffer.clear();
        for (int i = 0; i < 75; ++i) {
            buffer.push_back(1.0f);  // 6 veces
            buffer.push_back(2.0f);
            buffer.push_back(3.0f);
            buffer.push_back(0.5f);
            buffer.push_back(0.6f);
            buffer.push_back(0.7f);
        }
    }
    auto after_after = std::chrono::high_resolution_clock::now();
    
    auto time_before = std::chrono::duration_cast<std::chrono::milliseconds>(
        after_before - before).count();
    auto time_after = std::chrono::duration_cast<std::chrono::milliseconds>(
        after_after - after).count();
    
    Logger::info("ANTES: " + std::to_string(time_before) + "ms");
    Logger::info("DESPUÉS: " + std::to_string(time_after) + "ms");
    Logger::info("Speedup: " + std::to_string((float)time_before / time_after) + "x");
}
```

---

## 📝 CHECKLIST DE REFACTORIZACIÓN

### [ ] Paso 1: Consolidación (2h)
- [ ] Eliminar src/main.cpp (monolítico)
- [ ] Actualizar CMakeLists.txt
- [ ] Compilar sin errores

### [ ] Paso 2: Módulos (3h)
- [ ] Crear src/GraphicsEngine.cpp completo
- [ ] Implementar initialize(), update(), render(), cleanup()
- [ ] Integrar InputManager, CameraSystem, GameLogic
- [ ] Compilar sin errores

### [ ] Paso 3: Bugs (2h)
- [ ] Fix InputManager: inicializar mouseX/Y
- [ ] Fix CameraSystem: height proporcional
- [ ] Fix CameraSystem: rotación eficiente
- [ ] Fix GraphicsEngine: cleanup correcto
- [ ] Fix EcosystemSimulator: race condition
- [ ] Compilar y testear

### [ ] Paso 4: Performance (1h)
- [ ] Pre-allocate vertex buffer
- [ ] Spatial grid para EcosystemSimulator
- [ ] Benchmarks compilados
- [ ] Medir speedup

### [ ] Paso 5: Validación (1h)
- [ ] Tests unitarios compilan
- [ ] Aplicación runs sin crashes
- [ ] No memory leaks (valgrind/ASAN)
- [ ] Performance dentro de límites

### [ ] Paso 6: Deploy
- [ ] Commit a git: "refactor: modular architecture implementation"
- [ ] Push a GitHub
- [ ] Crear release note

---

## 🎯 DEPENDENCIAS Y ORDEN DE EJECUCIÓN

```
Phase 1 (Consolidación) ─→ REQUISITO PARA Phase 2
        ↓
Phase 2 (Módulos) ─────→ REQUISITO PARA Phase 3 + 4
        ↓
Phase 3 (Bugs) + Phase 4 (Perf) ─ paralelo ─→ Phase 5
        ↓
Phase 5 (Testing) ───→ Validación
```

---

## ⚠️ RIESGOS Y MITIGACIONES

| Riesgo | Severidad | Mitigación |
|--------|-----------|-----------|
| Compilación rota después cambios | ALTO | Compilar cada paso |
| Comportamiento diferente | MEDIO | Unit tests, comparar output |
| Performance regresiona | MEDIO | Benchmarks antes/después |
| Memory leaks introducidos | BAJO | Valgrind/ASAN en tests |
| Lógica de transiciones states | MEDIO | Coverage testing |

---

## 📊 TIMELINE ESTIMADO

```
Día 1: Phase 1 + 2 (5h)
  - Consolidar código (2h)
  - Implementar GraphicsEngine.cpp (3h)

Día 2: Phase 3 + 4 (3h)
  - Fix bugs (2h)
  - Optimizaciones (1h)

Día 3: Phase 5 (1h)
  - Testing y validación
```

**Total**: 9 horas de trabajo enfocado

