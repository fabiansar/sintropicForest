# 👨‍💻 Guía para Nuevos Desarrolladores

## 🎯 Bienvenido

Si acabas de llegar a este proyecto, este documento te ayudará a entender la codebase y contribuir efectivamente. **No necesitas ser un experto en C++ o gráficos 3D** — todo está documentado.

---

## 📚 Estructura de la Documentación

### Para Aprender (COMIENZA AQUÍ)
1. **Este archivo** (estás aquí) — Introducción general
2. [QUICK_START.md](QUICK_START.md) — Compilar y ejecutar
3. [README.md](README.md) — Descripción del proyecto
4. [ESTRUCTURA_DEL_PROYECTO.md](ESTRUCTURA_DEL_PROYECTO.md) — Cómo se organiza el código

### Para Entender Técnicamente
5. [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md) — Explicación step-by-step del código existente
6. [ARQUITECTURA_ESCALABLE.md](ARQUITECTURA_ESCALABLE.md) — Patrones y diseño futuro

### Para Extender
7. [EXTENSIBILIDAD_Y_ESCALABILIDAD.md](EXTENSIBILIDAD_Y_ESCALABILIDAD.md) — Cómo agregar características

### Para Jugar
8. [GESTION_3D.md](GESTION_3D.md) — Controles del juego
9. [CONTROLES_FPS.md](CONTROLES_FPS.md) — Sistema de cámara

---

## 🚀 Quick Start (5 minutos)

### Paso 1: Compilar
```bash
cd c:\Users\elfab\Documents\IA
powershell .\build.ps1
```

**Resultado esperado**:
```
Configuring done
Generating done
Building project...
[3/3] Linking CXX executable 3DEngine.exe
Build complete!
```

### Paso 2: Ejecutar
```bash
.\build\3DEngine.exe
```

**Qué deberías ver**:
- Splash screen: "Made by Fabian" (3 segundos)
- Menú principal: 3 botones (Play, Settings, Credits)
- Click "Play": Terreno verde/marrón con grilla

### Paso 3: Jugar
- **Click izquierdo**: Plantar (Hierba, Arbusto, Árbol aleatorio)
- **WASD**: Mover vista rápido (rotado con cámara)
- **Flechas**: Mover vista lento
- **Q/E**: Rotar cámara
- **ESC**: Volver al menú

---

## 📂 Mapa de Archivos

### Archivos principales que debes conocer
```
src/
├── main_new.cpp
│   └─ Punto de entrada. Solo 60 líneas, muy simple.
│      Crea GraphicsEngine, loop principal, limpia.
│
├── GraphicsEngine.h
│   └─ Interfaz (qué hace GraphicsEngine)
│      Declara métodos públicos y privados
│      Declara variables miembro
│
├── GraphicsEngine.cpp
│   └─ Implementación (cómo lo hace)
│      700+ líneas con toda la lógica
│      - initialize(): Preparar OpenGL
│      - update(): Lógica cada frame
│      - render(): Dibujar cada frame
│
├── Config.h
│   └─ Todas las configuraciones en un lugar
│      Constantes, enums, structuras
│      - NUM_LIGHTS, MAX_LIGHTS
│      - PlantType enum
│      - Struct Light (datos de plantas)
│
├── Shaders.h
│   └─ Código GLSL (gráficos)
│      2 programas:
│      - vertexShaderSource: Calcula posición 3D
│      - fragmentShaderSource: Calcula color
│
└── CMakeLists.txt
    └─ Configuración de compilación
       Define qué compilar, librerías a incluir
```

### Archivos de documentación
```
├── README.md
│   └─ Descripción general del proyecto
│
├── QUICK_START.md
│   └─ Cómo compilar y ejecutar
│
├── ESTRUCTURA_DEL_PROYECTO.md
│   └─ Explicación de la reorganización modular
│
├── DOCUMENTACION_TECNICA.md
│   └─ Deep dive: cómo funciona cada función
│
├── ARQUITECTURA_ESCALABLE.md
│   └─ Cómo diseñar para escalabilidad
│      Patrones de diseño, refactorización
│
├── EXTENSIBILIDAD_Y_ESCALABILIDAD.md
│   └─ Cómo agregar nuevas características
│      Ideas de extensiones, roadmap
│
├── GESTION_3D.md
│   └─ Guía del juego: qué es cada cosa
│
└── CONTROLES_FPS.md
    └─ Cómo funciona la cámara
```

