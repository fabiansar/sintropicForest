# 🎮 First-Person Shooter - Controles

## Controles de Movimiento

| Tecla | Acción |
|-------|--------|
| **W** | Avanzar (hacia donde mira) |
| **S** | Retroceder |
| **A** | Movimiento lateral izquierda |
| **D** | Movimiento lateral derecha |
| **ESPACIO** | Subir arriba |
| **CTRL** | Bajar abajo |

## Control de Cámara

| Control | Acción |
|---------|--------|
| **RATÓN** | Mirar alrededor (rotación libre) |
| **ESC** | Volver al menú |

---

## ¿Cómo Funciona?

### 1️⃣ Sistema de Cámara
La cámara está en **primera persona**. Ves el mundo desde los ojos del jugador:
- Puedes mirar en cualquier dirección moviendo el ratón
- El rango de rotación vertical está limitado (89°) para evitar rotaciones extrañas
- La sensibilidad del ratón es configurable

### 2️⃣ Movimiento
Tu personaje se mueve en la dirección hacia donde está mirando:
- **W/S:** Adelante/Atrás (respeta la dirección de la mirada)
- **A/D:** Izquierda/Derecha (perpendicular a donde miras)
- **ESPACIO/CTRL:** Arriba/Abajo (eje vertical absoluto)

### 3️⃣ Velocidad
La velocidad de movimiento es: **15 unidades por segundo**

Puedes modificar esto en [Config.h](src/Config.h):
```cpp
const float CAMERA_SPEED = 15.0f;  // Cambia este valor
```

---

## Información en Pantalla

Mientras juegas, ves:
```
Game Info
ESC: Back to Menu
Lights: 75
WASD: Move, SPACE/CTRL: Up/Down
Mouse: Look Around
Pos: 0.5, 5.2, -3.1        ← Tu posición actual
Yaw: -87.3, Pitch: 5.9     ← Ángulos de rotación
```

---

## Modificar Controles

### Cambiar Sensibilidad del Ratón
En [Config.h](src/Config.h):
```cpp
const float MOUSE_SENSITIVITY = 0.005f;  // Más pequeño = más lento
```

**Ejemplos:**
- `0.001f` = Muy lento (sniper)
- `0.005f` = Normal (recomendado)
- `0.01f` = Rápido
- `0.02f` = Muy rápido

### Cambiar Velocidad de Movimiento
En [Config.h](src/Config.h):
```cpp
const float CAMERA_SPEED = 15.0f;  // Unidades por segundo
```

**Ejemplos:**
- `5.0f` = Muy lento (caminar)
- `15.0f` = Normal (recomendado)
- `30.0f` = Rápido (correr)
- `50.0f` = Super rápido

---

## Cambiar Teclas

Si quieres cambiar las teclas (ej: ZQSD en lugar de WASD):

Abre [GraphicsEngine.cpp](src/GraphicsEngine.cpp) y busca `handleInput()`:

**Antes:**
```cpp
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cameraPos += movementSpeed * 0.016f * cameraFront;
}
```

**Después (cambiar a Z):**
```cpp
if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
    cameraPos += movementSpeed * 0.016f * cameraFront;
}
```

**Códigos de teclas disponibles:**
- `GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D` (letras)
- `GLFW_KEY_SPACE` (espacio)
- `GLFW_KEY_LEFT_CONTROL`, `GLFW_KEY_RIGHT_CONTROL` (control)
- `GLFW_KEY_LEFT_SHIFT`, `GLFW_KEY_RIGHT_SHIFT` (shift)
- `GLFW_KEY_UP`, `GLFW_KEY_DOWN`, `GLFW_KEY_LEFT`, `GLFW_KEY_RIGHT` (flechas)

---

## Tips

✅ **El ratón controla donde miras**
- Mueve el ratón libremente para rotar la cámara
- No hay límite horizontal, puedes girar 360°
- El límite vertical es 89° para evitar "gimbal lock"

✅ **El movimiento es fluido**
- La velocidad se calcula con delta time
- Funciona generalmente a 60 FPS con VSync

✅ **Puedes salir de la zona de luces**
- Explora los límites del mundo
- Las luces siguen rebotando independientemente de donde estés

---

## Próximas Mejoras Posibles

Estas características podrían agregarse:

| Feature | Dificultad | Descripción |
|---------|----------|-------------|
| Sprint (Shift) | ⭐ Fácil | Presionar Shift para correr más rápido |
| Jump (Espacio) | ⭐⭐ Media | Saltar con animación y gravedad |
| Collisiones | ⭐⭐⭐ Difícil | No atravesar paredes o el piso |
| Gravedad | ⭐⭐ Media | Caer naturalmente en lugar de bajar con CTRL |
| Armas | ⭐⭐⭐ Difícil | Disparar a las luces |
| Minimap | ⭐⭐ Media | Vista desde arriba en la esquina |

---

**¡Disfruta explorando el mundo en 3D!** 🚀
