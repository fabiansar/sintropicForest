# ⚙️ Configuración - Parámetros & Ajustes

**Archivo**: `src/Config.h`  
**Tipo**: Header de configuración centralizada

---

## 🎮 Controles de Cámara

```cpp
// Velocidades de movimiento
const float CAMERA_ROTATION_SPEED = 3.0f;        // Q/E rotación (rad/s)
const float CAMERA_MOVEMENT_SPEED = 20.0f;       // WASD velocidad (u/s)
const float CAMERA_SLOW_MOVEMENT = 5.0f;         // Flechas velocidad (u/s)

// Parámetros de órbita
const float CAMERA_DISTANCE = 30.0f;             // Distancia del target (u)
const float CAMERA_HEIGHT = 20.0f;               // Altura relativa (u)
```

### Ajuste Rápido

**Para más rápido**:
```cpp
CAMERA_ROTATION_SPEED = 5.0f;       // Rotación 2x más rápida
CAMERA_MOVEMENT_SPEED = 40.0f;      // WASD 2x más rápido
```

**Para más controlable**:
```cpp
CAMERA_ROTATION_SPEED = 1.5f;       // Rotación más suave
CAMERA_MOVEMENT_SPEED = 10.0f;      // WASD más lento
```

---

## 🌱 Probabilidades de Plantas

**Dónde ajustar**: ESC → SETTINGS (en-juego)

```cpp
// Valores por defecto (en Config.h)
float plantProbabilityTree = 0.30f;   // 30% árboles
float plantProbabilityBush = 0.15f;   // 15% arbustos
float plantProbabilityGrass = 0.55f;  // 55% hierba (residuo = ~55%)

// Escala de tamaño
float plantSizeMultiplier = 1.0f;     // Multiplicador global
```

### Cómo Funcionan las Probabilidades

```cpp
// En tiempo de ejecución:
float rand = getRandomFloat(0.0f, 1.0f);

if (rand < plantProbabilityTree) {
    type = PlantType::TREE;              // 0-30%
} else if (rand < plantProbabilityTree + plantProbabilityBush) {
    type = PlantType::BUSH;              // 30-45%
} else {
    type = PlantType::GRASS;             // 45-100%
}
```

**Nota**: Los porcentajes NO necesitan sumar 100%.  
Lo que no cae en TREE/BUSH automáticamente es GRASS.

### Ejemplos de Configuración

**Bosque Denso**:
```cpp
plantProbabilityTree = 0.60f;   // Muchos árboles
plantProbabilityBush = 0.20f;   // Algunos arbustos
// Resto = hierba
```

**Pradera**:
```cpp
plantProbabilityTree = 0.05f;   // Pocos árboles
plantProbabilityBush = 0.10f;   // Algunos arbustos
// Resto = mucha hierba
```

**Balanceado**:
```cpp
plantProbabilityTree = 0.33f;
plantProbabilityBush = 0.33f;
// Resto = 34% hierba
```

---

## 🌿 Tamaños de Plantas

```cpp
// Herbaceous (Grass)
const float GRASS_SIZE_MIN = 0.8f;
const float GRASS_SIZE_MAX = 1.2f;
const float GRASS_HEIGHT = 2.0f;

// Bush
const float BUSH_SIZE_MIN = 1.2f;
const float BUSH_SIZE_MAX = 1.8f;
const float BUSH_HEIGHT = 4.0f;

// Tree
const float TREE_SIZE_MIN = 1.5f;
const float TREE_SIZE_MAX = 2.5f;
const float TREE_HEIGHT = 8.0f;
```

---

## ⚛️ Parámetros de Átomos