---

## 🧠 Conceptos Básicos

### ¿Qué es OpenGL?
**OpenGL** = "Open Graphics Library" — librería para dibujar gráficos 3D

En este proyecto usamos OpenGL 3.3 Core Profile (versión 3.3, moderna y simple)

Flujo básico:
```
CPU (C++) ───ejecuta loops───> GPU (Shaders GLSL)
          
        glDrawArrays()
        |
        v
   GPU ejecuta shaders
   para cada vértice
        |
        v
   Resultado: pixels en pantalla
```

### ¿Qué es un Shader?
**Shader** = Programa que corre en GPU

En este proyecto tenemos 2:
- **Vertex Shader**: Determina posición final de cada punto
- **Fragment Shader**: Determina color de cada pixel

### ¿Qué es un Punto de Luz?
A pesar del nombre "_Light_" en el código, en realidad son **plantas**.

Estructura:
```cpp
struct Light {  // En realidad debería ser "Plant"
    glm::vec3 position;   // X, Y, Z en el mundo
    glm::vec3 color;      // RGB (rojo, verde, azul)
    glm::vec3 velocity;   // Qué tan rápido se mueve (ahora siempre 0)
    int type;             // GRASS (0), BUSH (1), TREE (2)
};
```

---

## 🔍 Flujo de Ejecución Principal

### Cada Frame (60 veces por segundo):

```
┌─────────────────────────────────────┐
│ 1. handleInput()                    │  Lee teclas, mouse
│    - WASD, Q, E, ESC, clicks        │
└─────────────────────────────────────┘
           |
           v
┌─────────────────────────────────────┐
│ 2. update(deltaTime)                │  Actualiza lógica
│    - Mueve plantas (aunque no...)   │  deltaTime = tiempo desde último frame
│    - Gestiona estados (splash→menu) │
│    - Actualiza tiempo transcurrido  │
└─────────────────────────────────────┘
           |
           v
┌─────────────────────────────────────┐
│ 3. render()                         │  Dibuja todo
│    - Limpia pantalla                │
│    - Renderiza terreno              │
│    - Renderiza plantas              │
│    - Renderiza UI (ImGui)           │
│    - Intercambia buffers (double)   │
└─────────────────────────────────────┘
           |
           v
      Repetir loop
```

---

## 💻 Anatomía de Cambios Simples

### Cambio 1: Aumentar Máximo de Plantas de 500 a 1000

**Archivo**: `src/Config.h`

```cpp
// LÍNEA ACTUAL:
const int MAX_LIGHTS = 500;      // Máximo de plantas permitidas

// CAMBIAR A:
const int MAX_LIGHTS = 1000;     // Máximo de plantas permitidas
```

**Compilar**:
```bash
.\build.ps1
```

**Listo** ✓ — Ahora puedes poner 1000 plantas

### Cambio 2: Cambiar Probabilidades (80% hierba → 50% hierba)

**Archivo**: `src/Config.h`

```cpp
// ANTES:
const float PLANT_PROBABILITY_GRASS = 0.80f;
const float PLANT_PROBABILITY_BUSH = 0.15f;
const float PLANT_PROBABILITY_TREE = 0.05f;

// DESPUÉS:
const float PLANT_PROBABILITY_GRASS = 0.50f;
const float PLANT_PROBABILITY_BUSH = 0.30f;
const float PLANT_PROBABILITY_TREE = 0.20f;
```

**Compilar y ejecutar** ✓

### Cambio 3: Cambiar Color del Terreno (Marrón → Verde)

**Archivo**: `src/Shaders.h`

Busca en `terrainFragmentShaderSource`:
```cpp
// BUSCAR ESTA LÍNEA (aproximadamente línea 70):
vec3 baseColor = vec3(0.35, 0.25, 0.15);  // Marrón

// CAMBIAR A:
vec3 baseColor = vec3(0.2, 0.6, 0.2);     // Verde claro
```

También cambiar:
```cpp
vec3 darkColor = vec3(0.25, 0.18, 0.1);   // Marrón oscuro

// A:
vec3 darkColor = vec3(0.1, 0.4, 0.1);     // Verde oscuro
```

