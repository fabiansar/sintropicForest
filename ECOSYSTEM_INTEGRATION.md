# 🔗 Integración de EcosystemSimulator con Sistemas Existentes

## 📋 Cambios Necesarios

### 1. Config.h - Agregar nuevos tipos

```cpp
// Agregar a Config.h:

// Enumeración de etapas de vida
enum PlantLifeStage {
    SEEDLING,
    JUVENILE,
    ADULT,
    SENESCENT,
    DEAD
};

// Estructura mejorada de planta con datos ecológicos
struct PlantData {
    // ... (ver EcosystemSimulator.h)
};

// Enumeración de estadísticas
enum EcosystemMetric {
    METRIC_GRASS_COUNT,
    METRIC_BUSH_COUNT,
    METRIC_TREE_COUNT,
    METRIC_AVERAGE_HEALTH,
    METRIC_BIODIVERSITY,
    METRIC_BIOMASS
};
```

### 2. GameLogic.h - Integrar EcosystemSimulator

```cpp
#include "EcosystemSimulator.h"

class GameLogic {
private:
    EcosystemSimulator ecosystem;
    std::vector<Plant> plants;  // Mantener para compatibilidad renderizado
    
    // ... resto de variables
    
    void syncEcosystemToPlants();
    void syncPlantsToEcosystem();

public:
    GameLogic();
    
    void reset() override {
        ecosystem.initialize();
        plants.clear();
    }
    
    void update(float deltaTime) override {
        ecosystem.update(deltaTime);
        syncEcosystemToPlants();  // Actualizar plantas para renderizado
    }
    
    bool addPlant(const glm::vec3& position, PlantType type) {
        return ecosystem.addPlant(position, type);
    }
    
    const EcosystemSimulator& getEcosystem() const {
        return ecosystem;
    }
    
    // Obtener estadísticas
    EcosystemSimulator::EcosystemStats getEcosystemStats() const {
        return ecosystem.getStatistics();
    }
};
```

### 3. GameLogic.cpp - Implementación de sincronización

```cpp
void GameLogic::syncEcosystemToPlants() {
    plants.clear();
    
    const auto& ecoPlants = ecosystem.getPlants();
    for (const auto& ecoPlant : ecoPlants) {
        Plant plant;
        plant.position = ecoPlant.position;
        plant.type = ecoPlant.type;
        plant.createdTime = ecoPlant.createdTime;
        
        // Scale visual según edad y health
        float scale = (ecoPlant.currentHeight / 2.0f) * ecoPlant.healthFactor;
        plant.scale = glm::clamp(scale, 0.1f, 1.0f);
        
        // Color basado en salud
        if (ecoPlant.healthFactor > 0.8f) {
            plant.color = PLANT_COLOR_HEALTHY;  // Verde vibrante
        } else if (ecoPlant.healthFactor > 0.5f) {
            plant.color = PLANT_COLOR_STRESSED; // Verde pálido
        } else {
            plant.color = PLANT_COLOR_DYING;    // Marrón
        }
        
        plants.push_back(plant);
    }
}

void GameLogic::syncPlantsToEcosystem() {
    // Si el usuario agrega plantas desde UI (deprecado con ecosistema)
    // Sincronizar aquí
}
```

### 4. GraphicsEngine.cpp - Mostrar estadísticas ecológicas

```cpp
void GraphicsEngine::renderGameScene() {
    // ... código existente ...
    
    // Obtener estadísticas del ecosistema
    auto ecoStats = gameLogic->getEcosystemStats();
    
    ImGui::Begin("Game Info", nullptr, 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    
    ImGui::Text("=== ECOSYSTEM STATUS ===");
    ImGui::Text("Species Distribution:");
    ImGui::Text("  🌾 Grass: %d", ecoStats.grazeCount);
    ImGui::Text("  🌳 Bush: %d", ecoStats.bushCount);
    ImGui::Text("  🌲 Tree: %d", ecoStats.treeCount);
    ImGui::Separator();
    
    ImGui::Text("Ecosystem Health:");
    ImGui::Text("  Avg Health: %.2f", ecoStats.averageHealth);
    ImGui::ProgressBar(ecoStats.averageHealth);
    ImGui::Text("  Biodiversity: %.2f", ecoStats.biodiversity);
    ImGui::Text("  Biomass: %.1f", ecoStats.biomass);
    
    ImGui::Separator();
    ImGui::Text("Environmental Conditions:");
    
    auto& eco = gameLogic->getEcosystem();
    ImGui::SliderFloat("Global Light", 
        const_cast<float*>(&eco.globalLight), 0.0f, 1.0f);
    ImGui::SliderFloat("Global Water", 
        const_cast<float*>(&eco.globalWater), 0.0f, 1.0f);
    ImGui::SliderFloat("Global Nutrients", 
        const_cast<float*>(&eco.globalNutrients), 0.0f, 1.0f);
    ImGui::SliderFloat("Simulation Speed", 
        const_cast<float*>(&eco.simulationSpeed), 0.1f, 5.0f);
    
    ImGui::End();
}
```