```cpp
// Degradación (segundos para convertir a SOIL)
const float ATOM_DEGRADATION_HERBACEOUS = 60.0f;    // Grass
const float ATOM_DEGRADATION_BUSH = 120.0f;         // Bush
const float ATOM_DEGRADATION_TREE = 300.0f;         // Tree

// Resistencia al daño (0.0-1.0)
const float ATOM_BREAK_RESISTANCE_STEM = 0.7f;      // Difícil romper
const float ATOM_BREAK_RESISTANCE_LEAF = 0.3f;      // Fácil romper
const float ATOM_BREAK_RESISTANCE_BRANCH = 0.8f;    // Muy difícil

// Generación
const int HERBACEOUS_STEM_Segments = 4;
const int HERBACEOUS_LEAVES_PER_SEGMENT = 3;
const int BUSH_PRIMARY_BRANCHES = 3;
const int BUSH_SECONDARY_BRANCHES = 2;
const int TREE_BRANCHING_LEVELS = 4;
```

---

## 🌳 Parámetros del Ecosistema

```cpp
// Especies iniciales
enum class PlantType {
    GRASS = 0,
    BUSH = 1,
    TREE = 2
};

// Datos de especies
struct SpeciesData {
    float growthRate;            // 0.05-0.15 (plantas/día)
    float reproductionAge;       // Mínimo edad para reproducir (días)
    int maxLifespan;             // Máximo edad (días)
    float seedProductionRate;    // Semillas por planta madura/año
    float germinationProbability; // 0.01-0.1
    float competitionTolerance;  // 0.3-0.9
    float symbiosisBonus;        // +5-15% crecimiento
};
```

### Configuración Por Especie

```cpp
// GRASS
SpeciesData grass{
    growthRate: 0.15f,           // Crece rápido
    reproductionAge: 5.0f,       // Reproduce pronto
    maxLifespan: 30.0f,          // Corta vida
    seedProductionRate: 500.0f,  // Muchas semillas
    germinationProbability: 0.15f,
    competitionTolerance: 0.5f,
    symbiosisBonus: 0.0f
};

// BUSH
SpeciesData bush{
    growthRate: 0.10f,           // Mediano
    reproductionAge: 15.0f,
    maxLifespan: 100.0f,
    seedProductionRate: 50.0f,
    germinationProbability: 0.08f,
    competitionTolerance: 0.6f,
    symbiosisBonus: 0.1f         // Beneficio de simbiosis
};

// TREE
SpeciesData tree{
    growthRate: 0.05f,           // Lento y constante
    reproductionAge: 30.0f,      // Reproduce tarde
    maxLifespan: 500.0f,         // Larga vida
    seedProductionRate: 20.0f,   // Pocas pero buenas semillas
    germinationProbability: 0.05f,
    competitionTolerance: 0.8f,  // Muy resistente
    symbiosisBonus: 0.15f        // Beneficio de simbiosis
};
```

---

## 🎨 Colores & Renderizado

```cpp
// Colores de átomos (RGBA)
const glm::vec4 COLOR_STEM = glm::vec4(0.4f, 0.6f, 0.2f, 1.0f);     // Verde
const glm::vec4 COLOR_LEAF = glm::vec4(0.2f, 0.8f, 0.3f, 0.9f);     // Bright verde
const glm::vec4 COLOR_BRANCH = glm::vec4(0.6f, 0.5f, 0.3f, 1.0f);   // Marrón
const glm::vec4 COLOR_SOIL = glm::vec4(0.55f, 0.40f, 0.25f, 1.0f);  // Tierra

// Punto de vista / Renderizado
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float POINTS_SIZE_PLANTS = 5.0f;
const float POINTS_SIZE_TERRAIN = 3.0f;
```

---

## 🔊 Parámetros de Audio

```cpp
// Windows Beep API
const int SOUND_FREQUENCY_PLACE = 800;        // Hz
const int SOUND_FREQUENCY_DELETE = 400;       // Hz
const int SOUND_DURATION = 100;               // ms

// Volumen (0.0-1.0)
const float MASTER_VOLUME = 1.0f;
```

---

## 📊 Límites de Simulación

```cpp
// Límites de entities
const int MAX_PLANTS_IN_SCENE = 500;          // Máximo plantas
const int MAX_ATOMS_PER_PLANT = 200;          // Máximo átomos por plant
const int MAX_TRIANGLES_PER_PLANT = 600;      // Máximo triángulos

// Física
const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);
const float PHYSICS_DAMPING = 0.95f;          // Amortiguamiento
```

