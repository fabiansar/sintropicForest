# 🎮 Juego de Gestión 3D - Terreno y Clicks

## 🎯 Nuevas Características

### ✨ Terreno 3D
- Plano verde renderizado con patrón de grid
- Tamaño: 100x100 unidades
- Resolución: 50x50 cuadrados
- Iluminación básica para profundidad

### 📍 Sistema de Clics
**Click izquierdo en el terreno:**
- Convierte coordenadas 2D de pantalla a 3D del mundo (raycast)
- Crea una nueva luz aleatoria en esa posición
- Hasta 500 luces máximo simultáneamente
- Las luces rebotan en los límites del terreno

---

## 🎮 Controles

### Cámara (Juego de Gestión)
| Tecla | Acción |
|-------|--------|
| **Q** | Rotar cámara izquierda |
| **E** | Rotar cámara derecha |
| **↑ Arriba** | Mover vista hacia arriba |
| **↓ Abajo** | Mover vista hacia abajo |
| **← Izquierda** | Mover vista hacia la izquierda |
| **→ Derecha** | Mover vista hacia la derecha |

### Interacción
| Acción | Efecto |
|--------|--------|
| **Click Izquierdo** | Agregar luz aleatoria en esa posición |
| **ESC** | Volver al menú |

---

## 🔧 Cómo Funciona

### 1️⃣ Raycast (Conversión 2D→3D)
Cuando haces click:
```
Pantalla (2D)
     ↓
Normalizar coordenadas (-1 a 1)
     ↓  
Aplicar matrices de proyección/vista
     ↓
Crear rayo 3D desde cámara
     ↓
Intersectar con plano Y=0 (terreno)
     ↓
Resultado: Posición 3D en el terreno
```

### 2️⃣ Generación del Terreno
- Grid de 50x50 cuadrados
- Cada cuadrado = 2x2 unidades
- Total = 100x100 unidades
- 1 triángulo = 2 tris por cuadrado

### 3️⃣ Movimiento de Luces
- Se mueven suavemente hacia los límites
- Rebotan al chocar (velocidad inversa)
- Mantienen altura Y=2 (arriba del terreno)

---

## 📊 Configuración (Config.h)

```cpp
const int MAX_LIGHTS = 500;              // Máximo de luces
const int TERRAIN_SIZE = 100;            // Tamaño del terreno (100x100)
const int TERRAIN_RESOLUTION = 50;       // Resolución del grid (50x50)
const float CAMERA_HEIGHT = 40.0f;       // Altura de la cámara
const float CAMERA_DISTANCE = 30.0f;     // Distancia de la órbita
const float CAMERA_ROTATION_SPEED = 1.0f; // Velocidad de rotación con Q/E
```

### Modificar Tamaño del Terreno
En `Config.h`:
```cpp
const int TERRAIN_SIZE = 200;  // Ahora 200x200 en lugar de 100x100
```

### Modificar Máximo de Luces
En `Config.h`:
```cpp
const int MAX_LIGHTS = 1000;  // Hasta 1000 luces
```

---

## 🎨 Información en Pantalla

Mientras juegas ves:
```
Game Info
ESC: Back to Menu
Lights: 47 / 500          ← Luces actuales / máximo
Q/E: Rotate Camera
Arrows: Move Camera
Click: Add Light
────────────
Camera Pos: 15.3, -8.7
Rotation: 45.2 degrees
```

---

## 🔍 Detalles Técnicos

### Shaders Nuevos

**Terreno (Vertex Shader):**
- Lee posición y normal de cada vértice
- Aplica transformaciones (proyección, vista, modelo)
- Pasa normal al fragment shader

**Terreno (Fragment Shader):**
- Iluminación direccional simple (luz desde arriba)
- Color base: verde (0.2, 0.6, 0.3)
- Brillo mínimo: 30% (sin completa oscuridad)
- Patrón de grid procedural cada 2 unidades

### Raycast

La función `getRaycastHit()`:
1. Normaliza coordenadas mouse a [-1, 1]
2. Invierte matrices de proyección y vista
3. Crea rayo desde cámara
4. Intersecta con plano Y=0
5. Clampea resultado dentro del terreno

---

## 💡 Ejemplos de Uso

### Ejemplo 1: Crear patrón de luces
1. Abre el juego
2. Presiona "Play"
3. Haz clicks en forma de línea → luces en línea
4. Haz clicks en forma de círculo → patrón circular
5. Las luces rebotan y crean efectos visuales

### Ejemplo 2: Explorar desde diferentes ángulos
1. Usa Q/E para rotar
2. Usa flechas para mover centro
3. Observa cómo se ve el terreno desde diferentes puntos

### Ejemplo 3: Llenar el terreno
1. Haz clicks rápidamente
2. Observa cómo se llena el mapa
3. Verás el límite de 500 luces

---

## 🐛 Solución de Problemas

### **El raycast no funciona correctamente**
- Verifica que `getRaycastHit()` esté siendo llamado
- Comprueba que las matrices de proyección/vista sean iguales

### **Las luces aparecen bajo el terreno**
- Aumenta `light.position.y` en `addRandomLight()`
- Actualmente está en 2.0f

### **El terreno no se ve**
- Verifica que `generateTerrain()` se llame en `initialize()`
- Verifica que `terrainShaderProgram` se esté usando en render

### **Performance lento con muchas luces**
- Reduce `MAX_LIGHTS`
- O aumenta `TERRAIN_RESOLUTION` (menos triángulos)

---

## 🎯 Arquitectura del Código

```
GraphicsEngine.cpp

handleInput()
  ├─ Detectar Q/E paralotación
  ├─ Detectar flechas para movimiento
  └─ Detectar click izquierdo
      └─ getRaycastHit()
          └─ addRandomLight()

update()
  └─ Actualizar posiciones de luces
      └─ Physics simple (colisiones)

renderGameScene()
  ├─ Renderizar terreno
  │   ├─ terrainShaderProgram
  │   └─ terrainVAO / terrainEBO
  ├─ Renderizar puntos de luz
  │   ├─ shaderProgram
  │   └─ VAO / VBO
  └─ ImGui (información)
```

---

## 🚀 Próximas Mejoras Posibles

| Feature | Dificultad | Descripción |
|---------|-----------|-------------|
| Seleccionar/mover luces | ⭐⭐ | Click drag para mover |
| Eliminar luces | ⭐ | Click derecho para borrar |
| Terreno con texturas | ⭐⭐⭐ | Agregar césped/tierra |
| Colisiones real | ⭐⭐⭐ | Physics engine |
| Partículas | ⭐⭐ | Efectos al agregar luz |
| Guardar/cargar mapa | ⭐⭐ | Persistencia de datos |
| Zoom cámara | ⭐ | Rueda del ratón |
| Múltiples terrenos | ⭐⭐⭐ | Sistema de niveles |

---

**¡Disfruta tu nuevo juego de gestión 3D!** 🎮✨
