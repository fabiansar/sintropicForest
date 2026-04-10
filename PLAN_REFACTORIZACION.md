# 📋 Plan Detallado de Refactorización y Mejoras

## 🎯 Propósito

Este documento proporciona un plan **CONCRETO, PASO A PASO** para mejorar la escalabilidad del proyecto sin romper funcionalidad actual.

---

## 📊 Estado Actual vs Objetivo

### Estado Actual (v1.0)
```
GraphicsEngine.cpp = 700+ líneas
GraphicsEngine gestiona:
├─ Contexto OpenGL
├─ Renderizado
├─ Lógica de entrada
├─ Lógica de juego
├─ Gestión de plantas
└─ Gestión de cámara
```

**Problemas**:
- Difícil agregar features
- Difícil debuggear
- Difícil testear
- Acoplado

### Estado Objetivo (v2.0)
```
GraphicsEngine.cpp = 200 líneas (coordinador)
Responsabilidades distribuidas:
├─ GameManager: Lógica de juego
├─ EntityManager: Gestión de entidades
├─ Renderer: Renderizado
├─ Camera: Gestión de cámara
├─ InputManager: Procesamiento de entrada
├─ EventSystem: Comunicación desacoplada
└─ PlantFactory: Creación de plantas
```

**Beneficios**:
- Fácil agregar features
- Fácil debuggear
- Posible testear
- Desacoplado

---

## 🏁 Plan de Implementación

### FASE 1: Extracción de Componentes (2-3 horas)

**Objetivo**: Separar responsabilidades sin cambiar funcionamiento

#### PASO 1A: Crear `Camera.h/cpp` (30 min)
**Archivo nuevo**: `src/Camera.h`

```cpp
class Camera {
private:
    glm::vec3 target;
    float rotation;
    float distance;
    
public:
    void orbit(float angleDelta);
    void pan(const glm::vec3& direction);
    glm::vec3 getPosition() const;
    glm::mat4 getViewMatrix() const;
    glm::vec3 getRayFromMousePos(double mx, double my) const;
};
```

**Mover de GraphicsEngine**:
- Variables: `cameraPos`, `cameraTarget`, `cameraRotation`
- Métodos: Lógica de cálculo de cámara
- Función: `getRaycastHit()` → `Camera::getRayFromMousePos()`

**Compilar y verificar**: Debe compilar sin cambios de comportamiento

---

#### PASO 1B: Crear `Renderer.h/cpp` (30 min)
**Archivo nuevo**: `src/Renderer.h`

```cpp
class Renderer {
private:
    unsigned int shaderProgram;
    unsigned int terrainShaderProgram;
    
public:
    void renderPlants(const std::vector<Light>& plants);
    void renderTerrain();
    void renderUI();
};
```

**Mover de GraphicsEngine**:
- Variables: `VAO`, `VBO`, `terrainVAO`, `terrainVBO`
- Métodos: `renderGameScene()`, `renderTerrain()` (OpenGL calls)

**Compilar y verificar**

---

#### PASO 1C: Crear `InputManager.h/cpp` (30 min)
**Archivo nuevo**: `src/InputManager.h`

```cpp
class InputManager {
private:
    GLFWwindow* window;
    
public:
    struct InputState {
        bool wPressed, aPressed, sPressed, dPressed;
        bool qPressed, ePressed;
        bool leftMousePressed;
        glm::vec2 mousePos;
    };
    
    InputState processInput();
};
```

**Mover de GraphicsEngine**:
- Métodos: `handleInput()` → Separar lectura de lógica

**Compilar y verificar**

---

#### PASO 1D: Crear `PlantFactory.h` (15 min)
**Archivo nuevo**: `src/PlantFactory.h`

```cpp
class PlantFactory {
public:
    static Light createPlant(const glm::vec3& position);
};
```

**Mover de GraphicsEngine**:
- Lógica de `addRandomLight()` al factory

**Compilar y verificar**

---

