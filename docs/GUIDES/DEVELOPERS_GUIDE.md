# 👨‍💻 Developer's Guide

Guía completa para desarrolladores que quieran contribuir al proyecto.

## 🚀 Setup Rápido (5 minutos)

```powershell
# 1. Clonar repositorio
git clone https://github.com/fabiansar/sintropicForest.git
cd sintropicForest

# 2. Compilar
.\build.ps1

# 3. Ejecutar
.\build\3DEngine.exe
```

## 🏗️ Estructura del Proyecto

```
proyecto/
├── src/                    # Código fuente
│   ├── main.cpp           # Punto de entrada (PRINCIPAL)
│   ├── InputManager.h      # Gestión de entrada (ratón/teclado)
│   ├── CameraSystem.h      # Sistema de cámara orbital
│   ├── Logger.h/.cpp       # Sistema de logging
│   ├── Config.h            # Configuración centralizada
│   └── ... (otros)
├── build/                  # Directorio de compilación
├── docs/                   # Documentación
│   ├── GUIDES/            # Guías de desarrollo
│   ├── REFERENCE/         # Referencias de API
│   └── SUMMARIES/         # Resúmenes
├── CMakeLists.txt         # Configuración de build
└── build.ps1              # Script de compilación PowerShell
```

## 🐛 Workflow de Debugging

### 1. Agregar Logs
```cpp
#include "Logger.h"

// En main.cpp o donde sea necesario:
Logger::init("game.log", Logger::Level::DEBUG);  // Habilitar DEBUG

// Usar en código
Logger::info("Camera rotated to: " + std::to_string(angle));
Logger::debug("Mouse position: " + std::to_string(mouseX) + ", " + std::to_string(mouseY));
Logger::warning("Input state: Q=" + std::to_string(input.keyQ));
Logger::error("Camera rotation not updating!");
```

### 2. Compilar con Logs
```powershell
# Los logs se activarán automáticamente
.\build.ps1

# Ver output en consola o en game.log
type game.log
```

### 3. Documentar Rápidamente
Cuando encuentres un bug, crea un archivo en `/memories/session/`:
```
/memories/session/mouse-rotation-bug.md
- Symptom: Right-click drag doesn't rotate camera
- Location: src/main.cpp line 395
- Status: BLOCKED - waiting for input system review
```

---

## 🎮 Sistemas Principales

### InputManager (src/InputManager.h)
Gestiona toda la entrada del usuario.

```cpp
const auto& input = inputManager->getInputState();

// Teclado
if (input.keyW) { /* adelante */ }
if (input.keyA) { /* izquierda */ }

// Ratón
if (input.mouseRightPressed) { /* botón derecho presionado */ }
float deltaX = input.mouseDeltaX;  // Cambio desde último frame
float deltaY = input.mouseDeltaY;
```

**Problema conocido**: Mouse delta no se calcula correctamente en algunos frames.

### CameraSystem (src/CameraSystem.h)
Sistema de cámara orbital alrededor de un punto objetivo.

```cpp
camera.panTargetRotated(forward, right);  // Moverse
camera.rotate(angle);                     // Rotar
camera.adjustDistance(distance);          // Zoom
glm::mat4 view = camera.getViewMatrix();  // Para shaders
```

**Problema conocido**: Fórmula de rotación matriz puede estar mal en ciertos ángulos.

### Logger (src/Logger.h/Logger.cpp)
Sistema centralizado de logging.

```cpp
Logger::init("game.log", Logger::Level::DEBUG);
Logger::info("Message");    // [INFO] Message
Logger::warning("Message"); // [WARNING] Message
Logger::error("Message");   // [ERROR] Message
Logger::debug("Message");   // [DEBUG] Message
```

---

## 🔧 Tareas Comunes

### Agregar un Nuevo Control
1. Editar `src/InputManager.h` - agregar variable a `InputState`
2. Editar `src/InputManager.h` - actualizar en método `update()`
3. Editar `src/main.cpp` - procesar en método `update()`

### Cambiar Comportamiento de Cámara
1. Editar `src/CameraSystem.h` - modificar fórmulas
2. Compilar: `.\build.ps1`
3. Probar en game

### Agregar Configuración Nueva
1. Editar `src/Config.h` - agregar constante
2. Editar `src/main.cpp` - usar en lógica

