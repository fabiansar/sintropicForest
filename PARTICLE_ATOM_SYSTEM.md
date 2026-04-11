# Particle Atom System - Architecture & Design

**Date:** April 11, 2026  
**Status:** Design Specification + Core Implementation  
**Scope:** Universal atomic representation for all game objects

---

## 1. Core Philosophy

### The Atom as Universal Unit

Every physical element in the game—terrain, plants, objects, breakable structures—is composed of **atoms**:
- **Atoms** are the fundamental particles (vertices/points)
- **Triangles** are faces formed by connecting 3 atoms
- **Meshes** are complete structures (plants, terrain patches)
- **Connections** define what stays together

### Why Atoms?

1. **Destruction**: Break atoms apart = visible destruction
2. **Pruning**: Remove leaf atoms = plant pruning
3. **Degradation**: Convert any atoms to SOIL = natural decomposition
4. **Modularity**: Each component can be independently damaged/modified
5. **Scalability**: Add/remove atoms without rebuilding entire structure
6. **Physics**: Individual atoms can have velocity, forces, collision

---

## 2. Atom Types

### Four-Type System (Extensible)

```
┌─ AtomType ─────────────────────────┐
│ SOIL        → Terrain, decomposed  │
│ STEM        → Plant structure      │
│ LEAF        → Foliage              │
│ BRANCH      → Higher-order support │
│ WATER (TBD) → Liquid particles     │
└────────────────────────────────────┘
```

#### Type Characteristics

| Type | Use Cases | Health | Color | Density |
|------|-----------|--------|-------|---------|
| **SOIL** | Terrain, mulch, decomposed plants | ∞ | Brown | High |
| **STEM** | Plant central axis (all types) | 0-1 | Green-brown | Medium |
| **LEAF** | Herbaceous, bush, tree foliage | 0-1 | Bright green | Low |
| **BRANCH** | Bush and tree structural support | 0-1 | Dark brown | Medium |

#### Type Combinations Generate Plant Types

- **GRASS** = STEM + LEAF (simple radial)
- **BUSH** = STEM + BRANCH + LEAF (multi-level)
- **TREE** = STEM + BRANCH + LEAF (complex hierarchy)

---

## 3. Data Structures

### Atom Structure

```cpp
struct Atom {
    // Identity
    uint32_t id;                           // Unique within mesh
    AtomType type;                         // What material is this
    
    // Physics
    glm::vec3 position;                    // World coordinates
    glm::vec3 velocity;                    // For falling/physics
    
    // Connectivity
    uint32_t parentMeshId;                 // Which structure owns this
    std::vector<uint32_t> connectedAtomIds; // Direct neighbors
    std::vector<uint32_t> triangleIndices; // Which triangles use this
    
    // State
    float health;                          // 0-1, structural integrity
    float degradationTimer;                // Time until SOIL conversion
    bool isScheduledForRemoval;            // Mark for cleanup
    
    // Rendering
    glm::vec4 color;                       // For visualization
};
```

### AtomicTriangle Structure

```cpp
struct AtomicTriangle {
    std::array<uint32_t, 3> atomIds;       // Three atom references
    glm::vec3 normal;                      // Face normal (precomputed)
    float breakThreshold;                  // Health threshold to break
    bool isIntact;                         // All three atoms present?
};
```

### AtomMesh Structure

```cpp
struct AtomMesh {
    // Composition
    std::vector<Atom> atoms;               // All atoms in structure
    std::vector<AtomicTriangle> triangles; // All faces
    
    // Identity
    uint32_t id;
    std::string name;
    PlantType structureType;               // GRASS/BUSH/TREE
    
    // Metrics
    float integrityScore;                  // 0-1, % intact
    glm::vec3 centerOfMass;
    float totalMass;
    
    // Configuration
    float breakResistance;                 // 0.0-1.0, how hard to break
    float degradationRate;                 // Seconds to degrade to SOIL
};
```

---

## 4. Three-Stage Generation

### Stage 1: Procedural Structure Generation

**AtomGenerator** creates structures with proper connectivity:

#### Herbaceous Plant
```
        Leaf
         |
        Leaf-STEM-Leaf
         |
        Leaf
         |
       ROOT
```

- **Process**:
  1. Generate main stem (4-6 segments)
  2. At each segment, attach 2-3 leaves radially
  3. Connect stem segments in sequence
  4. All atoms start with `health = 1.0f`

