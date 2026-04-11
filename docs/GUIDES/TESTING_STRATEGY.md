# 🧪 Testing Strategy & Plan

Guía completa de testing para el proyecto.

## 📋 Estrategia de Testing

### Niveles de Testing

```
┌─────────────────────────────────────┐
│   UNIT TESTS                        │ ← test_camera.cpp, test_input.cpp
│   (Funciones individuales)          │
└─────────────────────────────────────┘
            ↓
┌─────────────────────────────────────┐
│   INTEGRATION TESTS                 │ ← Camera + Input + Logger
│   (Sistemas interconectados)        │
└─────────────────────────────────────┘
            ↓
┌─────────────────────────────────────┐
│   MANUAL TESTS                      │ ← Jugar y verificar
│   (Pruebas manuales en el juego)    │
└─────────────────────────────────────┘
```

---

## 1️⃣ Unit Tests (Framework en tests/)

### Location
- `tests/test_framework.h` - Framework minimalista
- `tests/test_camera.cpp` - Tests del sistema de cámara
- `tests/test_input.cpp` - Tests del sistema de entrada (POR CREAR)
- `tests/test_logger.cpp` - Tests del logger (POR CREAR)

### Cómo Escribir Tests

```cpp
// Declarar test
TEST(SystemName, "Description of what is tested") {
    // Arrange: preparar datos
    CameraSystem cam;
    
    // Act: ejecutar funcionamiento
    cam.rotate(45.0f);
    
    // Assert: verificar resultado
    ASSERT_EQUAL(cam.getRotation(), 45.0f, "Rotation should be 45");
}
```

### Macros Disponibles

```cpp
ASSERT_TRUE(condition, "message")           // Verifica que sea true
ASSERT_FALSE(condition, "message")          // Verifica que sea false
ASSERT_EQUAL(actual, expected, "message")   // Verifica igualdad
ASSERT_NOT_EQUAL(actual, expected, "message") // Verifica desigualdad
ASSERT_THROW(expression, "message")         // Verifica que lance excepción
```

### Ejemplo Completo

```cpp
TEST(CameraSystem, "Zoom adjusts properly") {
    CameraSystem cam;
    
    // Obtener distancia inicial
    glm::vec3 pos1 = cam.getPosition();
    float dist1 = glm::distance(cam.getTarget(), pos1);
    
    // Zoom in (negativo = acercarse)
    cam.adjustDistance(-5.0f);
    
    glm::vec3 pos2 = cam.getPosition();
    float dist2 = glm::distance(cam.getTarget(), pos2);
    
    // Verificar que la distancia disminuyó
    ASSERT_TRUE(dist2 < dist1, "Camera should be closer after zoom in");
}
```

---

## 2️⃣ Integration Tests

Combinación de múltiples sistemas.

### Ejemplo: Input → Camera

```cpp
// test_integration.cpp
TEST(InputCamera, "Mouse input rotates camera") {
    // Simular input
    InputManager input(window);
    
    // Simular ratón en posición X
    glfwSetCursorPos(window, 100.0, 100.0);
    input.update();
    
    // Mover ratón a la derecha (delta positivo)
    glfwSetCursorPos(window, 150.0, 100.0);
    input.update();
    
    const auto& state = input.getInputState();
    
    // Verificar que se calculó delta
    ASSERT_TRUE(state.mouseDeltaX > 0.0, "Mouse should move right");
    ASSERT_EQUAL(state.mouseDeltaY, 0.0, "Y should not change");
}
```

---

## 3️⃣ Manual Tests (In-Game)

Pruebas visuales y funcionales en el juego mismo.

### Test Checklist

#### Camera Tests
- [ ] Camera rotates smoothly with right-click drag
- [ ] W/A/S/D move camera in correct directions
- [ ] UP/DOWN zoom in/out properly
- [ ] Camera stays at reasonable distance
- [ ] Settings slider changes rotation speed (0.1 - 2.0)

#### Input Tests
- [ ] W key moves forward
- [ ] S key moves backward
- [ ] A key moves left
- [ ] D key moves right
- [ ] Q key (SHOULD NOT rotate) ← **BUG: Still active**
- [ ] E key (SHOULD NOT rotate) ← **BUG: Still active**
- [ ] Right-click drag rotates ← **BUG: Not working**
- [ ] UP/DOWN arrow zoom

