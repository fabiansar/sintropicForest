# 🌍 SintropicForest - Visión Completa del Proyecto

## 🎯 Pitch Ejecutivo

**SintropicForest** es un simulador 3D de ecosistemas basado en agroforestería que permite observar, planificar e influir sobre la sucesión ecológica natural. El jugador actúa como planificador ambiental, sembrando plantas y observando cómo la naturaleza self-organiza hacia un bosque maduro y biodiverso.

### Géneros
- Simulador ecológico
- Puzzle estratégico 
- Sandbox observacional

---

## 🎮 Core Loop del Juego

```
1. OBSERVAR
   └─ Cámara libre para explorar simulación en tiempo acelerado
   
2. PLANTAR
   └─ Click para sembrar especies (hierba, arbusto, árbol)
   
3. MONITOREAR
   └─ Ajustar condiciones globales (luz, agua, nutrientes)
   
4. CICLO COMPLETO
   └─ Ver evolución: germinación → crecimiento → reproducción → muerte
```

---

## 🏗️ Arquitectura Técnica Final

### Módulos Principales

```
SintropicForest
├── GraphicsEngine (180 LOC)
│   └─ Coordinador de sistemas
├── InputManager
│   └─ Entrada desacoplada (teclado/mouse)
├── CameraSystem  
│   └─ Cámara orbital isométrica
├── GameLogic
│   └─ Coordinador de gameplay
├── StateManager
│   └─ Máquina de estados (SPLASH → MENU → PLAYING)
├── EcosystemSimulator ⭐ (NUEVO)
│   └─ Core de simulación ecológica
├── AudioManager
│   └─ Efectos de sonido
└── Shaders
    └─ Renderizado 3D
```

### EcosystemSimulator - Detalles

```cpp
EcosystemSimulator
├── update(deltaTime)
│   ├─ updatePlantLife() → Ciclo de vida
│   ├─ updateEnvironmentalStress() → Estrés ambiental
│   ├─ checkSuccession() → Transiciones GRASS→BUSH→TREE
│   ├─ disperseSeeds() → Reproducción
│   ├─ resolveCompetition() → Competencia por luz/agua
│   └─ updateInteractions() → Simbiosis
├── PlantData (500 plantas máx)
│   ├─ age, stage, healthFactor
│   ├─ currentHeight, currentRadius
│   ├─ localLight, localWater, localNutrients
│   └─ seedBankAccumulated
└── SpeciesData × 3
    ├─ GRASS (pionera)
    ├─ BUSH (transición)
    └─ TREE (clímax)
```

---

## 🌱 Dinámicas Ecológicas Implementadas

### 1. Ciclo de Vida Realista (5 Etapas)

| Etapa | % Edad | Características |
|-------|--------|-----------------|
| SEEDLING | 0-20% | Tamaño 10%, vulnerable |
| JUVENILE | 20-50% | Crecimiento acelerado |
| ADULT | 50-85% | Producción de semillas |
| SENESCENT | 85-100% | Decline de salud |
| DEAD | 100%+ | Biomasa → descomposición |

**Crecimiento**: Curva sigmoide realista (s-curve natural)

### 2. Sucesión Ecológica

```
Hierba (30s lifespan)
   ↓ (sombra de árboles)
Arbusto (80s lifespan)
   ↓ (edad + estabilidad)
Árbol (180s lifespan)
```

### 3. Reproducción y Dispersión

- Plantas ADULT generan semillas continuamente
- Dispersión aleatoria en radio específico de especie
- 40% germination chance
- Genera colonización natural

### 4. Estrés Ambiental

**Factores**: Luz, Agua, Nutrientes

```
Stress = LightStress + WaterStress + NutrientStress

Si Stress > 1.0:
  → Health decrece
  → Crecimiento se ralentiza
  → Probabilidad de muerte
```

### 5. Competencia

- Plantas altas proyectan sombra
- Reduce luz local de vecinas cercanas
- Natural crowding-out effect

### 6. Interacciones Simbióticas

- Arbustos (leguminosas) + Árboles
- Árbol gana bonus de health
- Simula fijación de nitrógeno

---

## 📊 Sistema de Retroalimentación Visual

### En Tiempo Real

**Tamaño**: Refleja edad × health
```
visual_height = max_height × age_curve × health_factor
```

**Color**: Indica salud
- 🟢 Verde vibrante (health > 0.8)
- 🟡 Verde pálido (0.5-0.8)
- 🟠 Marrón (< 0.5)

**Animación**: Crecimiento suave y perceptible

### En UI

```
ECOSYSTEM STATUS
├─ Trees: 12 (████░░░░░)
├─ Bushes: 89 (██████░░░)
├─ Grass: 234 (█████████)
├─ Avg Health: 0.75
├─ Biodiversity: 1.23
└─ Biomass: 347.2

ENVIRONMENT
├─ ☀️ Light: 0.80
├─ 💧 Water: 0.60
├─ 🌱 Nutrients: 0.50
└─ ⚡ Sim Speed: 1.5x
```

---

## 🎯 Modos de Juego

### 1. **SANDBOX** (Actual)
- Sin límites ni objetivos
- Observar simulación libremente
- Manipular condiciones ambientales

### 2. **CHALLENGE** (Futuro)
- Objetivos específicos:
  - "Crear 100 árboles"
  - "Máxima biodiversidad en 5 minutos"
  - "Sobrevivir sequía de 30 segundos"
