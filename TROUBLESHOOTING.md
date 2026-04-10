# 🆘 Troubleshooting Guide - Guía de Solución de Problemas

**Última actualización**: Abril 2026  
**Versión**: 1.0

Centralizado troubleshooting para sintropicForest. Para reportar nuevos problemas, consulta [CONTRIBUTING.md](#).

---

## 📚 Tabla de Contenidos

1. [Problemas de Compilación](#compilación)
2. [Errores en Tiempo de Ejecución](#runtime)
3. [Problemas de Rendimiento](#rendimiento)
4. [Entrada y Controles](#entrada)
5. [Gráficos y Rendering](#gráficos)
6. [Arquitectura y Diseño](#arquitectura)

---

## 🔴 COMPILACIÓN

### ❌ "CMake not found"

**Síntomas**: 
```
cmake : El término 'cmake' no se reconoce...
```

**Solución**:
```powershell
# Opción 1: Instalar via winget
winget install Kitware.CMake

# Opción 2: Agregar a PATH
$env:Path += ";C:\Program Files\CMake\bin"

# Opción 3: Ejecutar desde instalador
C:\Program Files\CMake\bin\cmake.exe ..
```

**Verificar**:
```powershell
cmake --version
```

---

### ❌ "No compiler found" / "No such file or directory: cl.exe"

**Síntomas**:
- Error durante CMake configure
- "Visual C++ compiler not available"
- g++ o clang++ no disponibles

**Solución**:
```powershell
# Opción 1: Instalar MinGW (recomendado para Windows)
winget install BrechtSanders.WinLibs

# Opción 2: Instalar LLVM MinGW
winget install MartinStorsjo.LLVM-MinGW

# Opción 3: Instalar Visual Studio Community
# https://visualstudio.microsoft.com/downloads/
# → Desktop development with C++ workload
```

**Verificar**:
```powershell
g++ --version        # G++ desde MinGW
clang++ --version    # Clang
cl.exe               # Visual C++
```

---

### ❌ "Error: Multiple definitions of symbol 'X'"

**Síntomas**:
```
[Error] multiple definitions of 'GraphicsEngine::initialize()' first defined here
```

**Causa**: Header include guards missing o inclusiones duplicadas

**Solución**:
```cpp
// ✅ CORRECTO - Siempre en headers
#pragma once

class GraphicsEngine { ... };

// ❌ INCORRECTO - Nunca en headers
struct Light { ... };  // Colocar en Config.h
```

**Revisar**:
- [ ] Todos los `.h` tienen `#pragma once`
- [ ] No incluir `.cpp` desde otros archivos
- [ ] Verificar CMakeLists.txt no agrega mismo archivo dos veces

---

### ❌ "Linking errors" / "undefined reference to"

**Síntomas**:
```
undefined reference to 'GraphicsEngine::render()'
undefined reference to 'glCreateShader'
```

**Soluciones checklist**:

1. **Verificar CMakeLists.txt**
```cmake
# ✅ Incluir TODOS los .cpp (NO .h)
add_executable(BasicGame src/main_new.cpp src/GraphicsEngine.cpp)

# ✅ Linkear librerías
target_link_libraries(BasicGame glfw glad OpenGL::OpenGL)
```

2. **Verificar archivos compilados**
```powershell
cd build
cmake ..   # Reconfigurar
cmake --build . --clean-first  # Limpiar y recompilar
```

3. **Verificar dependencias instaladas**
```powershell
# Instalar dependencias
vcpkg install glfw:x64-windows glad:x64-windows glm:x64-windows
```

---

## 🔴 RUNTIME

### ❌ Ventana no abre / "Failed to create GLFW window"

**Síntomas**:
- Programa cierra instantáneamente
- Mensaje: "Failed to initialize GLFW"

**Solución**:
```cpp
// Verificar en GraphicsEngine::initialize()
if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    // → Instalar drivers gráficos
    // → Verificar GLFW instalado correctamente
    return false;
}
```

**Checklist**:
- [ ] Drivers gráficos actualizados
- [ ] DirectX 11+ instalado (Windows)
- [ ] GLFW 3.3+ debe estar en path

---

### ❌ Terreno completamente negro / plantas invisibles

**Síntomas**:
- Pantalla negra total
- No se ven plantas ni terreno
- Input funciona (ESC abre menú)

**Causas comunes**:
1. **Cámara mal posicionada**
2. **Shaders no compilados correctamente**
3. **Depth buffer problemas**

**Solución**:
```cpp
// Agregar debug en GraphicsEngine::initialize()
std::cout << "Camera position: " << glm::to_string(cameraPos) << std::endl;
std::cout << "Shader program: " << shaderProgram << std::endl;

// Verificar render
glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  // Rojo temporalmente
```

---

### ❌ Rendimiento muy bajo / FPS bajo (< 30 FPS)

**Síntomas**:
- Mucho lag / retraso
- Cámara no responde bien
- Plantas parpadean

**Solución por pasos**:

1. **Reducir resolución terreno**
```cpp
// En Config.h
const int TERRAIN_RESOLUTION = 25;  // Reducir de 50
```

2. **Reducir máximo de plantas**
```cpp
const int MAX_LIGHTS = 250;  // Reducir de 500
```

3. **Desactivar Perlin Noise**
```cpp
// En GraphicsEngine::generateTerrain()
float posY = 0.0f;  // Terreno plano temporalmente
// posY = (noiseValue - 0.5f) * TERRAIN_NOISE_HEIGHT;
```

4. **Proflear**
```cpp
auto start = std::chrono::high_resolution_clock::now();
// código a medir
auto end = std::chrono::high_resolution_clock::now();
auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "Time: " << dur.count() << " ms" << std::endl;
```

---

## 🔴 ENTRADA Y CONTROLES

### ❌ Click no funciona / no planta

**Síntomas**:
- Clic izquierdo no hace nada
- Las plantas no se crean

**Verificar**:
```cpp
// En GraphicsEngine::handleInput()
// Comprobar si PLAYING state
if (currentState == PLAYING) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // Debe crear planta aquí
        std::cout << "Click!" << std::endl;  // Debug
    }
}
```

**Soluciones**:
1. Verificar `currentState` es `PLAYING` (ESC y vuelve a entrar)
2. Verificar límite de plantas no alcanzado: `lights.size() < MAX_LIGHTS`
3. Verificar click válido: no fuera de pantalla

---

### ❌ Cámara no responde / WASD no funciona

**Síntomas**:
- Teclas WASD sin efecto
- Q/E (rotación) sin efecto
- Solo ESC funciona

**Causa**: Probablemente `currentState != PLAYING`

**Verificar**:
```cpp
std::cout << "Current state: " << currentState << " (should be 2 for PLAYING)" << std::endl;

// En handleInput()
if (currentState == PLAYING) {  // ← Verificar esta condición
    // ...
}
```

---

## 🔴 GRÁFICOS Y RENDERING

### ❌ Shaders no compilan / error 0 en pantalla

**Síntomas**:
```
Shader compilation failed: [error message]
Program linking failed: [error message]
```

**Solución**:
```cpp
// En GraphicsEngine.cpp - compileShader()
if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "Shader compilation failed: " << infoLog << std::endl;  // ← Ver qué error
    
    // Common shader errors:
    // - Sintaxis GLSL incorrecta
    // - Uniform no declarado
    // - Varying vs in/out mismatch (GLSL 330)
}
```

**Verificar GLSL versión**:
```glsl
#version 330 core  // ← Debe coincidir con contexto pedido
```

---

### ❌ Plantas que parpadean / aparecen y desaparecen

**Síntomas**:
- Plantas parpadean cada frame
- Desaparecen cuando no ves directamente

**Causa**: Frustum culling o Z-fighting

**Solución**:
```cpp
// Verificar depth test
glEnable(GL_DEPTH_TEST);
glClearBufferfv(GL_DEPTH, 0, glm::value_ptr(glm::vec4(1.0f)));
```

---

## 🔴ARQUITECTURA

### ❌ "¿Cómo agrego un nuevo tipo de planta?"

**Respuesta completa**: Ver [ARQUITECTURA_ESCALABLE.md - Agregar nuevo PlantType](ARQUITECTURA_ESCALABLE.md#agregar-nuevoplanttype)

**Pasos rápidos**:
1. Agregar `SHRUB = 3` en `Config.h` enum PlantType
2. Agregar probabilidad `PLANT_PROBABILITY_SHRUB` en Config.h
3. Agregar geometría en `PerlinNoise.h` `PlantGeometry::GenerateShrub()`
4. Agregar caso en `GraphicsEngine::generatePlantGeometry()`
5. Compilar y probar

---

### ❌ "¿Por qué GraphicsEngine es tan grande?"

**Explicación**: Diseño monolítico actual. Ver ARQUITECTURA_ESCALABLE.md para refactorización propuesta.

**Solución futura**: Separar en:
- `Renderer` (renderizado)
- `GameLogic` (lógica)
- `InputManager` (entrada)
- `EntityManager` (plantas)

---

## 📞 ¿No encontraste tu problema?

1. Revisar [GUIA_REFERENCIA.md](GUIA_REFERENCIA.md) para una guía general
2. Revisar [GUIA_PARA_NUEVOS_DESARROLLADORES.md](GUIA_PARA_NUEVOS_DESARROLLADORES.md) para arquitectura
3. Crear un issue en GitHub: https://github.com/fabiansar/sintropicForest/issues
4. Ver QUICK_START.md para pasos de compilación básicos

---

**Última actualización**: Abril 2026  
**Mantenedor**: Fabian SR  
**Licencia**: MIT
