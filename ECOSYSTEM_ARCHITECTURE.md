# 🌿 Arquitectura de Simulación Ecológica

## 🎯 Visión General

Sistema completo de simulación de sucesión ecológica basado en agroforestería. Las plantas experimentan un ciclo de vida natural: germinación → crecimiento → reproducción → madurez → envejecimiento → muerte. El sistema modeliza interacciones complejas entre especies.

---

## 🏗️ Componentes Principales

### 1. **EcosystemSimulator** (Controller Central)
Coordina toda la simulación ecológica.

```cpp
update(deltaTime)
├─ updatePlantLife()          // Envejecimiento y ciclos
├─ updateEnvironmentalStress() // Luz, agua, nutrientes
├─ checkSuccession()          // Transformaciones (hierba→arbusto→árbol)
├─ disperseSeeds()           // Reproducción
├─ resolveCompetition()       // Interacciones
└─ updateInteractions()       // Simbiosis, alelopatía
```

### 2. **SpeciesData** (Configuración por Especie)
Define comportamiento único de cada tipo de planta.

| Parámetro | Hierba | Arbusto | Árbol |
|-----------|--------|---------|--------|
| Lifespan | 30s | 80s | 180s |
| Max Height | 0.5m | 2.5m | 6.0m |
| Seed Production | Alta | Media | Baja |
| Light Min | 0.3 | 0.4 | 0.2 |

### 3. **PlantLifeStage** (Estados)
- **SEEDLING** (0-20%): Germinación, vulnerable
- **JUVENILE** (20-50%): Crecimiento rápido
- **ADULT** (50-85%): Reproducción activa
- **SENESCENT** (85-100%): Envejecimiento, decline
- **DEAD**: Biomasa muerta (descomposición)

### 4. **PlantData** (Estado Individual)
Cada planta mantiene su propio estado ecológico.

---

## 🌱 Dinámicas Ecológicas Implementadas

### A. Ciclo de Vida Realista

**Crecimiento sigmoide** (S-curve natural):
```
Height = MaxHeight × t² × (3 - 2t)
```
Donde `t` es la edad normalizada (0-1).

**Producción de semillas**:
- Solo plantas ADULT con health > 0.5
- Acumulación continua en `seedBankAccumulated`
- Dispersión aleatoria dentro de radio

### B. Sucesión Ecológica

**Transformaciones (solo si condiciones se cumplen)**:

1. **Hierba → Arbusto**
   - Condiciones: Stage=ADULT, health > 0.8, age > 15s
   - Activador: Cercanía a árboles (sombra)
   - Prepara terreno para especies más competitivas

2. **Arbusto → Árbol**
   - Condiciones: Stage=ADULT, health > 0.7, age > 30s
   - Probabilidad: 2% por segundo
   - Representa maduración y estabilización

### C. Estrés Ambiental

**Factores de stress**:
```
Total Stress = Light Stress + Water Stress + Nutrient Stress
```

**Efectos del stress**:
- Reduce `healthFactor` (0.1-1.0)
- Reduce tamaño visual (`currentHeight`, `currentRadius`)
- Si stress > 1.0: Probabilidad de muerte

### D. Competencia por Recursos

**Luz**:
- Plantas más altas proyectan sombra
- Reduce `localLight` de vecinas cercanas
- Effect gradual basado en distancia

**Agua y Nutrientes**:
- Consumidas por cada planta
- Redistribuidas globalmente
- Regeneración natural lenta

### E. Interacciones Planta-Planta

**Simbiosis (Arboles + Leguminosas)**:
- Distancia < 5m
- Árbol gana +0.01 health/frame
- Simula fijación de nitrógeno

**Alelopatía** (opcional, no implementada aún):
- Algunas plantas inhiben otras
- Reduce health de competidoras

---

## 📊 Sistema de Estrés Detallado

### Cálculo de Stress Local

```cpp
// Para cada factor ambiental:
StringForLight = abs(OptimalLight - LocalLight) × Weight

// Ejemplo hierba con LocalLight=0.2:
LightStress = (0.3 - 0.2) × 2.0 = 0.2
```

### Cascada de Efectos

```
High Stress (> 1.0)
├─ healthFactor decrece
├─ Crecimiento más lento
├─ Producción de semillas cesa
└─ Probabilidad de muerte aumenta
```

---