- Puntuación y leaderboard

### 3. **RESTORATION** (Futuro)
- Empezar con terreno bare
- Restaurar bosque nativo
- Proteger especies endémicas

---

## 📈 Roadmap de Desarrollo

### ✅ Fase 1 (Completada)
- [x] Motor gráfico 3D con OpenGL
- [x] Sistema modular básico
- [x] Interacción de clicks
- [x] Menú y UI

### 🔄 Fase 2 (Actual)
- [x] EcosystemSimulator core
- [x] Ciclos de vida realistas
- [x] Sucesión ecológica
- [ ] Integración con GameLogic (próximo)
- [ ] Testing de dinámicas

### 📋 Fase 3 (Próximo)
- [ ] Clima dinámico (estaciones, sequía)
- [ ] Terreno heterogéneo
- [ ] Plagas y enfermedades
- [ ] Polinizadores
- [ ] Descomposición visible

### 🚀 Fase 4 (Long-term)
- [ ] Ciclos biogeoquímicos (C, N, P)
- [ ] Sistemas de raíces
- [ ] Microclimas
- [ ] Multijugador cooperativo
- [ ] VR/AR support

---

## 🎓 Valor Educativo

### Conceptos Ecológicos Modelizados

1. **Sucesión Primaria**: Colonización → Estabilización
2. **Competencia Interespecífica**: Por luz, agua, nutrientes
3. **Dispersión de Semillas**: Radios y probabilidades
4. **Ciclos de Vida**: Edad, reproducción, mortalidad
5. **Fecundidad Dependiente de Edad**: Solo adultas producen
6. **Senescencia**: Decline natural al envejecer
7. **Interacciones**: Simbiosis, alelopatía, facilitación

### Aplicaciones Prácticas

- **Agroforestería**: Diseño de sistemas sostenibles
- **Reforestación**: Planificación de restauración
- **Cambio Climático**: Impacto de condiciones ambientales
- **Biodiversidad**: Importancia de diversidad

---

## 🛠️ Especificaciones Técnicas

### Performance Target
- **FPS**: 60 (VSync)
- **Plants**: Hasta 500 simultáneas
- **Update Rate**: 0.1s (10 Hz) para simulación
- **Memoria**: < 200MB

### Plataforma
- **OS**: Windows 10+
- **Graphics**: OpenGL 3.3+
- **CPU**: Mínimo: AMD Ryzen 5 / Intel i5
- **RAM**: 4GB

### Tecnologías
- **Motor**: Custom C++ 17 + OpenGL
- **UI**: ImGui
- **Physics**: Matemáticas simples (sin Bullet3D)
- **Datos**: En-memoria (sqlite futuro)

---

## 💾 Persistencia (Futuro)

### Guardar Simulación
```json
{
  "timestamp": "2026-04-11T15:30:00",
  "ecosystemState": {
    "plants": [...],
    "globalConditions": {
      "light": 0.8,
      "water": 0.6,
      "nutrients": 0.5
    }
  },
  "stats": {
    "playtime": 3600,
    "plantsCumulative": 1500
  }
}
```

---

## 📱 UI/UX Flow

### Pantalla Principal
```
[PLAY] [SETTINGS] [CREDITS]
```

### Juego (PLAYING)
```
┌──────────────────────────────────┐
│ 3D Viewport (Dramageable)        │
│ ┌──────────────────────────────┐ │
│ │                              │ │
│ │    (3D Ecosystem Scene)      │ │
│ │                              │ │
│ └──────────────────────────────┘ │
├──────────────────────────────────┤
│ [ECOSYSTEM STATS]  [ENVIRONMENT] │
│ Trees: 12          Light: 0.80   │
│ Bushes: 89         Water: 0.60   │
│ Grass: 234         Nutrient: 0.5 │
└──────────────────────────────────┘

Controls:
  Click: Plant
  WASD: Move camera
  Q/E: Rotate
  Scroll: Zoom
  ESC: Menu
```

---

## 🔐 Validación y Testing

### Unit Tests (Futuro)
- [ ] Crecimiento sigmoide
- [ ] Estrés ambiental
- [ ] Sucesión triggers
- [ ] Dispersión aleatoria
- [ ] Biodiversidad index

### Integration Tests
- [ ] Full lifecycle (germinación→muerte)
- [ ] Sucesión GRASS→BUSH→TREE
- [ ] Respuesta a cambios ambientales
- [ ] Performance con 500 plantas

---

## 💡 Inspiración y Referencias

- **Papers**: 
  - "Ecological Succession Models" (Odum, 1969)
  - "Agroforestry: A Sustainable Land-Use System" (Nair, 1993)

- **Juegos Similares**:
  - Creatures (vida artificial)
  - Evolution: The Game (simulación genética)
  - Terra Nil (puzzle ambiental)

- **Simuladores**:
  - NetLogo (sistemas complejos)
  - LANDIS Pro (sucesión forestal)

---

## 📞 Próximos Pasos

1. **Integrar** EcosystemSimulator en GameLogic ✓ (guía creada)
2. **Testar** dinámicas individuales
3. **Visualizar** estadísticas en time-lapse
4. **Optimizar** performance si es necesario
5. **Documentar** parámetros ajustables
6. **Publicar** alpha para feedback

---

**Versión**: 1.0  
**Última actualización**: 2026-04-11  
**Estado del Proyecto**: Pre-alpha (Core systems ready)  
**Próximo Milestone**: Integration testing
