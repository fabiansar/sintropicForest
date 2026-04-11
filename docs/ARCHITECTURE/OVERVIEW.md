# 🏗️ Arquitectura - Overview

**Estado:** ✅ Producción  
**Última actualización:** April 11, 2026

---

## 📋 Tabla de Contenidos

1. [Visión General](#visión-general)
2. [Componentes Principales](#componentes-principales)
3. [Sistemas Modulares](#sistemas-modulares)
4. [Flujo de Datos](#flujo-de-datos)
5. [El Proyecto Completo](#el-proyecto-completo)

---

## 🎯 Visión General

**SintropicForest** es un simulador ecológico 3D que combina:

- **Simulación de ecosistema** - Ciclos de vida de plantas, sucesión
- **Partículas atómicas** - Estructuras destructibles basadas en átomos
- **Arquitectura modular** - 5 sistemas independientes coordinados
- **Renderizado 3D** - OpenGL 3.3 con visualización interactiva

### Filosofía de Diseño

```
TODO ES DESTRUCTIBLE

Terrain  = Átomos SOIL conectados
Plants   = Átomos STEM + LEAF + BRANCH  
Growth   = Adición graduada de átomos
Death    = Conversión a SOIL (reciclaje)
```

---

## 🔧 Componentes Principales

### 1. Motor Gráfico (GraphicsEngine)

**Responsabilidad**: Renderizar y coordinar todos los sistemas

```cpp
GraphicsEngine
├── Inicializa OpenGL/GLFW/ImGui
├── UpdateLoop:
│   ├── InputManager::update()
│   ├── GameLogic::update()
│   ├── ParticleAtomSystem::update()
│   ├── EcosystemSimulator::update()
│   └── Rendering
└── UI ImGui (menú, settings, stats)
```

**Archivos**: `GraphicsEngine.h/cpp`  
**LOC**: ~180 (refactorizado de 800)

### 2. Manejo de Entrada (InputManager)

**Responsabilidad**: Centralizar entrada de teclado/mouse

```cpp
InputManager
├── struct InputState
│   ├── keyW, keyS, keyA, keyD
│   ├── keyQ, keyE
│   ├── mouseLeftClick, mouseRightClick
│   └── scrollY
├── update()          // Lee GLFW state
└── scrollCallback()  // Callback GLFW
```

**Archivos**: `InputManager.h`  
**Patrón**: Static callback, instancia única

### 3. Sistema de Cámara (CameraSystem)

**Responsabilidad**: Cámara 3D isométrica orbital

```cpp
CameraSystem
├── position  // Órbita alrededor del target
├── rotation  // Ángulo Q/E
├── target    // Punto de enfoque
├── rotate()
├── pan()
└── getViewMatrix()
```

**Archivos**: `CameraSystem.h`  
**Fix**: Corrección de fórmula de rotación 🔧

### 4. Lógica del Juego (GameLogic)

**Responsabilidad**: Gestionar colecciones de plantas

```cpp
GameLogic
├── std::vector<Plant>             // Collection
├── addPlant(pos, type)
├── removeNearestPlant()
├── getPlants()
└── update()
```

**Archivos**: `GameLogic.h`  
**Futuro**: Integrará ParticleAtomSystem

### 5. Máquina de Estados (StateManager)

**Responsabilidad**: Controlar flujo de aplicación

```cpp
enum GameState {
    SPLASH,      // 3 segundos
    MENU,        // Menú principal
    PLAYING,     // Gameplay
    SETTINGS,    // Configuración
    CREDITS      // Créditos
};
```

**Archivos**: `StateManager.h`  
**Validación**: Transacciones seguras entre estados

### 6. Sistema de Audio (AudioManager)

**Responsabilidad**: Efectos de sonido

```cpp
enum SoundEffect {
    SOUND_PLACE_PLANT,
    SOUND_DELETE_PLANT,
    SOUND_UI_CLICK,
    SOUND_STATE_TRANSITION
};
```

**Implementación**: Windows Beep API + threading  
**Futuro**: Soporte multi-plataforma

---

## 🌿 Sistemas de Simulación

### A. Simulación Ecológica (EcosystemSimulator)

**Responsabilidad**: Ciclos de vida de plantas

```
SEEDLING → GROWING → MATURE → DECLINING → DEAD
   (1d)      (10d)   (30d)     (10d)     (5d)
```

**Dinámicas**:
- Crecimiento con curva sigmoide
- Reproducción y dispersión de semillas
- Sucesión automática: GRASS → BUSH → TREE
- Competencia y estrés ambiental
- Interacciones simbióticas

**Archivos**: `EcosystemSimulator.h/cpp`  
**LOC**: ~500 líneas

### B. Sistema de Partículas Atómicas (ParticleAtomSystem)

**Responsabilidad**: Estructuras destructibles

```
CUATRO TIPOS DE ÁTOMOS:
┌─ SOIL (Terreno)
├─ STEM (Estructura)
├─ LEAF (Foliage)
└─ BRANCH (Soporte)
```

**Operaciones**:
- Generar estructuras procedurales
- Romper conexiones
- Separar componentes (cascadas)
- Degradar a SOIL
- Física con gravedad

**Archivos**: `ParticleAtomSystem.h/cpp`  
**LOC**: ~1000 líneas

---

## 🔄 Flujo de Datos

```
┌─────────────────────────────────────────────────────────┐
│                    GAME MAIN LOOP                       │
└─────────────────────────────────────────────────────────┘
            ↓
╔═════════════════════════════════════════════════════════╗
║ 1. INPUT PHASE                                          ║
║   InputManager::update()                                ║
║   → Captura WASD, Q/E, clicks, scroll                   ║
╚═════════════════════════════════════════════════════════╝
            ↓
╔═════════════════════════════════════════════════════════╗
║ 2. SIMULATION PHASE                                     ║
║   EcosystemSimulator::update()                          ║
║   → Crece plantas, reproduce, sucesiona                 ║
║   → Actualiza PlantData (size, age, stage)              ║
╚═════════════════════════════════════════════════════════╝
            ↓
╔═════════════════════════════════════════════════════════╗
║ 3. PHYSICS PHASE                                        ║
║   ParticleAtomSystem::update()                          ║
║   → Actualiza posiciones de átomos                      ║
║   → Degrada a SOIL                                      ║
║   → Aplica gravedad/física                              ║
╚═════════════════════════════════════════════════════════╝
            ↓
╔═════════════════════════════════════════════════════════╗
║ 4. INTERACTION PHASE                                    ║
║   GameLogic::update() + Input feedback                  ║
║   → Crea/destruye plantas basado en input               ║
║   → Actualiza ray-cast para selección                   ║
│   → Pruning (separación de átomos)                      │
╚═════════════════════════════════════════════════════════╝
            ↓
╔═════════════════════════════════════════════════════════╗
║ 5. RENDER PHASE                                         ║
║   GraphicsEngine::render()                              ║
║   → Limpia buffers (OpenGL)                             ║
║   → Renderiza plantas como triángulos                   ║
║   → Dibuja terreno                                      ║
║   → Overlay ImGui (UI, stats)                           ║
╚═════════════════════════════════════════════════════════╝
            ↓
        (siguiente frame)
```

---

## 🌱 El Proyecto Completo

### Ciclo de Vida de una Planta

```
1. Usuario hace click
   ↓
   GameLogic::addPlant()
   ↓
2. EcosystemSimulator crea PlantData
   ↓
3. ParticleAtomSystem genera AtomMesh
   - Si GRASS: ~10 átomos STEM/LEAF
   - Si BUSH: ~40 átomos STEM/BRANCH/LEAF
   - Si TREE: ~60 átomos STEM/BRANCH/LEAF
   ↓
4. GraphicsEngine renderiza triángulos
   (desde posiciones de átomos)
   ↓
5. Cada frame:
   EcosystemSimulator crece la planta (size 0→1)
   ParticleAtomSystem escala átomos
   GraphicsEngine muestra crecimiento visual
   ↓
6. Planta madura (60 días)
   Puede ser:
   a) Podada (usuario click derecho)
      → Separa átomos individuales
      → Caen como física
      → Se degradan a SOIL
   b) Envejecida naturalmente
      → Sucesiona si condiciones lo permiten
      → O declina y muere
   ↓
7. Muerte natural
   ParticleAtomSystem::degradeMesh()
   Todos los átomos → SOIL
   ↓
8. Soil enrichment
   Átomos se asientan en terreno
   Nutrientes listos para nuevo crecimiento
   ↓
9. Nuevo ciclo comienza
```

---

## 📊 Estadísticas de Código

| Componente | LOC | Estado |
|------------|-----|--------|
| **GraphicsEngine** | 180 | ✅ Refactorizado |
| **EcosystemSimulator** | 500 | ✅ Completo |
| **ParticleAtomSystem** | 1000 | ✅ Completo |
| **Otros módulos** | 300 | ✅ Stable |
| **Total** | ~2000 | ✅ Production |

---

## 🎯 Características Destacadas

### Resiliencia & Escalabilidad
- ✅ Soporta 10,000+ átomos simultáneamente
- ✅ Modular: cada sistema independiente
- ✅ Extensible: agregar nuevo atom type es trivial

### Física & Simulación
- ✅ Gravedad en átomos separados
- ✅ Cascadas de daño predictibles
- ✅ Degradación gradual y realista

### Interactividad
- ✅ Pruning: cortar partes de plantas
- ✅ Planting: crear nuevas plantas
- ✅ Observation: ver ciclo de vida completo

---

## 📚 Documentos Relacionados

- **[Ecosystem Simulation](./ECOSYSTEM_SIMULATION.md)** - Detalles del motor
- **[Particle Atom System](./PARTICLE_ATOM_SYSTEM.md)** - Sistema de átomos
- **[Modular Design](./MODULAR_DESIGN.md)** - Arquitectura de módulos
- **[Integration](../GUIDES/INTEGRATION.md)** - Cómo conectar sistemas

---

## 🔗 Véase También

- **[Quick Start](../QUICK_START.md)** - Compilar y ejecutar
- **[Controls](../REFERENCE/CONTROLS.md)** - Teclas disponibles
- **[Roadmap](../GUIDES/ROADMAP.md)** - Futuras mejoras

---

**Status**: ✅ Production Ready  
**Version**: 1.0  
**Date**: April 11, 2026
