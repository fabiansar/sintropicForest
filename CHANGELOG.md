# 📝 Changelog - Historial de Cambios

**Formato**: [Semantic Versioning](https://semver.org/)  
**Archivo**: Sigue [Keep a Changelog](https://keepachangelog.com/)

---

## [1.0-RC2] - 2026-04-11

### ✨ Nuevo

- **🎨 Documentación Mejorada**
  - ✅ `TROUBLESHOOTING.md` - Guía centralizada de solución de problemas
  - ✅ `API_REFERENCE.md` - Referencia completa de clases y métodos
  - ✅ `STATUS.md` - Matriz de estado de componentes
  - ✅ Diagramas Mermaid en `ARQUITECTURA_ESCALABLE.md`

- **🌱 Sistema de Plantas Procedural**
  - ✅ Geometría procedural 3D (cilindro, esfera, cono)
  - ✅ Escala aleatoria (0.8-1.3x)
  - ✅ Rotación Y randomizada
  - ✅ Colores mejorados por tipo

- **🗻 Terreno Realista**
  - ✅ Perlin Noise multi-octava (4 octavas)
  - ✅ Heightmap dinámico
  - ✅ Altura variable (-2.5 a +2.5 unidades)
  - ✅ Parámetros configurables en Config.h

- **⚡ Optimizaciones**
  - ✅ Geometry caching para plantas (VAO/VBO por tipo)
  - ✅ Matrix caching framework (preparación futura)
  - ✅ Perlin Noise sample O(1)

### 🔧 Mejorado

- **📚 Documentación**
  - Unificación de troubleshooting en un solo doc
  - Clarificación de arquitectura monolítica vs propuesta
  - Mejor organización de referencias

- **🎮 Experiencia**
  - Plantas con más variedad visual
  - Terreno más interesante y realista
  - HUD mejorado con conteo de plantas

### 🐛 Corregido

- Performance con 100+ plantas mejorado
- Renderizado de geometría optimizado
- Caché de shaders
- Memoria leak en cleanup

### 🔥 Breaking Changes

- `struct Light` con nuevos campos (`scale`, `rotation`)
  - Anterior código que crea lights debe actualizar

### 📋 Documentación

- Added: TROUBLESHOOTING.md (completo)
- Added: API_REFERENCE.md (completo)
- Added: STATUS.md (matrix de componentes)
- Updated: README.md (referencias nuevos docs)
- Updated: ARQUITECTURA_ESCALABLE.md (diagramas Mermaid)

### 🧩 Nuevo en el Código

- `src/PerlinNoise.h` - Generador Perlin + PlantGeometry

---

## [1.0-RC1] - 2026-04-09

### ✨ Nuevo

- **🎮 Sistema de Juego Completo**
  - Menús (Splash, Main Menu, Settings, Credits)
  - Escena 3D jugable con terreno
  - Sistema de plantas con 3 tipos
  - Cámara isométrica funcional

- **🎨 Gráficos**
  - OpenGL 3.3 Core
  - Shaders para terreno y plantas
  - Renderizado de puntos de luz (glow effect)
  - ImGui UI

- **🎮 Entrada**
  - WASD + Flechas para movimiento
  - Q/E para rotación cámara
  - Click izquierdo para plantar
  - ESC para menús

- **📚 Documentación**
  - 10+ guías técnicas
  - Ejemplos de modificación
  - Guía para nuevos desarrolladores
  - Referencia de arquitectura

### 🏗️ Arquitectura

- Clase `GraphicsEngine` como coordinador
- Config.h centralizado
- Separación de shaders en `Shaders.h`
- main_new.cpp limpio (60 líneas)

### 🐛 Conocidos

- Parpadeo ocasional con 100+ plantas
- Terreno completamente plano (solución: Perlin Noise →  RC2)
- Sin física/colisiones
- Sin sonido

---

## [0.9-BETA] - 2026-03-15

### ✨ Inicial

- Proyecto scaffold básico
- Primer commit a GitHub
- Documentación de arquitectura
- Sistema de menús prototipo

---

## 🗺️ Roadmap

### v1.0 Estable (Próximo mes)

- [ ] Refactorización arquitectura (Entity System)
- [ ] Tests unitarios
- [ ] Más variantes de plantas
- [ ] Sistema de sonido

### v1.1 (2 meses)

- [ ] Física básica
- [ ] Ecosistema simples
- [ ] Save/Load
- [ ] Steam release?

### v2.0 Vision

- [ ] Multijugador local
- [ ] Campaign mode
- [ ] Modding support
- [ ] VR support?

---

## 📊 Estadísticas de Versiones

| Versión | Fecha | Cambios | Líneas Código | Documentación |
|---------|-------|---------|---------------|---------------|
| 1.0-RC2 | Apr 11 | +8 files +463 lines | 835 | +5 docs +8000 words |
| 1.0-RC1 | Apr 09 | +5 files | 372 | 10 docs |
| 0.9-BETA | Mar 15 | Initial | ~200 | 5 docs |

---

## 🤝 Contribuidores

- **Fabian SR** - Desarrollador principal

---

## 📄 Licencia

MIT - Ver LICENSE file

---

## 📞 Soporte

- 🐛 Bugs: [GitHub Issues](https://github.com/fabiansar/sintropicForest/issues)
- 💬 Preguntas: Ver TROUBLESHOOTING.md
- 📖 Documentación: README.md, GUIA_PARA_NUEVOS_DESARROLLADORES.md

---

**Próxima release**: May 2026  
**EOL (End of Life)**: TBD
