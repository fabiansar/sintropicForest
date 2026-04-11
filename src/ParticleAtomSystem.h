#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <string>
#include "Config.h"

/**
 * @file ParticleAtomSystem.h
 * @brief Universal atom-based particle system for all game structures
 * 
 * Architecture:
 * - All structures (terrain, plants, objects) are collections of atoms
 * - Atoms are connected points that form triangles (low-poly)
 * - Each atom has a type: SOIL, LEAF, STEM, BRANCH
 * - Atoms can break/separate and degrade into SOIL
 * - Fully modular and scalable for dynamic destruction/growth
 */

namespace ParticleAtom {

// ============================================================================
// ATOM TYPES - Everything in the world is one of these
// ============================================================================

enum class AtomType : uint8_t {
    SOIL      = 0,  // Base material - terrain, degraded plants
    STEM      = 1,  // Central axis (herbaceous, bushes, trees)
    LEAF      = 2,  // Foliage (all plant types)
    BRANCH    = 3,  // Structure (bushes and trees only)
    WATER     = 4,  // For future water particles
};

// ============================================================================
// ATOM - Individual particle unit
// ============================================================================

struct Atom {
    // Identity
    uint32_t id;                           // Unique identifier within mesh
    AtomType type;                         // What kind of particle is this
    
    // Physics
    glm::vec3 position;                    // World position
    glm::vec3 velocity;                    // For physics simulation
    
    // Structure
    uint32_t parentMeshId;                 // Which mesh/structure owns this
    std::vector<uint32_t> connectedAtomIds; // IDs of directly connected atoms
    std::vector<uint32_t> triangleIndices; // Indices into global triangle list
    
    // State
    float health;                          // 0-1, how intact this atom is
    float degradationTimer;                // Time before conversion to SOIL
    bool isScheduledForRemoval;            // Mark for cleanup
    
    // Rendering
    glm::vec4 color;                       // RGBA color for rendering
    
    Atom() 
        : id(0), type(AtomType::SOIL), position(0.0f), velocity(0.0f),
          parentMeshId(0), health(1.0f), degradationTimer(0.0f), 
          isScheduledForRemoval(false), color(1.0f) {}
};

// ============================================================================
// ATOMIC TRIANGLE - Shared vertex references
// ============================================================================

struct AtomicTriangle {
    std::array<uint32_t, 3> atomIds;       // IDs of three atoms forming triangle
    glm::vec3 normal;                      // Precomputed face normal
    float breakThreshold;                  // Health threshold before breaking
    bool isIntact;                         // Whether all three atoms are present
    
    AtomicTriangle(uint32_t a, uint32_t b, uint32_t c, float threshold = 0.3f)
        : atomIds({a, b, c}), normal(0.0f), breakThreshold(threshold), isIntact(true) {}
};

// ============================================================================
// ATOM MESH - Collection of atoms forming a structure
// ============================================================================

struct AtomMesh {
    uint32_t id;                           // Mesh identifier
    std::string name;                      // Human-readable name (for debugging)
    
    // Atomic composition
    std::vector<Atom> atoms;               // All atoms in this structure
    std::vector<AtomicTriangle> triangles; // All triangles (references to atoms)
    
    // Metadata
    PlantType structureType;               // GRASS, BUSH, TREE (for plants)
    glm::vec3 centerOfMass;                // Computed center
    float totalMass;                       // Sum of all atom masses
    float integrityScore;                  // 0-1, percentage of intact atoms
    
    // Structure parameters
    float breakResistance;                 // How hard to break individual atoms (0.0-1.0)
    float degradationRate;                 // Time to degrade into soil (seconds)
    
    AtomMesh(uint32_t meshId, const std::string& meshName, PlantType type)
        : id(meshId), name(meshName), structureType(type),
          centerOfMass(0.0f), totalMass(0.0f), integrityScore(1.0f),
          breakResistance(0.5f), degradationRate(300.0f) {}
    
    // Query methods
    uint32_t getAtomCount() const { return atoms.size(); }
    uint32_t getTriangleCount() const { return triangles.size(); }
    uint32_t getIntactAtomCount() const;
    std::vector<uint32_t> getConnectedComponent(uint32_t startAtomId) const;
};

// ============================================================================
// ATOM GENERATION - Procedurally create structures
// ============================================================================

class AtomGenerator {
public:
    /**
     * Generate herbaceous plant structure (simple single stem + leaves)
     * Single stem with radial leaves
     */
    static std::unique_ptr<AtomMesh> generateHerbaceousPlant(
        uint32_t meshId,
        const glm::vec3& basePosition,
        float height = 2.0f,
        int stemSegments = 4,
        int leavesPerSegment = 3
    );
    