**Compilar y ejecutar** ✓

---

## 🐛 Debugging Básico

### El programa no compila

**Paso 1**: Leer el error
```
error: no member named 'foobar' in 'GraphicsEngine'
       ↑                     ↑
       variable que no existe    en dónde
```

**Paso 2**: Buscar en archivos
- Si buscas una función: `grep "foobar" src/*`
- Si agregaste una variable: verifica Config.h

**Paso 3**: Verificar sintaxis C++
- `{` debe tener `}`
- `;` al final de líneas
- `#include` para cada librería que uses

### El programa compila pero no funciona

**Verificar**:
1. ¿Se ve la splash screen? → Graphics OK
2. ¿Funciona el menú? → UI OK  
3. ¿Puedes hacer click? → Input OK
4. ¿Aparecen las plantas? → Lógica OK

**Output a consola**:
Usa `std::cout` para ver valores:
```cpp
// En GraphicsEngine.cpp, dentro de addRandomLight():
std::cout << "Plant added at: " << position.x << ", " << position.z << std::endl;
```

### El framerate es bajo

**Verificar**:
1. ¿Cuántas plantas? (panel muestra "Plants: X / 500")
2. ¿Resolución alta? (puedes cambiar en Config.h)
3. ¿Terreno muy detallado? (TERRAIN_RESOLUTION = 50)

**Optimización rápida**:
```cpp
// En Config.h, reducir resolución:
const int TERRAIN_RESOLUTION = 25;  // Era 50
```

---

## 📝 Convenciones de Código

### Nombres de Variables
```cpp
// ✅ BIEN
int plantCount;
float cameraHeight;
glm::vec3 terrainPosition;

// ❌ MAL
int pc;
float cam_h;
glm::vec3 pos;
```

### Comentarios
```cpp
// ✅ BIEN - Explica POR QUÉ, no QUÉ
// Hierba tiene 80% porque es el tipo más común
const float PLANT_PROBABILITY_GRASS = 0.80f;

// ❌ MAL - Explica QUÉ (obvio)
// Probabilidad de hierba
const float PLANT_PROBABILITY_GRASS = 0.80f;
```

### Formato
```cpp
// ✅ BIEN
void addRandomLight(const glm::vec3& position) {
    std::mt19937 gen(std::random_device{}());
    // ... resto del código
}

// ❌ MAL
void addRandomLight(const glm::vec3& position){std::mt19937 gen(std::random_device{}());...}
```

---

## 🎓 Tareas Progresivas

### Nivel 1: Entender el Código (1-2 horas)

**Objetivo**: Poder compilar y ejecutar sin cambios

**Tareas**:
- [ ] Leer Main_new.cpp (es muy simple)
- [ ] Leer Config.h (solo constantes y defines)
- [ ] Leer ESTRUCTURA_DEL_PROYECTO.md
- [ ] Compilar sin errores
- [ ] Ejecutar y jugar 5 minutos

---

### Nivel 2: Cambios Simples (2-4 horas)

**Objetivo**: Hacer cambios pequeños en Config.h y Shaders.h

**Tareas**:
- [ ] Cambiar MAX_LIGHTS a 2000
- [ ] Cambiar probabilidades (50/30/20 en lugar de 80/15/5)
- [ ] Cambiar color de terreno
- [ ] Cambiar tamaños de plantas (en renderGameScene)
- [ ] Agregar nuevo tamaño para un tipo de planta

**Resultado**: 5 cambios funcionales compilados

---

### Nivel 3: Leer Implementación (4-8 horas)

**Objetivo**: Entender GraphicsEngine.cpp

**Tareas**:
- [ ] Leer initialize() completo (explicado en DOCUMENTACION_TECNICA.md)
- [ ] Entender initializeLights()
- [ ] Entender generateTerrain()
- [ ] Entender getRaycastHit() (conversión 2D→3D)
- [ ] Entender addRandomLight() (generación de plantas)
- [ ] Entender handleInput() (lectura de teclado/mouse)
- [ ] Entender update()
- [ ] Entender render() y renderGameScene()

**Resultado**: Comprender el flujo completo

---

### Nivel 4: Crear Nueva Característica (8-16 horas)

