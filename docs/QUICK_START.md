# 🚀 Quick Start - 5 Minutos

Compila y ejecuta **SintropicForest** en menos de 5 minutos.

---

## 1️⃣ Requisitos Previos

- **Windows 10/11**
- **Visual Studio 2019+ o Clang 22.1.3**
- **CMake 3.10+**
- **Git**

Recomendado:
- **vcpkg** (para manejar dependencias)
- **Ninja** (build generator más rápido)

---

## 2️⃣ Clonar Repositorio

```powershell
git clone https://github.com/fabiansar/sintropicForest.git
cd sintropicForest
```

---

## 3️⃣ Compilar (Automático)

### Opción A: PowerShell Script (Recomendado)

```powershell
.\build.ps1
```

✅ Automáticamente:
- Configura CMake
- Detecta compilador (Clang autodetectado)
- Compila con Ninja
- Genera 3DEngine.exe

### Opción B: Manual

```powershell
cd build
cmake ..
ninja
```

---

## 4️⃣ Ejecutar

```powershell
.\build\3DEngine.exe
```

O desde el directorio build:
```powershell
cd build
.\3DEngine.exe
```

---

## 🎮 Controles Básicos

| Input | Acción |
|-------|--------|
| **Click Izquierdo** | Plantar hierba/arbusto/árbol |
| **Click Derecho** | Eliminar planta más cercana |
| **WASD** | Mover vista (según ángulo cámara) |
| **Flechas** | Mover vista (lento) |
| **Q / E** | Rotar cámara |
| **ESC** | Abrir menú |
| **Scroll** | Zoom in/out |

---

## 🎨 Interfaz

- **Menú Principal**: ESC para acceder
- **Settings**: Configurar probabilidades de plantas
- **Créditos**: Ver información del proyecto
- **Game View**: Espacio principal para plantar y jugar

---

## ✅ Verificación

Después de ejecutar, deberías ver:

```
✓ Ventana OpenGL 3.3+
✓ Terreno (puntos verdes) 
✓ Interfaz ImGui (menú, settings, info)
✓ Cámara isométrica interactiva
```

---

## 🐛 Troubleshooting

### Error: "No se encuentra CMake"
```powershell
# Instalar CMake
choco install cmake
# o descargar desde https://cmake.org/download/
```

### Error: "Clang no encontrado"
```powershell
# El script intenta detectar automáticamente
# Si falla, verifica: C:/Program Files/LLVM/bin/
```

### Error: "vcpkg packages not found"
```powershell
# Este proyecto usa vcpkg autodetectado
# Si falla, actualiza rutas en CMakeLists.txt
```

### Construcción lenta
- Use SSD (recomendado)
- Cierre programas pesados
- Primera compilación es más lenta (~2-3 min)

---

## 📊 Compilación Esperada

```
Configuring project with CMake...
-- Detecting C compiler ABI info - done
-- Found OpenGL: opengl32
-- Configuring done (2.3s)

Building project...
[1/5] Building CXX object src/main_new.cpp.obj
[2/5] Building CXX object src/GraphicsEngine.cpp.obj
[3/5] Building CXX object src/EcosystemSimulator.cpp.obj
[4/5] Building CXX object src/ParticleAtomSystem.cpp.obj
[5/5] Linking CXX executable 3DEngine.exe

Build complete!
```

**Tiempo esperado**: 30-60 segundos (primera vez)

---

## 📁 Estructura de Archivos

```
sintropicForest/
├── src/                    ← Código fuente C++
│   ├── main_new.cpp
│   ├── GraphicsEngine.cpp
│   ├── ParticleAtomSystem.cpp
│   ├── EcosystemSimulator.cpp
│   └── *.h                 ← Headers
├── build/                  ← Archivos compilados
│   └── 3DEngine.exe        ← Ejecutable final
├── docs/                   ← Esta documentación
├── CMakeLists.txt          ← Configuración build
├── build.ps1               ← Script compilación
└── README.md               ← Overview general
```

---

## 🎯 Próximos Pasos

1. **[Documentación Completa](./INDEX.md)** - Guías detalladas
2. **[Controles](./REFERENCE/CONTROLS.md)** - Todas las teclas
3. **[Arquitectura](./ARCHITECTURE/OVERVIEW.md)** - Entender el sistema
4. **[Integración](./GUIDES/INTEGRATION.md)** - Añadir nuevas características

---

## 💡 Tips

- ⚡ Primera compilación genera código optimizado después
- 🎮 Prueba planting: click izq en terreno, verás plantas crecer
- 🔄 ESC abre menú: ajusta probabilidades de tipos de plantas
- 💬 Consulta [./INDEX.md](./INDEX.md) para documentación completa

---

**¡Listo para jugar! Disfruta del simulador de ecosistema 🌿**

