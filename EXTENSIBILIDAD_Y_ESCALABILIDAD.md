# 🚀 Guía de Extensibilidad y Escalabilidad

## 📋 Tabla de Contenidos
1. [Puntos Clave de Extensión](#puntos-clave-de-extensión)
2. [Nuevas Características Sugeridas](#nuevas-características-sugeridas)
3. [Mejoras de Rendimiento](#mejoras-de-rendimiento)
4. [Roadmap de Desarrollo](#roadmap-de-desarrollo)
5. [Casos de Uso Comunes](#casos-de-uso-comunes)

---

## 🎯 Puntos Clave de Extensión

### 1. **Sistema de Plantas (PRIORITARIO)**

#### Versión Actual (Simple)
- 3 tipos: Grass, Bush, Tree
- Propiedades: posición, color, tipo
- Generación: aleatoria 80/15/5

#### Extensiones Propuestas

**A. Ciclo de Vida de Plantas**
```cpp
enum class PlantState {
    SEEDLING,    // Recién plantado (0-2 segundos)
    GROWING,     // Creciendo (2-5 segundos)
    MATURE,      // Adulto (5+ segundos)
    FLOWERING,   // Floreciendo (posibilidad de reproducción)
    DEAD         // Muerto
};

class Plant : public Entity {
    PlantState state;
    float age;
    
    void update(float deltaTime) override {
        age += deltaTime;
        updateState();
        updateVisuals();
    }
};
```

**B. Sistema de Crecimiento**
```cpp
struct PlantStats {
    float growthRate;        // Velocidad de crecimiento
    float maxHeight;         // Altura máxima
    float currentHeight;     // Altura actual
    int maxAge;              // Edad máxima
};
```
**Beneficio**: Las plantas crecen visualmente, envejecen, mueren.

**C. Interacción Planta-Planta**
```cpp
// Competencia por recursos
void updateNutrients(float deltaTime) {
    float adjacent = countNearbyPlants(5.0f);
    nutrientLevel -= adjacent * 0.1f;
}

// Propagación de semillas
void spreadSeeds() {
    if (age > maxAge * 0.7f && nutrients > threshold) {
        // Generar nuevas plantas cercanas
    }
}
```
**Beneficio**: Ecosistema dinámico, comportamiento emergente.

**D. Diferentes Biomas**
```cpp
enum class BiomeType {
    TEMPERATE,   // Hierba, arbustos, árboles
    DESERT,      // Cactus, arbustos pequeños
    TROPICAL,    // Árboles grandes, flores
    SNOW         // Pinos, plantas pequeñas
};

// Cada bioma tiene configuración diferente
struct BiomeConfig {
    std::vector<PlantType> availablePlants;
    glm::vec3 baseTerrainColor;
    WeatherData weather;
};
```

---

### 2. **Sistema de Terreno (INTERMEDIARIO)**

#### Versión Actual
- Plano simple (flat mesh)
- 100x100 unidades
- Grid de 50x50 resolución

#### Extensiones Propuestas

**A. Terreno Procedural 3D**
```cpp
class Terrain {
private:
    PerlinNoise noiseGenerator;
    HeightMap heightMap;
    TextureManager textures;
    
public:
    void generateWithPerlin(int scale, float frequency) {
        for (int z = 0; z < resolution; z++) {
            for (int x = 0; x < resolution; x++) {
                float height = noiseGenerator.perlin(x, z, frequency);
                heightMap.set(x, z, height);
            }
        }
    }
};
```

**B. Navegación en Terreno Irregular**
```cpp
// Pathfinding en terreno 3D
class Pathfinder {
public:
    std::vector<glm::vec3> findPath(const glm::vec3& start, 
                                    const glm::vec3& goal);
};
```

**C. Erosión y Deformación**
```cpp
void erodeTerrain(const glm::vec3& point, float radius) {
    // Simular efecto de pisoteo, agua, etc.
}
```

---

### 3. **Interactividad Avanzada (INTERMEDIARIO)**

#### Versión Actual
- Click para plantar
- ESC para volver al menú

#### Extensiones Propuestas

**A. Herramientas de Edición**
```cpp
enum class ToolType {
    PLANT,      // Plantar (actual)
    REMOVE,     // Eliminar plantas
    WATER,      // Regar (aumenta crecimiento)
    FERTILIZE,  // Fertilizar
    HARVEST,    // Cosechar
    BURN        // Eliminar rápidamente
};

class Tool {
    areaOfEffect;  // Tamaño del efecto
    strength;      // Intensidad
    
    virtual void apply(glm::vec3 pos);
};
```

**B. Modos de Selección**
```cpp
// Seleccionar múltiples plantas
std::vector<Plant*> selection;
void selectInRect(glm::vec3 from, glm::vec3 to);
void deleteSelected();
void copySelected();
void pasteSelected();
```

**C. Undo/Redo**
```cpp
class UndoSystem {
    std::queue<Action> history;
    
    void undo();
    void redo();
};
```

---

### 4. **Sistema de Cámara Mejorado (FÁCIL)**

#### Versión Actual
- Órbita fija alrededor de objetivo
- Q/E para rotación
- WASD para movimiento

#### Extensiones Propuestas

**A. Zoom Suave**
```cpp
void updateZoom(float scrollWheel) {
    targetDistance = glm::clamp(targetDistance + scrollWheel, 5.0f, 100.0f);
    // Interpolación suave entre distancias
}
```

**B. Vistas Predefinidas**
```cpp
enum class ViewPreset {
    TOP_DOWN,
    ISO_FRONT,
    ISO_BACK,
    SIDE
};

void switchToPreset(ViewPreset preset) {
    targetRotation = presetRotations[preset];
    targetDistance = presetDistances[preset];
    // Transición suave
}
```

**C. Modo Libre 3D**
```cpp
// Al presionar V, pasar a FPS-like para inspeccionar
void toggleFreeCamera();
```

---

### 5. **Sistema de Materiales (INTERMEDIO)**

#### Extensión Propuesta

**A. Shaders Avanzados**
```cpp
class Material {
    Shader shader;
    glm::vec3 albedo;
    float metallic;
    float roughness;
    Texture normalMap;
    Texture aoMap;
    
    void bind();
};
```

**B. Animación de Plantas**
```cpp
// Shader con sine wave para balanceo
const char* plantVertexShader = R"(
    ...
    vPos.xyz += wind * sin(uTime * frequency) * amplitude;
    ...
)";
```

---

### 6. **Sistema de Sonido (FÁCIL)**

#### Extensión Propuesta

**A. Audio Manager**
```cpp
class AudioManager {
    std::map<std::string, Sound> sounds;
    std::map<std::string, Music> music;
    
    void playSound(const std::string& name);
    void playMusic(const std::string& name, bool loop);
};
```

**B. Eventos de Sonido**
```cpp
eventSystem.subscribe(EventType::PLANT_ADDED, [](const Event& e) {
    audioManager.playSound("plant_place");
});
```

---

### 7. **Sistema de Guardado/Carga (INTERMEDIO)**

#### Extensión Propuesta

**A. Serialización de Mundo**
```cpp
class World {
    void save(const std::string& filename);
    void load(const std::string& filename);
    
private:
    std::vector<Plant> plants;
    Terrain terrain;
};

// JSON o Binary format
// {
//     "version": 1,
//     "plants": [
//         {"type": "TREE", "x": 10, "z": 20, "age": 5.2},
//         {"type": "GRASS", "x": 11, "z": 21, "age": 2.1}
//     ]
// }
```

---

### 8. **Sistema de Estadísticas/UI (FÁCIL)**

#### Extensión Propuesta

**A. Panel de Información Mejorado**
```cpp
class StatsPanel {
    void render() {
        ImGui::Text("Total Plants: %d", entityManager.getCount());
        ImGui::ProgressBar(healthAverage, "Average Health");
        ImGui::PlotLines("Plant Count Over Time", ...);
        
        // Gráficos de distribución
        ImGui::BarChart(plantDistribution);
    }
};
```

**B. Minimap**
```cpp
class Minimap {
    void render(const glm::vec3& cameraTarget) {
        // Viewport pequeño mostrando vista superior
    }
};
```

---

## 📈 Nuevas Características Sugeridas

### Tier 1: FÁCIL (0.5-1 hora)
- [ ] Sistema de Zoom (scroll wheel)
- [ ] Teclas numéricas para vistas predefinidas
- [ ] Sistema de Sonido básico
- [ ] Panel de estadísticas mejorado
- [ ] Herramienta DELETE (click derecho)

### Tier 2: MEDIO (2-4 horas)
- [ ] Ciclo de vida de plantas (seedling → mature → dead)
- [ ] Terreno con Perlin noise
- [ ] Undo/Redo
- [ ] Serialización (save/load)
- [ ] Animación de plantas en shader
- [ ] Sistema de biomas iniciales

### Tier 3: DIFÍCIL (4-8 horas)
- [ ] Propagación de semillas y ecosistema
- [ ] Pathfinding 3D
- [ ] Interacción planta-planta
- [ ] Modo libre 3D FPS
- [ ] Editor de tipos de plantas configurables
- [ ] Sistema de partículas

### Tier 4: MUY DIFÍCIL (8+ horas)
- [ ] Multijugador en línea
- [ ] Físicas avanzadas
- [ ] IA de plantas/criaturas
- [ ] Sistema de misiones/objetivos
- [ ] Procedural world generation
- [ ] Modding API

---

## ⚡ Mejoras de Rendimiento

### Nivel 1: Optimizaciones Actuales
- [ ] Instancing para plantas del mismo tipo
- [ ] Culling (no renderizar fuera de cámara)
- [ ] Level of Detail (LOD)

### Nivel 2: Optimizaciones Avanzadas
- [ ] Spatial hashing para búsquedas rápidas
- [ ] Object pooling para plantas
- [ ] GPU compute para simulaciones
- [ ] Multithreading

### Nivel 3: Arquitectura Escalable
- [ ] Sistema de chunking (dividir mundo)
- [ ] Streaming de datos
- [ ] Compresión de datos
- [ ] Streaming de texturas

---

## 🗺️ Roadmap de Desarrollo

### Versión 1.0 (ACTUAL)
```
✅ 3 tipos de plantas (Grass, Bush, Tree)
✅ Terreno simple
✅ Generación aleatoria con probabilidades
✅ Sistema de clics para plantar
✅ Cámara isométrica
✅ UI con ImGui
```

### Versión 1.1 (2 horas)
```
🔄 Zoom y vistas predefinidas
🔄 Herramienta eliminar (botón derecho)
🔄 Estadísticas mejoradas
🔄 Sonidos de plantas
```

### Versión 1.2 (6 horas)
```
🔄 Ciclo de vida de plantas
🔄 Terreno con Perlin noise
🔄 Undo/Redo
🔄 Sistema de biomas básico
🔄 Animación de viento en shaders
```

### Versión 2.0 (24+ horas)
```
🔄 Ecosystem completo (propagación de semillas)
🔄 Editor de tipos de plantas
🔄 Multijugador local
🔄 Misiones y objetivos
🔄 Sistema de guardado/carga robusto
```

### Versión 3.0 (Future)
```
🔄 Mundo procedural masivo
🔄 Multijugador en línea
🔄 Modding API
🔄 Plataformas móviles/consolas
```

---

## 💡 Casos de Uso Comunes

### Caso 1: Agregar Nuevo Tipo de Planta

**Requisito**: Agregar "Flower" (Flor) que aparece 5% de las veces

**Pasos**:
```cpp
// 1. En Config.h:
enum class PlantType { GRASS, BUSH, TREE, FLOWER };
const float PLANT_PROBABILITY_FLOWER = 0.05f;

// 2. En PlantFactory.h:
case PlantType::FLOWER:
    return std::make_unique<Plant>(
        PlantType::FLOWER,
        glm::vec3(1.0f, 0.5f, 0.7f),  // Color magenta
        4  // Tamaño
    );

// 3. Compilar y listo ✓
```

### Caso 2: Cambiar Probabilidades

**Requisito**: 50% Grass, 30% Bush, 20% Tree

**Pasos**:
```cpp
// En Config.h:
const float PLANT_PROBABILITY_GRASS = 0.50f;
const float PLANT_PROBABILITY_BUSH = 0.30f;
const float PLANT_PROBABILITY_TREE = 0.20f;

// Compilar ✓
```

### Caso 3: Agregar Herramienta Nueva

**Requisito**: "Fertilizer" que aumenta crecimiento

**Pasos**:
```cpp
// 1. En Config.h:
enum class ToolType { PLANT, REMOVE, FERTILIZE };

// 2. Crear clase Tool:
class FertilizerTool : public Tool {
    void apply(Plant& plant) override {
        plant.nutrients *= 1.5f;
        plant.growthRate *= 1.2f;
    }
};

// 3. En InputManager:
if (rightClick) {
    currentTool = std::make_unique<FertilizerTool>();
}

// 4. Compilar ✓
```

### Caso 4: Cambiar Color del Terreno

**Requisito**: Terreno con tonos azulados (playa/agua)

**Pasos**:
```cpp
// En Shaders.h:
// Cambiar colores en fragmentShaderSource
const char* terrainFragmentShader = R"(
    ...
    vec3 baseColor = vec3(0.5, 0.7, 0.9);  // Azul agua
    vec3 darkColor = vec3(0.3, 0.5, 0.8);  // Azul más oscuro
    ...
)";
```

---

## 🎓 Máximas de Escalabilidad

### ✅ HACER
- **Separar responsabilidades**: Cada clase, una función
- **Usar abstracciones**: Clases base, interfaces
- **Configuración centralizada**: Todo en Config.h
- **Datos en estruturas**: Fácil de serializar
- **Extensión por subclases**: No modificar código existente

### ❌ EVITAR
- **Clases monolíticas**: >300 líneas = refactorizar
- **Lógica esparcida**: Todo en `update()` es malo
- **Magic numbers**: Siempre constantes nombradas
- **Dependencias circular**: A → B → A = problema
- **Modificar código existente**: Extensión, no modificación

---

## 📊 Beneficios de Escalabilidad

| Característica | Costo Inicial | Costo por Adición |
|---|---|---|
| **Sin arquitectura** | 1 hora | 30 min / característica |
| **Con arquitectura** | 3 horas | 5-10 min / característica |

**En 100 características**: Arquitectura es **10x más eficiente**

---

## ✅ Checklist de Implementación

### Antes de Agregar Característica
- [ ] ¿Cabe en una clase existente?
- [ ] ¿Puedo hacerlo sin modificar código antiguo?
- [ ] ¿Tengo una prueba para validar?
- [ ] ¿Está documentado?

### Después de Agregar
- [ ] ¿Compila sin warnings?
- [ ] ¿Funciona correctamente?
- [ ] ¿Está integrado con el resto?
- [ ] ¿Otros pueden entender el código?

---

## 🔗 Referencias Internas
- [ARQUITECTURA_ESCALABLE.md](ARQUITECTURA_ESCALABLE.md)
- [GUIA_PARA_NUEVOS_DESARROLLADORES.md](GUIA_PARA_NUEVOS_DESARROLLADORES.md)
- [Config.h](src/Config.h)

---

**Versión**: 1.0  
**Última actualización**: 2026-04-11  
**Punto de Contacto**: Escalability Guide  
**Estado**: Propuesta Ejecutiva