**Objetivo**: Implementing an idea from EXTENSIBILIDAD_Y_ESCALABILIDAD.md

**Opciones Fáciles**:
- [ ] Add zoom with scroll wheel
- [ ] Add right-click delete tool
- [ ] Add keyboard shortcuts for camera presets

**Opciones Medias**:
- [ ] Add plant age and growth
- [ ] Add simple life/death cycle
- [ ] Add undo/redo system

**Resultado**: Código nuevo, compilado, funcional

---

## 🤝 Contribución Guidelines

### Antes de Cambiar Código

1. **Entiende el código existente**
   - Leer comentarios existentes
   - Buscar ejemplos similares
   - Preguntar en comentarios si no estás seguro

2. **Planifica tu cambio**
   - ¿Qué archivos necesito cambiar?
   - ¿Puedo hacerlo sin modificar código existente?
   - ¿Necesito crear nuevas clases o funciones?

3. **Escribe código limpio**
   - Variables con nombres descriptivos
   - Comentarios que expliquen POR QUÉ
   - Formato consistente

### Al Hacer Cambios

4. **Compila frecuentemente**
   - Después de cada método nuevo
   - Si no compila, arréglalo inmedatamente

5. **Prueba tu código**
   - Asegúrate que funciona
   - Verifica casos límite (máximo de plantas, etc.)

6. **Documenta tu código**
   - Comentarios en funciones nuevas
   - Actualiza documentación si la cambia

### Después de Cambios

7. **Verifica que no rompiste nada**
   - El menú aún funciona?
   - El juego aún funciona?
   - Aún compila sin warnings?

8. **Actualiza documentación**
   - Si cambias estructura: actualiza docs
   - Si agregas feature: documenta en README

---

## 📞 Getting Help

### Errores de Compilación

**Tipo**: "Undefined reference to `xyz`"
- **Causa**: Función declarada en .h pero no implementada en .cpp
- **Solución**: Implementar en GraphicsEngine.cpp o dónde corresponda

**Tipo**: "No member named `xyz` in class"
- **Causa**: Variable/método no declarado en .h
- **Solución**: Agregar en GraphicsEngine.h

### Errores de Runtime

**Tipo**: "Segmentation fault" o "crash"
- **Causa**: Acceder a memoria inválida
- **Solución**: Verificar punteros, limites de arrays

**Tipo**: "Black screen"
- **Causa**: Shaders con error o inicialización incompleta
- **Solución**: Checks de error en initialize()

---

## ✅ Checklist para Nuevo Desarrollador

- [ ] Leí esta guía completamente
- [ ] Leí QUICK_START.md  
- [ ] Compilé exitosamente
- [ ] Ejecuté el programa
- [ ] Jugué por 5 minutos
- [ ] Entiendo la estructura de archivos
- [ ] Sé dónde cambiar configuraciones
- [ ] Sé dónde agregar características
- [ ] Sé dónde reportar bugs

---

## 🎯 Próximos Pasos

### Si quieres aprender más:
1. Lee [DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md)
2. Leer comentarios en GraphicsEngine.cpp
3. Hace cambios simples y experimenta

### Si quieres contribuir:
1. Elige tarea de Nivel 1-2
2. Implementa y prueba
3. Documenta cambios
4. Push a repository

### Si quieres especialización:
1. Lee [ARQUITECTURA_ESCALABLE.md](ARQUITECTURA_ESCALABLE.md)
2. Planifica refactorización
3. Implementa por fases
4. Documenta nuevas clases

---

## 📚 Recursos Externos

### C++
- [cppreference.com](https://en.cppreference.com/) — Referencia del lenguaje
- [C++ Best Practices](https://isocpp.org/) — Estándares

### OpenGL
- [Learn OpenGL](https://learnopengl.com/) — Tutorial interactivo
- [Khronos OpenGL Wiki](https://www.khronos.org/opengl/) — Oficial

### Librerías
- [GLM Docs](https://glm.g-truc.net/) — Matemáticas 3D
- [GLFW Docs](https://www.glfw.org/) — Ventanas
- [ImGui Demo](https://github.com/ocornut/imgui) — UI

---

**Versión**: 1.0  
**Bienvenido al equipo** 🚀  
**¡Diviértete codificando!**