    /**
     * Generate bush structure (branched stems with leaves)
     * Multiple branches emerging from central stem
     */
    static std::unique_ptr<AtomMesh> generateBush(
        uint32_t meshId,
        const glm::vec3& basePosition,
        float height = 4.0f,
        int primaryBranches = 3,
        int secondaryBranches = 2
    );
    
    /**
     * Generate tree structure (complex multi-level branches + leaves)
     * Recursive fractal-like branching pattern
     */
    static std::unique_ptr<AtomMesh> generateTree(
        uint32_t meshId,
        const glm::vec3& basePosition,
        float height = 8.0f,
        int branchingLevels = 4
    );
    
    /**
     * Generate flat terrain patch (soil atoms)
     * Grid of soil atoms connected as triangles
     */
    static std::unique_ptr<AtomMesh> generateTerrainPatch(
        uint32_t meshId,
        const glm::vec3& basePosition,
        float width = 10.0f,
        float depth = 10.0f,
        int gridResolution = 5
    );
};

// ============================================================================
// ATOM OPERATIONS - Break, separate, degrade
// ============================================================================

class AtomOperations {
public:
    /**
     * Break a connection between two atoms (simulated damage)
     * Reduces structural integrity and may cascade failures
     */
    static bool breakConnection(AtomMesh& mesh, uint32_t atomId1, uint32_t atomId2);
    
    /**
     * Separate an atom from its structure (pruning, breaking)
     * Returns list of atoms that detach as a consequence
     */
    static std::vector<uint32_t> separateAtom(
        AtomMesh& mesh,
        uint32_t atomId,
        bool cascadeSeparation = true
    );
    
    /**
     * Degrade atoms to soil (natural decomposition or finishing blow)
     * Marks atoms for conversion to SOIL type
     */
    static void degradeToSoil(AtomMesh& mesh, const std::vector<uint32_t>& atomIds);
    
    /**
     * Calculate structural integrity after damage
     * Returns percentage of intact atoms with full connectivity
     */
    static float calculateIntegrity(const AtomMesh& mesh);
    
    /**
     * Remove all detached components from mesh
     * Returns count of atoms removed
     */
    static uint32_t removeDetachedComponents(AtomMesh& mesh, uint32_t rootAtomId = 0);
};

// ============================================================================
// PARTICLE ATOM SYSTEM (Main manager)
// ============================================================================

class ParticleAtomSystem {
public:
    ParticleAtomSystem();
    ~ParticleAtomSystem();
    
    // Lifecycle
    void update(float deltaTime);
    void render();
    void clear();
    
    // Mesh management
    uint32_t createMesh(const std::string& name, PlantType type);
    std::unique_ptr<AtomMesh> createPlantMesh(
        const glm::vec3& position,
        PlantType plantType,
        float scale = 1.0f
    );
    bool removeMesh(uint32_t meshId);
    AtomMesh* getMesh(uint32_t meshId);
    const std::vector<std::unique_ptr<AtomMesh>>& getAllMeshes() const;
    
    // Atom operations through system
    bool damageAtom(uint32_t meshId, uint32_t atomId, float damageAmount);
    bool pruneAtom(uint32_t meshId, uint32_t atomId);
    void degradeMesh(uint32_t meshId);
    std::vector<uint32_t> getSoilAtomsGenerated() const;
    
    // Queries
    uint32_t getTotalAtomCount() const;
    uint32_t getTotalTriangleCount() const;
    uint32_t getTotalIntactAtomCount() const;
    float getAverageIntegrity() const;
    
    // Physics
    void applyForce(uint32_t meshId, uint32_t atomId, const glm::vec3& force);
    void simulatePhysics(float deltaTime);
    
private:
    std::vector<std::unique_ptr<AtomMesh>> meshes;
    std::unordered_map<uint32_t, size_t> meshIdToIndex;
    
    uint32_t nextMeshId;
    uint32_t nextAtomId;
    
    // Degradation tracking
    std::vector<uint32_t> soilAtomsThisFrame;
    
    // Physics parameters
    glm::vec3 gravityDirection;
    float damping;
    
    void updateDegradation(float deltaTime);
    void cleanupScheduledAtoms();
    void recalculateMeshProperties(AtomMesh& mesh);
};

} // namespace ParticleAtom