## 🔄 Loop de Simulación

```
Frame(deltaTime)
  ├─ Acumular tiempo = deltaTime × simulationSpeed
  ├─ Si tiempo >= updateInterval (0.1s)
  │  ├─ Para cada planta:
  │  │  ├─ Envejecer (age += dt)
  │  │  ├─ Determinar stage de vida
  │  │  ├─ Calcular crecimiento visual (sigmoide)
  │  │  ├─ Producir semillas si adulta
  │  │  ├─ Evaluar estrés ambiental
  │  │  ├─ Modificar health según stress
  │  │  ├─ Revisar sucesión ecológica
  │  │  ├─ Dispersar semillas
  │  │  └─ Verificar si muerte
  │  ├─ Resolver competencia
  │  ├─ Aplicar interacciones
  │  └─ Eliminar plantas muertas
  └─ Renderizar estado final
```

---

## 📈 Estadísticas del Ecosistema

### `EcosystemStats`

- **grazeCount, bushCount, treeCount**: Conteo por especie
- **averageHealth**: Salud promedio (0-1)
- **biodiversity**: Shannon Index (mayor = más diverso)
- **biomass**: Suma de alturas (proxy de productividad)

---

## 🎮 Integración con GameLogic

**Cambios recomendados en GameLogic.h**:

```cpp
class GameLogic {
private:
    EcosystemSimulator ecosystem;  // Agregar
    std::vector<Plant> plants;      // Mantener para compatibilidad
    
public:
    void update(float deltaTime) {
        ecosystem.update(deltaTime);
        // Sincronizar PlantData → Plant para renderizado
        syncEcosystemToPlants();
    }
    
    void addPlant(const glm::vec3& pos, PlantType type) {
        ecosystem.addPlant(pos, type);
    }
};
```

---

## ⚙️ Parámetros Configurables

### Global (Por Frame)
- `globalLight` (0-1): Nubosidad/estación
- `globalWater` (0-1): Lluvia/sequía
- `globalNutrients` (0-1): Fertilidad del suelo
- `simulationSpeed` (multiplicador): 1.0 = real, 2.0 = 2x

### Por Especie
- Todos en `SpeciesData`: minGrowthTime, lifespan, etc.
- Editable en `initializeSpeciesData()`

### Físicos
- `maxPlants`: Límite (defensa de OOM)
- `updateInterval`: 0.1s (precision vs performance)

---

## 🚀 Roadmap de Extensiones

### Phase 1 (Actual)
✅ Ciclo de vida básico
✅ Sucesión ecológica
✅ Dispersión de semillas
✅ Estrés por luz/agua/nutrientes

### Phase 2 (Próximo)
- [ ] Clima dinámico (lluvias, sequías, estaciones)
- [ ] Terreno heterogéneo (fertilidad variable)
- [ ] Plagas y enfermedades
- [ ] Polinización por insectos

### Phase 3 (Long-term)
- [ ] Ciclos biogeoquímicos (C, N, P)
- [ ] Degradación de suelo
- [ ] Sistemas de raíces y competencia subterránea
- [ ] Microclimas (temperatura local)

---

## 📝 Notas de Implementación

### Performance
- Update cada 0.1s en lugar de cada frame
- Culling de plantas fuera de vista
- Update espacial por grid si > 1000 plantas

### Precisión
- Curva sigmoide para crecimiento (más natural que lineal)
- Probabilidades estocásticas para sucesión
- Actualización incremental de stress

### Validación
- Clamping de valores (0-1 para health, light, etc.)
- Bounds checking en dispersión de semillas
- Manejo seguro de dead plants

---

## 🎓 Conceptos Ecológicos Reales Modelizados

| Concepto | Implementación |
|----------|----------------|
| **Sucesión ecológica** | Transformaciones de tipo según edad/stress |
| **Competencia** | Sombra reduce luz local |
| **Simbiosis** | Árboles + arbustos cercanos |
| **Dispersión** | Radio aleatorio con germination chance |
| **Ciclo de vida** | 5 stages (seedling-dead) |
| **Fecundidad edad-dependiente** | Seeds solo en ADULT |
| **Senescencia** | Health decrece al final de vida |

---

**Versión**: 1.0  
**Estado**: Implementado y compilable  
**Próximo paso**: Integrar con GameLogic y testar dinámicas
