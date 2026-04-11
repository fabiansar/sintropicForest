# 🌿 Documentación - SintropicForest

Bienvenido a la documentación técnica de **SintropicForest** - un simulador de ecosistema de plantas en 3D con arquitectura modular y sistema de partículas atómicas destructibles.

---

## 📚 Guías Rápidas

### Para Empezar
- **[Quick Start](./QUICK_START.md)** - Compilar y ejecutar en 5 minutos
- **[Controles](./REFERENCE/CONTROLS.md)** - Cómo usar la aplicación
- **[Configuración](./REFERENCE/CONFIGURATION.md)** - Parámetros ajustables

---

## 🏗️ Arquitectura del Sistema

| Documento | Descripción |
|-----------|------------|
| **[Overview](./ARCHITECTURE/OVERVIEW.md)** | Vista general de todos los sistemas |
| **[Modular Design](./ARCHITECTURE/MODULAR_DESIGN.md)** | Cómo están organizados los módulos |
| **[Simulación Ecológica](./ARCHITECTURE/ECOSYSTEM_SIMULATION.md)** | Motor de simulación de plantas |
| **[Sistema de Partículas Atómicas](./ARCHITECTURE/PARTICLE_ATOM_SYSTEM.md)** | Estructuras destructibles basadas en átomos |

---

## 🛠️ Guías de Implementación

### Integración
- **[Full Integration Guide](./GUIDES/INTEGRATION.md)** - Cómo conectar todos los sistemas
  - Paso 1: Build system (CMake)
  - Paso 2: GraphicsEngine
  - Paso 3: GameLogic
  - Paso 4: InputManager
  - Paso 5: EcosystemSimulator
  - Paso 6: ParticleAtomSystem

### Características
- **[Features Implemented](./GUIDES/FEATURES.md)** - Lista de características completadas
- **[Roadmap](./GUIDES/ROADMAP.md)** - Futuras mejoras planeadas

---

## 📖 Referencias

### API Reference
- **[Particle Atom API](./REFERENCE/API_PARTICLE_ATOMS.md)** - Métodos y estructuras
- **[Ecosystem API](./REFERENCE/API_ECOSYSTEM.md)** - Funciones de simulación

### Quick Reference
- **[Particle Atom Quick Ref](./REFERENCE/PARTICLE_ATOM_QUICK_REFERENCE.md)** - Cheat sheet
- **[Game Vision](./REFERENCE/GAME_VISION.md)** - Objetivo del juego

---

## 📊 Resúmenes Técnicos

- **[Particle Atom Summary](./SUMMARIES/PARTICLE_ATOM_SUMMARY.md)** - Resumen de implementación
- **[Architecture Summary](./SUMMARIES/ARCHITECTURE_SUMMARY.md)** - Resumen de diseño
- **[Implementation Status](./SUMMARIES/STATUS.md)** - Estado actual del proyecto

---

## 🔍 Índice Completo

### Por Categoría

**Documentación Original (Refactorizada)**
- Architectural Refactoring Plan → [ARCHITECTURE/OVERVIEW.md](./ARCHITECTURE/OVERVIEW.md)
- Documentation Index → [INDEX.md](./INDEX.md) (este archivo)
- Ecosystem Architecture → [ARCHITECTURE/ECOSYSTEM_SIMULATION.md](./ARCHITECTURE/ECOSYSTEM_SIMULATION.md)
- Ecosystem Integration → [GUIDES/INTEGRATION.md](./GUIDES/INTEGRATION.md)
- Features Implementation → [GUIDES/FEATURES.md](./GUIDES/FEATURES.md)
- Game Vision → [REFERENCE/GAME_VISION.md](./REFERENCE/GAME_VISION.md)
- Particle Atom System → [ARCHITECTURE/PARTICLE_ATOM_SYSTEM.md](./ARCHITECTURE/PARTICLE_ATOM_SYSTEM.md)
- Particle Atom Integration → [GUIDES/INTEGRATION.md](./GUIDES/INTEGRATION.md)
- Quick Start → [QUICK_START.md](./QUICK_START.md)

---

## 🎯 Cómo Usar Esta Documentación

### Si eres **nuevo en el proyecto**:
1. Lee [QUICK_START.md](./QUICK_START.md)
2. Juega con la aplicación
3. Lee [ARCHITECTURE/OVERVIEW.md](./ARCHITECTURE/OVERVIEW.md)

### Si necesitas **integrar nuevas características**:
1. Lee [ARCHITECTURE/OVERVIEW.md](./ARCHITECTURE/OVERVIEW.md)
2. Consulta [GUIDES/INTEGRATION.md](./GUIDES/INTEGRATION.md)
3. Revisa los ejemplos de código en [REFERENCE/](./REFERENCE/)

### Si necesitas **entender un sistema específico**:
1. Ve a [ARCHITECTURE/](./ARCHITECTURE/)
2. Selecciona el documento del sistema
3. Revisa las APIs en [REFERENCE/](./REFERENCE/)

### Si necesitas **información rápida**:
- [Controles del juego](./REFERENCE/CONTROLS.md)
- [Parámetros configurables](./REFERENCE/CONFIGURATION.md)
- [Quick reference](./REFERENCE/PARTICLE_ATOM_QUICK_REFERENCE.md)

---

## 📧 Estructura de Carpetas

```
docs/
├── INDEX.md                          ← Estás aquí
├── QUICK_START.md                    ← Comienza aquí
│
├── ARCHITECTURE/
│   ├── INDEX.md
│   ├── OVERVIEW.md
│   ├── MODULAR_DESIGN.md
│   ├── ECOSYSTEM_SIMULATION.md
│   └── PARTICLE_ATOM_SYSTEM.md
│
├── GUIDES/
│   ├── INTEGRATION.md
│   ├── FEATURES.md
│   ├── ROADMAP.md
│   └── DEBUGGING.md
│
├── REFERENCE/
│   ├── CONTROLS.md
│   ├── CONFIGURATION.md
│   ├── API_ECOSYSTEM.md
│   ├── API_PARTICLE_ATOMS.md
│   ├── GAME_VISION.md
│   └── PARTICLE_ATOM_QUICK_REFERENCE.md
│
└── SUMMARIES/
    ├── PARTICLE_ATOM_SUMMARY.md
    ├── ARCHITECTURE_SUMMARY.md
    ├── ECOSYSTEM_SUMMARY.md
    └── STATUS.md
```

---

## 🔗 Véase También

- **GitHub**: https://github.com/fabiansar/sintropicForest
- **Source Code**: `../src/` (uno nivel arriba)
- **Build Files**: `../build/` (uno nivel arriba)

---

**Última actualización:** April 11, 2026  
**Estado:** ✅ Documentación Reorganizada y Actualizada  
**Mantenedor:** Copilot Agent

---

## ¿Preguntas o Sugerencias?

Revisa la documentación pertinente o consulta el código fuente directamente,que es muy legible.

