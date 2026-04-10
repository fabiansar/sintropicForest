# 🌿 3D Graphics Engine - Plant Ecosystem Simulator

Una aplicación 3D interactiva que simula un ecosistema de plantas. Construida con **C++ 17** y **OpenGL 3.3 Core Profile**, demostrando arquitectura escalable para juegos y simulaciones.

## 🎮 Características

- ✨ **Motor Gráfico Profesional**: OpenGL 3.3, GLFW, ImGui
- 🌱 **Simulador de Plantas**: 3 tipos (Hierba, Arbusto, Árbol) con generación probabilística
- 🎯 **Interactividad**: Click para plantar, WASD para mover, cámara isométrica
- 🏗️ **Arquitectura Escalable**: Código modular, fácil de extender
- 📚 **Documentación Profesional**: 8 guías técnicas para desarrolladores
- 🎨 **Interfaz Intuitiva**: Menú principal, configuración, créditos

## 🚀 Quick Start

### Requisitos
- Windows 10+
- C++ 17 compatible compiler (LLVM/Clang recomendado)
- CMake 3.0+
- Git (para clonar)

### Compilar

```powershell
cd c:\Users\elfab\Documents\IA
powershell .\build.ps1
```

**Resultado esperado**: "Build complete!" sin errores ✓

### Ejecutar

```powershell
.\build\3DEngine.exe
```

## 🎮 Controles del Juego

| Acción | Efecto |
|--------|--------|
| **Click Izquierdo** | Plantar en terreno |
| **WASD** | Mover vista (rápido, rotado con cámara) |
| **Flechas** | Mover vista (lento) |
| **Q/E** | Rotar cámara |
| **ESC** | Volver al menú |

## 📁 Estructura del Proyecto

```
src/
├── main_new.cpp              ← Punto de entrada (60 líneas)
├── GraphicsEngine.h/cpp      ← Motor gráfico (coordinador)
├── Config.h                  ← Configuración centralizada
├── Shaders.h                 ← Código GLSL
└── CMakeLists.txt            ← Build configuration

docs/
├── QUICK_START.md            ← Compilación rápida
├── README.md                 ← Este archivo
├── ARQUITECTURA_ESCALABLE.md ← Diseño y patrones
├── EXTENSIBILIDAD_Y_ESCALABILIDAD.md ← Cómo agregar features
├── GUIA_PARA_NUEVOS_DESARROLLADORES.md ← Onboarding
├── DOCUMENTACION_TECNICA.md  ← Deep dive del código
├── ESTRUCTURA_DEL_PROYECTO.md ← Explicación modular
├── GESTION_3D.md            ← Guía del generador
└── CONTROLES_FPS.md         ← Sistema de cámara
```

## 💡 Para Nuevos Desarrolladores

**Comienza aquí**: [GUIA_PARA_NUEVOS_DESARROLLADORES.md](GUIA_PARA_NUEVOS_DESARROLLADORES.md)

- ✅ Entender arquitectura (15 min)
- ✅ Hacer cambios simples (30 min)
- ✅ Agregar características (1-2 horas)

## 🏗️ Arquitectura

### Actual (Monolítica)
```
GraphicsEngine (700+ líneas)
├─ Inicialización OpenGL
├─ Renderizado
├─ Lógica de juego
├─ Gestión de entrada
└─ Actualización
```

### Propuesta (Escalable)
```
main.cpp (coordinador)
├─ GameManager (lógica)
├─ EntityManager (entidades)
├─ Renderer (gráficos)
├─ Camera (cámara)
├─ InputManager (entrada)
└─ EventSystem (comunicación)
```

Ver: [ARQUITECTURA_ESCALABLE.md](ARQUITECTURA_ESCALABLE.md)

## 🔧 Configuración Rápida

Abre `src/Config.h`:

```cpp
const int MAX_LIGHTS = 500;              // Máximo de plantas
const float PLANT_PROBABILITY_GRASS = 0.80f;  // 80% hierba
const float PLANT_PROBABILITY_BUSH = 0.15f;   // 15% arbusto
const float PLANT_PROBABILITY_TREE = 0.05f;   // 5% árbol
```

**Cambiar probabilidades o máximo y compilar** = Listo ✓

## 📈 Extensiones Propuestas

### Fáciles (0.5-1 hora)
- [ ] Zoom con scroll wheel
- [ ] Delete con botón derecho
- [ ] Sistema de sonido básico

### Intermedias (2-4 horas)
- [ ] Ciclo de vida de plantas
- [ ] Terreno con Perlin noise
- [ ] Undo/Redo

### Avanzadas (8+ horas)
- [ ] Ecosistema con propagación
- [ ] Multijugador
- [ ] Editor de tipos de plantas

Ver: [EXTENSIBILIDAD_Y_ESCALABILIDAD.md](EXTENSIBILIDAD_Y_ESCALABILIDAD.md)

## 🛠️ Tecnologías

