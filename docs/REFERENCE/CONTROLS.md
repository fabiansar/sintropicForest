# 🎮 Controles - Guía Completa

## Teclado

| Tecla | Acción | Módulo |
|-------|--------|--------|
| **Click Izquierdo** | Plantar en posición | GameLogic |
| **Click Derecho** | Eliminar planta cercana | GameLogic |
| **W** | Mover vista adelante | InputManager |
| **A** | Mover vista izquierda | InputManager |
| **S** | Mover vista atrás | InputManager |
| **D** | Mover vista derecha | InputManager |
| **↑** | Mover vista arriba (lento) | InputManager |
| **↓** | Mover vista abajo (lento) | InputManager |
| **←** | Mover vista izquierda (lento) | InputManager |
| **→** | Mover vista derecha (lento) | InputManager |
| **Q** | Rotar cámara 45° counterclockwise | CameraSystem |
| **E** | Rotar cámara 45° clockwise | CameraSystem |
| **Scroll Up** | Zoom in (acercarse) | InputManager |
| **Scroll Down** | Zoom out (alejarse) | InputManager |

---

## ESC - Menú Principal

Presionar **ESC** abre el menú con opciones:

```
┌─────────────────────────────┐
│   SINTROPIC FOREST          │
├─────────────────────────────┤
│  [PLAY]         Continuar    │
│  [SETTINGS]     Configurar   │
│  [CREDITS]      Créditos     │
│  [EXIT]         Salir        │
└─────────────────────────────┘
```

### SETTINGS (Configuración)

```
Plant Probabilities:
┌─────────────────────────┐
│ Tree Probability: [====|] 30%  
│ Bush Probability: [==| ]  15%
│ Grass Probability: [====] 55%
│
│ Plant Size (Scale): [==|  ] 1.0x
│
│ [APPLY]  [CANCEL]
└─────────────────────────┘
```

**Parámetros ajustables:**
- `plantProbabilityTree` - Probabilidad de árboles (0-100%)
- `plantProbabilityBush` - Probabilidad de arbustos (0-100%)
- `plantSizeMultiplier` - Escala de todas las plantas
- *Los porcentajes deben sumar ~100%*

---

## Movimiento de Cámara

### WASD - Movimiento Rápido

Los movimientos **rotan según el ángulo de rotación actual** (Q/E):

```
Ángulo: 0°
┌─────────┐
│  W      │
│ A   D   │  WASD mueve hacia donde apunta la vista
│  S      │
└─────────┘
  Adelante
  Izq Dcha
  Atrás

Ángulo: 90° (rotado 90° con E)
┌─────────┐
│  A      │
│ S   W   │  Controles ajustados a rotación
│  D      │
└─────────┘
  Adelante
  Izq Dcha
  Atrás
```

**Velocidad**: 20 unidades/segundo (configurable)

### Flechas - Movimiento Lento

```
↑ = +Y (arriba)
↓ = -Y (abajo)
← = -X (izquierda)
→ = +X (derecha)
```

**Velocidad**: 5 unidades/segundo (fijo)
**Ventaja**: NO se afecta por rotación de cámara

---

## Rotación de Cámara (Q/E)

```
Q: Rota 45° counterclockwise
↓
Rotación: -45°
↓
Ver ángulo en pantalla: "Camera Angle: 315°"

E: Rota 45° clockwise
↓
```

**Velocidad**: 3.0 rotaciones/segundo (configurable)
**Rango**: 0° a 360° (cíclico)

**Impacto en WASD**: Los controles WASD se rotan!
```cpp
// Cuando presionas W y cámara está rotada 90°:
// En lugar de avanzar +Z, avanzas +X
// El sistema automáticamente rota el input
```

---

## Zoom (Mouse Wheel)

```
Scroll Up    → Acerca la cámara (zoom in)
Scroll Down  → Aleja la cámara (zoom out)
```

**Rango**: 5 a 50 unidades de distancia  
**Velocidad**: Suave e inmediato

---

## Click - Plantación

### Click Izquierdo - Plantar