### FASE 2: Refactorizar GraphicsEngine como Coordinador (1 hora)

**Objetivo**: GraphicsEngine ahora COORDINA en lugar de IMPLEMENTAR

#### PASO 2A: Reducir GraphicsEngine.cpp
```cpp
// ANTES:
void GraphicsEngine::handleInput() {
    if (glfwGetKey(...)) { ... }  // 50 líneas de lógica
}

// DESPUÉS:
void GraphicsEngine::handleInput() {
    InputState state = inputManager.processInput();
    gameLogic.handleInput(state);
}
```

**Proceso**:
1. Reemplazar lógica con llamadas a componentes
2. GraphicsEngine pasa a ~250 líneas
3. Compilar y probar

---

#### PASO 2B: Crear `GameManager.h`
```cpp
class GameManager {
private:
    std::vector<Light> plants;
    EntityManager entities;
    
public:
    void handleInput(InputState state);
    void update(float deltaTime);
};
```

**Mover**:
- Lógica de `update()` relacionada a plantas
- Array de `lights` (plantas)

---

### FASE 3: Sistema de Entidades (2 horas)

#### PASO 3A: Implementar `Entity.h` (archivo ya existe)
- Usado como base

#### PASO 3B: Crear `Plant.h/cpp` heredando de Entity
```cpp
class Plant : public Entity {
    PlantType type;
    
    void update(float deltaTime) override;
    void render(Renderer* renderer) override;
};
```

#### PASO 3C: Crear `EntityManager.h/cpp`
```cpp
class EntityManager {
    std::vector<std::unique_ptr<Entity>> entities;
    
public:
    void add(std::unique_ptr<Entity> e);
    void updateAll(float dt);
    void renderAll(Renderer* r);
};
```

**Compilar y probar**

---

### FASE 4: Sistema de Eventos (1 hora)

#### PASO 4A: Crear `EventSystem.h`
```cpp
enum class EventType {
    PLANT_ADDED,
    PLANT_REMOVED,
    CAMERA_MOVED,
    GAME_STARTED
};

class EventSystem {
    std::map<EventType, std::vector<std::function<void()>>> listeners;
    
public:
    void subscribe(EventType type, std::function<void()> callback);
    void emit(EventType type);
};
```

#### PASO 4B: Integrar en GameManager
```cpp
void GameManager::plantAt(const glm::vec3& pos) {
    auto plant = PlantFactory::createPlant(pos);
    plants.push_back(plant);
    eventSystem.emit(EventType::PLANT_ADDED);  // ← Notificar
}
```

---

## 🔧 Cambios Menores Recomendados

### 1. Mejorar Config.h
✅ Ya hecho en última sesión

### 2. Agregar Logging
**Archivo**: `src/Logger.h` (ya creado como propuesta)

**Uso**:
```cpp
// En initialize():
Logger::init("engine.log");
Logger::info("Engine initialized");

// En addPlant():
Logger::debug("Plant added at position");
```

### 3. Agregar Comentarios TODO
En GraphicsEngine.cpp, marcar dónde iría cada componente:

```cpp
void GraphicsEngine::initialize() {
    // TODO: Mover a Renderer::initialize()
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // TODO: Mover a Camera constructor
    cameraPos = glm::vec3(0.0f, CAMERA_HEIGHT, 0.0f);
}
```

---

## 📈 Métricas de Éxito

### Antes de Refactorización
- GraphicsEngine.cpp: 700+ líneas
- Acoplamiento: Alto
- Test coverage: 0%
- Tiempo para nueva feature: 30 min

### Después de Refactorización (Fase 4 completa)
- GraphicsEngine.cpp: 200 líneas
- Acoplamiento: Bajo
- Test coverage: 50%+
- Tiempo para nueva feature: 5 min

---

## 🎯 Siguientes Pasos Después de Refactorización

### Nivel 1: Features Fáciles (30 min cada)
- [ ] Agregar zoom (scroll wheel)
- [ ] Agregar herramienta delete (botón derecho)
- [ ] Agregar sistema de sonido