| Componente | Tecnología | Versión |
|---|---|---|
| **Lenguaje** | C++ | 17 |
| **Gráficos** | OpenGL | 3.3 Core |
| **Ventana/Input** | GLFW | 3.4 |
| **Matemáticas** | GLM | 1.0.3 |
| **UI** | ImGui | 1.92.7 |
| **Build** | CMake | 3.0+ |
| **Compilador** | LLVM/Clang | 22.1.3 |

## 📊 Estadísticas del Código

| Métrica | Valor |
|---------|-------|
| **Líneas de C++** | ~800 |
| **Líneas de GLSL** | ~100 |
| **Líneas de docs** | ~3000+ |
| **Archivos fuente** | 5 |
| **Clases principales** | 1 (GraphicsEngine) |
| **Compilación** | ~3-5 segundos |

## ✅ Checklist de Desarrollo

### Build System
- [x] CMake configurado
- [x] Dependencias instaladas (GLFW, GLAD, GLM, ImGui)
- [x] PowerShell build script
- [x] Compilación correcta

### Runtime
- [x] Splash screen funciona
- [x] Menú principal completo
- [x] Juego ejecutable
- [x] Generación de plantas
- [x] Raycast picking
- [x] UI con información

### Documentación
- [x] README actualizado
- [x] QUICK_START
- [x] Guía para nuevos devs
- [x] Arquitectura documentada
- [x] Escalabilidad definida
- [x] Extensiones propuestas

## 🐛 Solución de Problemas

### El programa no compila
```
❌ C++ compiler no disponible
✅ Solución: Instalar LLVM/Clang desde https://releases.llvm.org/

❌ CMake no encontrado
✅ Solución: Agregar a PATH: C:\Program Files\CMake\bin

❌ OpenGL error
✅ Solución: Actualizar driver de GPU
```

### El programa funciona pero lento
```
❌ Muchas plantas (ej: 2000)
✅ Solución: Reducir MAX_LIGHTS en Config.h

❌ Terreno muy detallado
✅ Solución: Reducir TERRAIN_RESOLUTION en Config.h
```

Ver: [GUIA_PARA_NUEVOS_DESARROLLADORES.md#debugging](GUIA_PARA_NUEVOS_DESARROLLADORES.md)

## 📚 Documentación Completa

1. **[QUICK_START.md](QUICK_START.md)** — Empezar rápido (5 min)
2. **[GUIA_PARA_NUEVOS_DESARROLLADORES.md](GUIA_PARA_NUEVOS_DESARROLLADORES.md)** — Onboarding (30 min)
3. **[ESTRUCTURA_DEL_PROYECTO.md](ESTRUCTURA_DEL_PROYECTO.md)** — Organización (20 min)
4. **[DOCUMENTACION_TECNICA.md](DOCUMENTACION_TECNICA.md)** — Deep Dive (1-2 horas)
5. **[ARQUITECTURA_ESCALABLE.md](ARQUITECTURA_ESCALABLE.md)** — Diseño (1 hora)
6. **[EXTENSIBILIDAD_Y_ESCALABILIDAD.md](EXTENSIBILIDAD_Y_ESCALABILIDAD.md)** — Expansión (30 min)
7. **[GESTION_3D.md](GESTION_3D.md)** — Sistema de juego (15 min)
8. **[CONTROLES_FPS.md](CONTROLES_FPS.md)** — Cámara 3D (10 min)

**Tiempo total de lectura**: ~4-5 horas para dominar completamente

## 🎯 Objetivos del Proyecto

- ✅ Demostrar arquitectura escalable de C++
- ✅ Implementar motor gráfico 3D moderno
- ✅ Crear UI profesional
- ✅ Documentar extensamente para aprendizaje
- ✅ Facilitar mantenimiento y extensión
- ❓ Crear juego divertido (bonus)

## 🚀 Roadmap

### v1.0 (ACTUAL)
- ✅ 3 tipos de plantas
- ✅ Terreno simple
- ✅ Sistema de clics
- ✅ Cámara isométrica
- ✅ UI con menú

### v1.1 (Próxima)
- 🔄 Zoom suave
- 🔄 Herramienta delete
- 🔄 Estadísticas mejoradas

### v2.0 (Futuro)
- 🔄 Ciclo de vida completo
- 🔄 Terreno procedural
- 🔄 Editor de plantas
- 🔄 Multijugador local

### v3.0+ (Visión)
- 🔄 Mundo masivo
- 🔄 Multijugador online
- 🔄 Modding API
- 🔄 Plataformas móviles

## 💬 Contribución

Estamos buscando colaboradores para:
- Agregar features de Roadmap
- Mejorar documentación
- Optimizar rendimiento
- Crear ejemplos

Ver: [GUIA_PARA_NUEVOS_DESARROLLADORES.md#contribución-guidelines](GUIA_PARA_NUEVOS_DESARROLLADORES.md)

## 📝 Licencia

Este proyecto está documentado con fines educativos y demostración de buenas prácticas de ingeniería de software en C++/OpenGL.

---

**Última actualización**: 2026-04-11  
**Versión**: 1.0  
**Mantenedor**: Fabian (Framework creador)
**Estado**: Activo y mantenido