```
1. Clic en terreno (no en planta)
   ↓
2. Ray-cast desde cámara al mundo
   ↓
3. Acierta terreno en (x, y=0, z)
   ↓
4. Genera tipo aleatorio:
   - 30% Árbol (8-10 metros)
   - 15% Arbusto (4-5 metros)
   - 55% Hierba (1-2 metros)
   ↓
5. Planta creada en escena
   Sound: "plip plop" (double tone)
```

**Probabilidades**: Ajustables en SETTINGS

**Distancia máxima**: Ray-cast hasta 1000 unidades

---

### Click Derecho - Eliminar

```
1. Clic en cualquier lugar
   ↓
2. Ray-cast busca planta más cercana
   ↓
3. Si distancia < 50 unidades:
   - Elimina planta
   - Sound: "poop" (single tone)
   ↓
4. Si no hay planta cercana:
   - Sin efecto
```

**Rango**: ~50 unidades de distancia  
**Sound feedback**: Confirmación audible

---

## Indicadores Visuales

### HUD On-Screen

```
┌──────────────────────────────────────────┐
│ Camera Position: (10.0, 15.0, -12.5)     │
│ Camera Angle: 45°                        │
│ Plants: 23 (15 Grass, 5 Bush, 3 Tree)    │
│ FPS: 60                                  │
│                                          │
│ [ESC] Menu                               │
└──────────────────────────────────────────┘
```

### Información Debug (ImGui)

```
Atom System Stats:
├─ Total Atoms: 2,341
├─ Total Triangles: 7,023
├─ Intact Atoms: 2,105
└─ Avg Integrity: 89.9%

Ecosystem Stats:
├─ Biodiversity: 0.8
├─ Total Biomass: 156.5 kg
├─ Ecosystem Health: 0.92
└─ Soil Generated: 237 atoms
```

---

## Atajos de Teclado (Comando Rápido)

| Combo | Efecto |
|-------|--------|
| **ESC** | Abrir menú (toggle) |
| **ESC ESC** | Confirmar salida |
| **WASD + Q/E** | Mover + rotar (combinado) |
| *Ctrl+Click* | *Reservado para futuro* |

---

## Estados de Interacción

```
IDLE STATE
└─ Esperando input del usuario
   ├─ Click Izq en terreno → Plantar
   ├─ Click Der en planta → Eliminar
   ├─ WASD/Arrows → Mover vista
   ├─ Q/E → Rotar
   └─ ESC → Abrir menú

MENU STATE
└─ Menú abierto
   ├─ Click en botón → Transicionar estado
   ├─ Ajustar sliders → Cambiar config
   └─ ESC → Cerrar menú

PLAYING STATE (=IDLE pero controlado)
└─ Gameplay en progreso
   └─ Todos los controles disponibles
```

---

## Velocidades Configurables

Los siguientes valores están en `Config.h`:

```cpp
const float CAMERA_ROTATION_SPEED = 3.0f;      // Q/E rotación
const float CAMERA_MOVEMENT_SPEED = 20.0f;    // WASD movimiento
const float CAMERA_SLOW_MOVEMENT = 5.0f;      // Flechas movimiento
const float CAMERA_DISTANCE = 30.0f;          // Distancia orbital
const float CAMERA_HEIGHT = 20.0f;            // Altura cámara
```

---

## Troubleshooting de Controles

### WASD no funciona según espero
- **Problema**: Cámara rotada con Q/E
- **Solución**: Los movimientos WASD se rotan automáticamente ✅
- **Alternativa**: Usa flechas (no se rotan)

### Click no planta nada
- **Problema**: Clicked en planta existente, no en terreno
- **Solución**: Click en área vacía (terreno verde)

### Cámara no se mueve
- **Problema**: Ventana no enfocada o tecla ESC abierta menú
- **Solución**: Click en ventana, cierra menú con ESC

### Zoom no funciona
- **Problema**: Cursor fuera de ventana o evento no capturado
- **Solución**: Click en ventana primero, luego scroll

---

## Tips Avanzados

1. **Combo Eficiente**: Usa Q/E + WASD juntos para explorar rápido
2. **Precision**: Flechas para micromovimientos exactos
3. **Planting**: Click rápido múltiple en diferentes spots
4. **Rotation**: 4× E-presses = giro total (360°)
5. **Menu**: ESC abre/cierra (toggle), ajusta probs antes de jugar

---

**Last Updated**: April 11, 2026  
**Status**: ✅ Complete