#### UI Tests
- [ ] "Generate Random Forest" button works
- [ ] Settings menu opens
- [ ] Volume slider works
- [ ] Rotation speed slider works
- [ ] ESC returns to menu

#### Performance Tests
- [ ] Game runs 60+ FPS with 120 objects
- [ ] No memory leaks (check Task Manager)
- [ ] Smooth camera movement

---

## 🚀 Cómo Ejecutar Tests

### Opción 1: Build y ejecutar en consola (TODO)

```powershell
# Compilar tests solo
g++ tests/test_camera.cpp -I. -std=c++17 -o tests/run_tests

# Ejecutar
.\tests\run_tests
```

Resultado esperado:
```
=== Running Tests ===

[TEST] CameraSystem :: Rotation updates correctly ... ✓ PASSED
[TEST] CameraSystem :: Rotation wraps at 360 ... ✓ PASSED
[TEST] CameraSystem :: Negative rotation works ... ✓ PASSED
...

=== Test Results ===
Passed: 7
Failed: 0
Total:  7
Rate:   100%
```

### Opción 2: Tests manuales (ACTUAL)

1. Compilar: `.\build.ps1`
2. Ejecutar: `.\build\3DEngine.exe`
3. Seguir checklist manual
4. Anotar bugs/issues

### Opción 3: Debug con Logger (RECOMENDADO AHORA)

```powershell
# 1. Editar main.cpp para Logger::init en PLAYING state
# Logger::init("game.log", Logger::Level::DEBUG);

# 2. Compilar
.\build.ps1

# 3. Ejecutar y probar features
.\build\3DEngine.exe

# 4. Ver logs
type game.log | Select-Object -Last 50  # Últimas 50 líneas
```

---

## 🐛 Debugging with Logger

### Añadir Logging a Sistema

```cpp
// En InputManager.h update():
Logger::debug("Raw mouse pos: (" + std::to_string(newMouseX) + 
              ", " + std::to_string(newMouseY) + ")");
Logger::debug("Mouse delta: (" + std::to_string(currentState.mouseDeltaX) + 
              ", " + std::to_string(currentState.mouseDeltaY) + ")");
Logger::debug("Right mouse: " + std::to_string(currentState.mouseRightPressed));

// En main.cpp update():
if (input.keyQ || input.keyE) {
    Logger::debug("WARNING: Q or E key pressed! Q=" + std::to_string(input.keyQ) +
                  ", E=" + std::to_string(input.keyE));
}

if (input.mouseRightPressed) {
    Logger::debug("Camera rotating by: " + 
                  std::to_string(input.mouseDeltaX * cameraRotationSpeed));
}
```

### Analizar Logs

```powershell
# Ver últimas 20 líneas
type game.log | Select-Object -Last 20

# Filtrar por palabra clave
Select-String "ERROR|WARNING" game.log

# Contar logs por nivel
(Select-String "ERROR" game.log).Count
(Select-String "WARNING" game.log).Count
```

---

## 📊 Test Coverage

| Sistema | Unit Tests | Integration | Manual | Status |
|---------|-----------|-------------|--------|--------|
| CameraSystem | ✓ (7 tests) | ⬜ | ⬜ | GOOD |
| InputManager | ⬜ | ⬜ | ⬜ | TODO |
| Logger | ⬜ | ⬜ | ⬜ | TODO |
| Main Game Loop | ⬜ | ⬜ | ✓ | PARTIAL |
| UI/ImGui | ⬜ | ⬜ | ✓ | PARTIAL |

---

## 🎯 Testing Priorities

### URGENT (arreglar bugs)
1. [ ] Test Q/E keyboard state with Logger
2. [ ] Test mouse delta calculation with Logger
3. [ ] Test right mouse button state with Logger
4. [ ] Identify root causes from logs

### HIGH (próximas sesiones)
1. [ ] Create test_input.cpp unit tests
2. [ ] Create integration test for Input+Camera
3. [ ] Create test suite runner

### MEDIUM (futuro)
1. [ ] Add CI/CD (GitHub Actions)
2. [ ] Add code coverage reporting
3. [ ] Add performance benchmarking

---

## 📝 Next Steps

1. **Ahora**: Use Logger to debug Q/E y mouse issues
2. **Sesión próxima**: Implement input unit tests
3. **Futuro**: Add CI/CD integration

---

**Testing Framework Status**: READY  
**Unit Tests**: PARTIAL (Camera only)  
**Integration Tests**: NOT STARTED  
**Manual Tests**: IN PROGRESS  