### Nivel 2: Features Medias (2 horas cada)
- [ ] Ciclo de vida de plantas (edad, muerte)
- [ ] Terreno procedural (Perlin noise)
- [ ] Undo/Redo

### Nivel 3: Features Avanzadas (8 horas cada)
- [ ] Ecosistema (propagación de semillas)
- [ ] Multijugador
- [ ] Editor de tipos personalizados

---

## ✅ Checklist de Implementación

### Fase 1 - Extracción
- [ ] Camera.h creado y funciona
- [ ] Renderer.h creado y funciona
- [ ] InputManager.h creado y funciona
- [ ] PlantFactory.h creado y funciona
- [ ] GraphicsEngine aún funciona igual

### Fase 2 - Coordinación
- [ ] GameManager.h creado
- [ ] GraphicsEngine refactorizado a coordinador
- [ ] GraphicsEngine.cpp < 300 líneas

### Fase 3 - Entidades
- [ ] Entity.h implementado (ya existe)
- [ ] Plant.h heredando de Entity
- [ ] EntityManager.h funcional
- [ ] Todas las plantas funcionan igual

### Fase 4 - Eventos
- [ ] EventSystem.h implementado
- [ ] Eventos emitidos en lugares clave
- [ ] Ejemplo de suscripción funciona

### Documentación
- [ ] README.md actualizado (✅ ya hecho)
- [ ] ARQUITECTURA_ESCALABLE.md creado (✅ ya hecho)
- [ ] EXTENSIBILIDAD_Y_ESCALABILIDAD.md creado (✅ ya hecho)
- [ ] GUIA_PARA_NUEVOS_DESARROLLADORES.md creado (✅ ya hecho)
- [ ] Nuevas clases documentadas en headers

---

## 📝 Notas Importantes

### Compilación Incremental
Después de cada PASO:
```bash
.\build.ps1
```

Si no compila:
- Verificar `#include` en headers
- Verificar que métodos tengan implementación
- Usar `forward declarations` si necesario

### Testing
Después de cada FASE:
```bash
.\build\3DEngine.exe
```

Verificar:
- Splash screen aparece
- Menú funciona
- Puedo plantar plantas
- No hay crash

### No Romper Funcionalidad
El juego DEBE funcionar idénticamente después de cada cambio.
Si funciona diferente, deshacer cambios y revisar.

---

## 🎓 Principios durante Refactorización

### ✅ HACER
- Cambios pequeños, compilables
- Documentar en headers
- Pruebas después de cada fase
- Mantener funcionalidad

### ❌ EVITAR
- Cambios grandes = riesgo de romper
- Cambios sin compilar
- Eliminar código sin reemplazarlo
- Ignorar warnings del compilador

---

## 📞 Troubleshooting

### "No member named..." error
**Causa**: Variable en clase incorrecta
**Solución**: Verificar que `#include` esté en lugar correcto

### "Undefined reference" error
**Causa**: Función declarada pero no implementada
**Solución**: Agregar implementación en .cpp

### Juego compila pero no funciona igual
**Causa**: Lógica movida incorrectamente
**Solución**: 
1. Revertir último cambio
2. Revisar implementación
3. Comparar con código original

### Performance bajó después de cambios
**Causa**: Cambio en renderizado o actualización
**Solución**: Revisar Renderer y EntityManager

---

## 📚 Referencias

- [ARQUITECTURA_ESCALABLE.md](ARQUITECTURA_ESCALABLE.md) — Patrones
- [EXTENSIBILIDAD_Y_ESCALABILIDAD.md](EXTENSIBILIDAD_Y_ESCALABILIDAD.md) — Features futuras
- [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) — Código actual

---

**Versión**: 1.0  
**Estado**: Propuesta  
**Duración estimada**: 6-8 horas  
**Compilaciones esperadas**: ~30-40  
**Tests esperados**: 4 (una por fase)