#### Bush
```
        Leaf
         |
    Leaf-BRANCH-Leaf
       /    |    \
   BRANCH BRANCH BRANCH
      |      |      |
   STEM - STEM - STEM
       \    |    /
         ROOT
```

- **Process**:
  1. Generate main trunk (5-7 segments)
  2. At intervals, branch out (2-3 primary branches)
  3. Each branch has 2-3 secondary branches
  4. Attach leaves to all branch endpoints

#### Tree
```
           Leaf cloud
          /     |     \
       CROWN - CROWN - CROWN
        |      |      |
      BRANCH-BRANCH-BRANCH
         \     |     /
          \    |    /
            TRUNK
```

- **Process**:
  1. Generate thick trunk (8-10 segments)
  2. At crown level, create dense leaf cloud (30+ atoms)
  3. Branch pattern creates realistic tapering
  4. Leaves form spherical crown

### Stage 2: Connection Establishment

Each generated structure:
1. Establishes `connectedAtomIds` vectors (bidirectional)
2. Creates triangle references (each atom notes which triangles it's in)
3. Sets `breakThreshold` per triangle (e.g., 0.3 = breaks if any atom ≤ 30% health)
4. Calculates `integrityScore` (percentage of intact atoms)

### Stage 3: Integration into ParticleAtomSystem

```cpp
auto mesh = ParticleAtomSystem::createPlantMesh(
    position,           // World position
    PlantType::TREE,    // Type
    1.5f                // Scale multiplier
);

system.addMesh(mesh);   // Now part of simulation
```

---

## 5. Breaking & Destruction System

### Connection Breaking

Two atoms can break their connection:

```cpp
bool AtomOperations::breakConnection(mesh, atomId1, atomId2) {
    // Remove atomId2 from atomId1's connectedAtomIds
    // Remove atomId1 from atomId2's connectedAtomIds
    // Triangle faces now have incomplete vertices (rendering handles)
}
```

**Result**: Visible separation, but both atoms still in mesh (for now)

### Atom Separation (Pruning)

Remove an atom from its structure:

```cpp
auto separated = AtomOperations::separateAtom(mesh, leafAtomId);
// Returns: vector of atom IDs that were separated
```

**Behavior**:
1. Disconnect atom from all neighbors
2. Check for orphaned components (atoms with no path to root)
3. Mark detached atoms for removal
4. **Optional**: Cascade separation (breaking one branch might break supporting structure)

**Example: Leaf Pruning**
```
ORIGINAL              AFTER PRUNING
   Leaf                  Leaf
    |                   (removed)
  STEM         →         STEM
    |                      |
  Branch                  Branch
```

### Integrity Cascades

When an atom falls below health threshold:

1. **Structural failure**: Any triangle with all atoms < 0.3f breaks
2. **Load redistribution**: Remaining atoms share stress
3. **Component separation**: Detached portions fall away
4. **Chain reactions**: Major branch loss can cascade upward

```cpp
// Simulate axe damage to tree trunk
damageAtom(treeMeshId, trunkAtomId, 0.4f);
// trunk.health = 0.6f
// Then next frame...
removeDetachedComponents(mesh);
// Upper branches fall if trunk health < support threshold
```

---

## 6. Degradation System

### Natural Decomposition

Over time, any non-SOIL atom converts to SOIL:

```
t=0         t=150s      t=300s
[STEM]────→[STEM, 50%]─→[SOIL]
```

**Configuration per mesh**:
- `degradationRate` = seconds for full decomposition
- Can be varied: herbaceous (60s) vs tree (600s)
- Adjustable per species in EcosystemSimulator

### Manual Degradation

Convert entire mesh to soil (final decomposition):

```cpp
system.degradeMesh(meshId);
// All atoms: type = SOIL, health = 1.0f
// All connections broken
// Result: particles fall and settle as terrain
```

### Two-Stage Decay Process

**Stage 1: Structural Decomposition**
- Leaf atoms → SOIL (10-20 seconds)
- Branch atoms → SOIL (30-60 seconds)
- Stem atoms → SOIL (60-120 seconds)
- Creates temporal layering in visual feedback

**Stage 2: Particle Settlement**
- SOIL atoms accumulate on terrain
- Physics simulation lets them settle
- Visual result: enriched soil pile at plant base
- Can be used for growth (nutrient cycling)

---

## 7. Integration with EcosystemSimulator

### PlantData ↔ AtomMesh Mapping

```cpp
struct PlantData {
    //... existing fields ...
    uint32_t atomMeshId;  // Reference to its AtomMesh
};
```

### Lifecycle Simulation

```
EcosystemSimulator::simulateGrowth(plant)
    ↓
    If plant is growing:
        • Increase plant.size (0→1)
        • Scale ALL atoms in associated mesh
        • Gradually add new leaf/branch atoms
    ↓
    If plant is declining:
        • Reduce plant.health
        • Mark atoms for damage
        • Phase atoms to SOIL
```

### Pruning Interaction

```
User right-clicks on plant
    ↓
    Ray-cast hits leaf atom
    ↓
    separateAtom(meshId, leafAtomId)
    ↓
    Detached leaf falls and degrades
    ↓
    EcosystemSimulator::simulateReproduction()
        → Trigger regrowth dynamics
```

---

## 8. Rendering Strategy

### Efficient Triangle Rendering

```cpp
// Pseudo-code for rendering all meshes
for each mesh in ParticleAtomSystem::meshes:
    for each triangle in mesh:
        if all 3 atoms are present:
            vertex[0] = atom[triangle.atomIds[0]].position
            vertex[1] = atom[triangle.atomIds[1]].position
            vertex[2] = atom[triangle.atomIds[2]].position
            
            // Use blended color from all three atoms
            color = (atom[0].color + atom[1].color + atom[2].color) / 3
            
            renderTriangle(vertex, normal, color)
        else:
            // Partial triangle - fade or skip
            alpha = (intact_count / 3.0f)
            renderTriangle(vertex, normal, color * alpha)
```

### Performance Considerations

- **Vertex Buffer Update**: Every frame, update VBO with current atom positions
- **Degenerate Triangles**: Triangles with removed atoms automatically degenerate
- **LOD System**: Hide atom meshes beyond draw distance
- **Batching**: Group all same-type atoms for efficient rendering

### Visual Feedback

1. **Health → Color Gradient**
   - Green (health=1.0) → Yellow (0.5) → Brown (0.0)

2. **Breaking → Particle Emission**
   - Each separated atom emits dust/particle effect

3. **Degradation → Transparency Fade**
   - SOIL-type atoms appear semi-transparent
   - Fade to blend into terrain

---

## 9. Scalability & Performance

### Design for Scale

**Target capability**: 10,000+ atoms simultaneously

**Optimization strategies**:

1. **Spatial Hashing**: Neighbor queries use grid instead of O(n²)
   ```cpp
   std::unordered_map<uint64_t, std::vector<AtomMesh*>> spatialGrid;
   // Hash 3D position to grid cell
   ```

2. **Multithreading**: Physics simulation on separate thread
   ```cpp
   DeferredOperationQueue degradationQueue;
   // Collect degradation decisions, batch-apply next frame
   ```

3. **Object Pooling**: Reuse atom/triangle allocations
   ```cpp
   ObjectPool<Atom> atomPool;
   ObjectPool<AtomicTriangle> trianglePool;
   ```

4. **Deferred Cleanup**: Don't delete immediately
   - Mark `isScheduledForRemoval`
   - Batch cleanup at frame end

### Memory Footprint

| Component | Size | Count | Total |
|-----------|------|-------|-------|
| Atom struct | 160 bytes | 10,000 | 1.6 MB |
| Triangle struct | 28 bytes | 30,000 | 840 KB |
| Mesh struct | 200 bytes | 500 | 100 KB |
| **Total** | | | **2.5 MB** |

---

## 10. Advanced Features (Roadmap)

### Phase 1 (Current Spec)
- ✅ Four atom types with connectivity
- ✅ Procedural generation (herbaceous, bush, tree, terrain)
- ✅ Basic separation and degradation
- ✅ Integration with EcosystemSimulator

### Phase 2 (Planned)
- 🔄 Water atom type for rain/irrigation
- 🔄 Soil nutrient tracking per atom
- 🔄 Symbiosis visualization (atom color changes)
- 🔄 Fractal damage propagation

### Phase 3 (Future)
- ❌ Atomic fusion (two atoms merge = growth)
- ❌ Animated atom migration (nutrient transport)
- ❌ Water flow physics through soil
- ❌ Microbe simulation (bacteria atoms?)

---

## 11. Integration Checklist

### ParticleAtomSystem Integration

```
✅ Header created (ParticleAtomSystem.h)
✅ Implementation complete (ParticleAtomSystem.cpp)
⚠️ Add to CMakeLists.txt (build system)
⚠️ Include in GraphicsEngine.h (rendering)
⚠️ Create update loop in GameLogic
⚠️ Add debugging visualization (atom wireframe)
⚠️ Connect to EcosystemSimulator lifecycle
```

### Data Flow

```
EcosystemSimulator
    ↓
PlantData (size, health, stage)
    ↓
ParticleAtomSystem
    ↓
AtomMesh (visual representation)
    ↓
GraphicsEngine
    ↓
Rendered triangles + atoms
```

### Configuration Required

```cpp
// Config.h additions needed:
const float ATOM_DEGRADATION_HERBACEOUS = 60.0f;   // seconds
const float ATOM_DEGRADATION_BUSH = 120.0f;
const float ATOM_DEGRADATION_TREE = 300.0f;

const float ATOM_BREAK_RESISTANCE_STEM = 0.7f;
const float ATOM_BREAK_RESISTANCE_LEAF = 0.3f;
const float ATOM_BREAK_RESISTANCE_BRANCH = 0.8f;

const uint32_t MAX_ATOMS_PER_PLANT = 200;
const uint32_t MAX_TRIANGLES_PER_PLANT = 600;
```

---

## 12. Example: Complete Lifecycle

```
               T=0s
        ┌──────────────┐
        │ Grass Atom   │
        │ Type: STEM   │
        │ Health: 1.0  │
        └──────────────┘
              │ Growth
              ↓
        ┌──────────────┐
        │ Grass (+leaves)
        │ Health: 1.0  │
        │ 8 atoms      │
        └──────────────┘
              │ Maturity (EcosystemSim)
              ↓
        ┌──────────────┐      User prunes 1 leaf
        │ Grass (mature) ←─────────────┐
        │ Health: 0.8  │               │
        │ 8 atoms      │               │
        └──────────────┘               │
              │                      Leaf removed
              ↓                   (degradeToSoil called)
        ┌──────────────┐
        │ Leaf atom    │
        │ Type: SOIL   │
        │ Falls, settles
        └──────────────┘

              T=300s
        ┌──────────────┐
        │ Remaining    │
        │ atoms        │
        │ degrade      │
        └──────────────┘
              │
              ↓
        ┌──────────────┐
        │ 8 SOIL atoms │
        │ All converted│
        │ Settles      │
        └──────────────┘
              │
              ↓
        ┌──────────────┐
        │ Soil enriched│
        │ Ready for    │
        │ next growth  │
        └──────────────┘
```

---

## 13. API Quick Reference

### Creating Structures

```cpp
ParticleAtomSystem system;

// Method 1: Use generator (automatic)
auto mesh = system.createPlantMesh(position, PlantType::TREE, 1.5f);

// Method 2: Manual mesh creation
uint32_t meshId = system.createMesh("MyPlant", PlantType::BUSH);
AtomMesh* mesh = system.getMesh(meshId);
```

### Breaking/Damaging

```cpp
// Damage individual atom
system.damageAtom(meshId, atomId, 0.3f);  // Reduce health by 30%

// Prune an atom
system.pruneAtom(meshId, leafAtomId);     // Separates from structure

// Complete degradation
system.degradeMesh(meshId);                // All → SOIL
```

### Operations

```cpp
// Check integrity
float integrScore = getMesh(meshId)->integrityScore;

// Get statistics
uint32_t totalAtoms = system.getTotalAtomCount();
float avgIntegrity = system.getAverageIntegrity();

// Query generated soil
auto soilAtoms = system.getSoilAtomsGenerated();  // This frame
```

---

## 14. File Structure

```
src/
├── ParticleAtomSystem.h           ← Header (structs, interfaces)
├── ParticleAtomSystem.cpp         ← Implementation
├── Config.h                        ← Add atom parameters
├── EcosystemSimulator.h            ← Links to PlantData
├── GameLogic.h                     ← Uses ParticleAtomSystem
├── GraphicsEngine.h/cpp            ← Renders atoms as triangles
└── CMakeLists.txt                  ← Include new .cpp file
```

---

## Conclusion

The **Particle Atom System** provides:

1. **Universal representation** for all destructible structures
2. **Modular components** (STEM, LEAF, BRANCH) optimized for specific roles
3. **Natural lifecycle** (grow → degrade → soil)
4. **Physics-enabled** individual atoms for realistic destruction
5. **Scalable architecture** supporting 10,000+ atoms simultaneously
6. **Deep integration** with EcosystemSimulator for living systems

This enables the game's core mechanic: **everything breaks, everything decomposes, everything feeds the soil for new growth.**