---

## 🎮 Estados del Juego

```cpp
enum class GameState {
    SPLASH,        // Pantalla de bienvenida (3s)
    MENU,          // Menú principal
    PLAYING,       // Gameplay en progreso
    SETTINGS,      // Configuración
    CREDITS        // Créditos
};

// Timers
const float SPLASH_DURATION = 3.0f;           // 3 segundos
const float ESC_DEBOUNCE_TIME = 0.3f;         // Evita rebotes
```

---

## 🔧 Cómo Editar Configuración

### Método 1: En Juego (Recomendado)

1. Presiona **ESC** → Abre menú
2. Selecciona **SETTINGS**
3. Ajusta sliders:
   - Tree Probability
   - Bush Probability
   - Plant Size
4. Click **APPLY**

**Ventaja**: Cambios en tiempo real, sin recompilar

### Método 2: Editar Config.h

1. Abre `src/Config.h`
2. Modifica constantes:
   ```cpp
   const float CAMERA_MOVEMENT_SPEED = 20.0f;
   ```
3. Recompila:
   ```powershell
   .\build.ps1
   ```

**Ventaja**: Control total, cambios permanentes

### Método 3: Archivo de Configuración (Futuro)

Planeado para Phase 2:
- `config.json` en el directorio de ejecución
- Cargar automáticamente al iniciar
- Guardar cambios del menú

---

## 📈 Perfiles de Configuración

### Rendimiento (Performance)

```cpp
// Menos content = más FPS
MAX_PLANTS_IN_SCENE = 100;
ATOM_DEGRADATION_TREE = 120.0f;     // Degrada más rápido
```

### Visual (Máxima Calidad)

```cpp
MAX_PLANTS_IN_SCENE = 500;
POINTS_SIZE_PLANTS = 8.0f;          // Puntos más visibles
TREE_BRANCHING_LEVELS = 5;          // Más detalles
```

### Educativo (Simulación Realista)

```cpp
plantProbabilityTree = 0.25f;
plantProbabilityBush = 0.25f;
ATOM_DEGRADATION_TREE = 300.0f;     // Realista (5 minutos)
```

---

## ⚡ Valores Recomendados

### Para Inicio (Principiante)

```cpp
CAMERA_MOVEMENT_SPEED = 15.0f;       // Más lento, más controlable
plantProbabilityTree = 0.20f;        // Menos árboles
MAX_PLANTS_IN_SCENE = 200;           // Menos plantas
```

### Para Producción (Estable)

```cpp
CAMERA_MOVEMENT_SPEED = 20.0f;
plantProbabilityTree = 0.30f;
plantProbabilityBush = 0.15f;
MAX_PLANTS_IN_SCENE = 500;
```

### Para Testing (Max Stress)

```cpp
MAX_PLANTS_IN_SCENE = 1000;
MAX_ATOMS_PER_PLANT = 300;
TREE_BRANCHING_LEVELS = 5;
// ⚠️ Puede causar lag
```

---

## 🐛 Debugging Flags

```cpp
// Añadir a Config.h según necesario:
#define DEBUG_ATOM_RENDERING   // Muestra wireframe de átomos
#define DEBUG_ECOSYSTEM_STATS  // ImGui stats panel
#define DEBUG_PHYSICS          // Muestra velocidades
#define DEBUG_RAYCASTING       // Muestra ray-cast
```

---

## 📝 Checklist de Customización

- [ ] Ajustado CAMERA_MOVEMENT_SPEED a preferencia
- [ ] Cambiad probabilidades de plantas
- [ ] Verificad MAX_PLANTS_IN_SCENE según tu PC
- [ ] Testad con diferentes ATOM_DEGRADATION_* valores
- [ ] Probad diferentes SpeciesData para balanceo
-

 [ ] Ajustado WINDOW_WIDTH/HEIGHT si lo necesitas

---

**Last Updated**: April 11, 2026  
**Version**: 1.0  
**Status**: ✅ Complete