---

## 🧪 Testing

### Test Manual
1. Compilar: `.\build.ps1`
2. Ejecutar: `.\build\3DEngine.exe`
3. Verificar visualmente

### Test Automatizado (Próximo)
```cpp
// archivo: tests/test_camera.cpp (POR CREAR)
#include "CameraSystem.h"

void test_camera_rotation() {
    CameraSystem cam;
    cam.rotate(90.0f);
    assert(cam.getRotation() == 90.0f);
}
```

### Uso de Logs para Debugging
```cpp
// En InputManager.h update():
Logger::debug("Mouse Delta: " + std::to_string(mouseDeltaX) + ", " + std::to_string(mouseDeltaY));
Logger::debug("Right Mouse Pressed: " + std::to_string(mouseRightPressed));

// En main.cpp update():
Logger::debug("Q pressed: " + std::to_string(input.keyQ));
Logger::debug("E pressed: " + std::to_string(input.keyE));
```

---

## ⚠️ Problemas Conocidos a Arreglar

### 1. Mouse Rotation Not Working
**Status**: NEEDS_INVESTIGATION  
**Location**: src/main.cpp (mouse rotation logic)  
**Evidence**: User reports right-click drag doesn't rotate camera  
**Next Step**: Add Logger::debug() calls in InputManager mouse delta calculation

### 2. Q and E Still Active
**Status**: NEEDS_FIX  
**Location**: src/main.cpp line ~395 (Q/E processing)  
**Issue**: Q/E rotate camera but should be disabled  
**Solution**: Remove or comment out Q/E key processing

**Quick Fix Structure**:
```
/memories/session/ISSUES.md  ← Document problem
↓
src/Logger.cpp               ← Add debug logging
↓
src/main.cpp                 ← Review code with Logger output
↓
Fix → Build → Test → Commit
```

---

## 🚀 Workflow Rápido para Arreglar Bugs

### Paso 1: Documentar el Bug
Crear archivo `docs/GUIDES/DEBUGGING_SESSIONS.md` o `/memories/session/ISSUE_NAME.md`

### Paso 2: Agregar Logging
```cpp
// Revisar datos relevantes
Logger::debug("Input state...");
Logger::debug("Camera state...");
```

### Paso 3: Compilar y Ejecutar
```powershell
.\build.ps1
.\build\3DEngine.exe
# Ver outputde debug
type game.log
```

### Paso 4: Identificar Root Cause
- Revisar logs
- Revisar código
- Crear hipótesis

### Paso 5: Fix y Commit
```powershell
# Hacer cambio
git add -A
git commit -m "fix: descripción del arreglo"
git push
```

---

## 📚 Documentación Requerida para Features Nuevas

Cuando agregues una feature, documenta:

1. **QUÉ** - Descripción breve
2. **DÓNDE** - Ubicación en código
3. **CÓMO** - Cómo usarlo
4. **TESTING** - Cómo testear
5. **KNOWN_ISSUES** - Problemas conocidos

Ejemplo:
```markdown
## Generate Random Forest Button

**QUÉ**: Botón que crea bosque procedural  
**DÓNDE**: src/main.cpp renderGameScene()  
**CÓMO**: Click en "Generate Random Forest"  
**TESTING**: Verificar que genera 120 objetos  
**ISSUES**: Colores pueden no ser correctos  
```

---

## 🎯 Checklist para Nueva Feature

- [ ] Código compilado sin errores
- [ ] Código compilado sin warnings
- [ ] Testado manualmente
- [ ] Logs agregados para debugging
- [ ] Documentado en código (comentarios)
- [ ] Mensaje de commit descriptivo
- [ ] Pushed a GitHub

---

## 📞 Preguntas Frecuentes

**P: ¿Cómo compilar rápido?**  
R: `.\build.ps1` - hace build incremental

**P: ¿Cómo ver logs?**  
R: Ver `game.log` en directorio de build

**P: ¿Cómo testear manualmente?**  
R: Ejecutar juego y verificar visualmente

**P: ¿Cómo arreglar compile errors?**  
R: Ver output de compilación, agregar includes si es necesario

---

**Última actualización**: 2026-04-11  
**Versión**: 1.0  
**Mantenedor**: Development Team