### 5. CameraSystem.h - Centrar en ecosistema

```cpp
void resetToEcosystem(const EcosystemSimulator& ecosystem) {
    // Centrar cámara en promedio de plantas
    glm::vec3 ecoCenter = ecosystem.getAveragePosition();
    target = ecoCenter;
    rotation = 0.0f;
    distance = 40.0f;
    height = 20.0f;
}
```

---

## 🎨 Cambios Visuales Recomendados

### Color dinámico basado en salud

```glsl
// En fragmentShader de plantas:
vec3 plantColor = mix(
    PLANT_COLOR_DYING,
    PLANT_COLOR_HEALTHY,
    plant.healthFactor
);
```

### Tamaño dinámico basado en edad

```cpp
// En renderizacion:
float plantSize = baseSize * plant.currentHeight / speciesMaxHeight;
glPointSize(plantSize);
```

### Animación de crecimiento

```glsl
// En vertex shader:
vPosition.y += sin(time * growthRate) * growth;
```

---

## 📊 UI de Monitoreo Ecológico

### Panel Principal

```
┌─────────────────────────────────┐
│ ECOSYSTEM STATUS                │
├─────────────────────────────────┤
│ 🌾 Grass:     234  ████████░░░░ │
│ 🌳 Bush:      89   ░░░░░░░░░░░░ │
│ 🌲 Tree:      12   ░░░░░░░░░░░░ │
├─────────────────────────────────┤
│ Avg Health:   0.75 ███████░░░░░░ │
│ Biodiversity: 1.23             │
│ Biomass:      347.2            │
├─────────────────────────────────┤
│ ☀️  Light:   0.80 ████████░░░░░░ │
│ 💧 Water:   0.60 ██████░░░░░░░░ │
│ 🌱 Nutrients: 0.50 █████░░░░░░░░░ │
│ ⚡ Sim Speed: 1.0x ─────────────── │
└─────────────────────────────────┘
```

### Panel de Detalles (Click en planta)

```
Plant Details
├─ Type: Tree
├─ Age: 45.3s / 180s (25%)
├─ Health: 0.92
├─ Stage: ADULT
├─ Height: 5.2m / 6.0m
├─ Seeds: 12
└─ Stress: 0.15
```

---

## 🔄 Flujo de Datos

```
User Input (Click)
    ↓
GameLogic::addPlant()
    ↓
EcosystemSimulator::addPlant()
    ↓
PlantData creada (SEEDLING)
    ↓
Cada frame:
    EcosystemSimulator::update()
    ├─ Actualizar edad, stage, health
    ├─ Producir semillas
    ├─ Evaluar stress
    ├─ Revisar sucesión
    └─ Dispersar semillas
    ↓
GameLogic::syncEcosystemToPlants()
    ↓
Renderizar con GraphicsEngine
```

---

## ⚙️ Configuración Recomendada

### Para modo observador (sandbox)
```cpp
// Sin límite de plantas
maxPlants = 5000;

// Simulación lenta para ver detalles
simulationSpeed = 0.5f;

// Condiciones óptimas
globalLight = 0.8f;
globalWater = 0.7f;
globalNutrients = 0.6f;
```

### Para modo de desafío
```cpp
// Escasez de recursos
globalLight = 0.4f;
globalWater = 0.3f;
globalNutrients = 0.2f;

// Simulación rápida
simulationSpeed = 2.0f;

// Límite realista
maxPlants = 300;
```

---

## ✅ Checklist de Integración

- [ ] Agregar EcosystemSimulator.h y .cpp a CMakeLists.txt
- [ ] Copiar tipos a Config.h
- [ ] Integrar EcosystemSimulator en GameLogic
- [ ] Implementar syncEcosystemToPlants()
- [ ] Actualizar renderGameScene() con stats
- [ ] Agregar UI de monitoreo
- [ ] Testar dinámicas (sucesión, dispersión, etc.)
- [ ] Optimizar performance si es necesario
- [ ] Documentar parámetros en Config

---

## 🐛 Testing Recomendado

### Test 1: Ciclo de Vida Básico
```
1. Plantar hierba
2. Observar: crecimiento gradual hasta ADULT
3. Verificar: producción de semillas
4. Confirmar: muerte después de 30s
```

### Test 2: Sucesión Ecológica
```
1. Plantar múltiples hierbas
2. Plantear árbol cerca
3. Observar: hierbas se convierten en arbustos
4. Esperar: arbustos se convierten en árboles
```

### Test 3: Estrés Ambiental
```
1. Reducir globalLight a 0.1
2. Observar: plantas empalidecen y mueren
3. Aumentar light
4. Verificar: recuperación y revitalización
```

---

**Versión**: 1.0  
**Última actualización**: 2026-04-11  
**Estado**: Listo para implementación
