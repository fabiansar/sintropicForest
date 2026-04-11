#include "ParticleAtomSystem.h"
#include <algorithm>
#include <queue>
#include <cmath>
#include <random>

namespace ParticleAtom {

// ============================================================================
// ATOM MESH - Implementation
// ============================================================================

uint32_t AtomMesh::getIntactAtomCount() const {
    uint32_t count = 0;
    for (const auto& atom : atoms) {
        if (atom.health > 0.0f && !atom.isScheduledForRemoval) {
            count++;
        }
    }
    return count;
}

std::vector<uint32_t> AtomMesh::getConnectedComponent(uint32_t startAtomId) const {
    std::vector<uint32_t> component;
    std::unordered_set<uint32_t> visited;
    std::queue<uint32_t> queue;
    
    queue.push(startAtomId);
    visited.insert(startAtomId);
    
    while (!queue.empty()) {
        uint32_t currentId = queue.front();
        queue.pop();
        component.push_back(currentId);
        
        // Find atom by ID
        for (const auto& atom : atoms) {
            if (atom.id == currentId) {
                for (uint32_t connectedId : atom.connectedAtomIds) {
                    if (visited.find(connectedId) == visited.end()) {
                        visited.insert(connectedId);
                        queue.push(connectedId);
                    }
                }
                break;
            }
        }
    }
    
    return component;
}

// ============================================================================
// ATOM GENERATOR - Procedural structure generation
// ============================================================================

std::unique_ptr<AtomMesh> AtomGenerator::generateHerbaceousPlant(
    uint32_t meshId,
    const glm::vec3& basePosition,
    float height,
    int stemSegments,
    int leavesPerSegment)
{
    auto mesh = std::make_unique<AtomMesh>(meshId, "Herbaceous_Plant", PlantType::GRASS);
    
    std::vector<uint32_t> stemAtomIds;
    uint32_t atomIdCounter = 0;
    
    // Generate stem
    for (int i = 0; i < stemSegments; ++i) {
        float t = static_cast<float>(i) / (stemSegments - 1);
        glm::vec3 stemPos = basePosition + glm::vec3(0.0f, t * height, 0.0f);
        
        Atom stemAtom;
        stemAtom.id = atomIdCounter++;
        stemAtom.type = AtomType::STEM;
        stemAtom.position = stemPos;
        stemAtom.color = glm::vec4(0.4f, 0.6f, 0.2f, 1.0f); // Green
        stemAtom.parentMeshId = meshId;
        
        stemAtomIds.push_back(stemAtom.id);
        mesh->atoms.push_back(stemAtom);
    }
    
    // Connect stem segments
    for (size_t i = 0; i < stemAtomIds.size() - 1; ++i) {
        uint32_t id1 = stemAtomIds[i];
        uint32_t id2 = stemAtomIds[i + 1];
        
        mesh->atoms[i].connectedAtomIds.push_back(id2);
        mesh->atoms[i + 1].connectedAtomIds.push_back(id1);
        
        // Create triangle faces (degenerate for now, will be proper in rendering)
        mesh->triangles.emplace_back(id1, id2, id1);
    }
    
    // Generate leaves around stem
    std::mt19937 rng(meshId);
    const float PI = 3.141592653589793f;
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI);
    
    for (int i = 1; i < stemSegments; ++i) {
        for (int j = 0; j < leavesPerSegment; ++j) {
            Atom leafAtom;
            leafAtom.id = atomIdCounter++;
            leafAtom.type = AtomType::LEAF;
            
            float angle = angleDist(rng);
            float leafDist = 1.5f;
            leafAtom.position = mesh->atoms[i].position + 
                glm::vec3(cos(angle) * leafDist, 0.3f, sin(angle) * leafDist);
            
            leafAtom.color = glm::vec4(0.2f, 0.8f, 0.3f, 0.8f); // Bright green
            leafAtom.parentMeshId = meshId;
            
            // Connect to nearest stem atom
            leafAtom.connectedAtomIds.push_back(stemAtomIds[i]);
            mesh->atoms[i].connectedAtomIds.push_back(leafAtom.id);
            
            // Create triangle
            mesh->triangles.emplace_back(leafAtom.id, stemAtomIds[i], 
                                        i < stemAtomIds.size() - 1 ? stemAtomIds[i + 1] : stemAtomIds[i]);
            
            mesh->atoms.push_back(leafAtom);
        }
    }
    
    return mesh;
}

std::unique_ptr<AtomMesh> AtomGenerator::generateBush(
    uint32_t meshId,
    const glm::vec3& basePosition,
    float height,
    int primaryBranches,
    int secondaryBranches)
{
    auto mesh = std::make_unique<AtomMesh>(meshId, "Bush", PlantType::BUSH);
    
    uint32_t atomIdCounter = 0;
    std::mt19937 rng(meshId);
    const float PI = 3.141592653589793f;
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI);
    
    // Main stem
    std::vector<uint32_t> mainStemIds;
    int mainStemSegments = 5;
    for (int i = 0; i < mainStemSegments; ++i) {
        float t = static_cast<float>(i) / (mainStemSegments - 1);
        glm::vec3 stemPos = basePosition + glm::vec3(0.0f, t * height, 0.0f);
        
        Atom stemAtom;
        stemAtom.id = atomIdCounter++;
        stemAtom.type = AtomType::STEM;
        stemAtom.position = stemPos;
        stemAtom.color = glm::vec4(0.5f, 0.4f, 0.2f, 1.0f); // Brown
        stemAtom.parentMeshId = meshId;
        
        mainStemIds.push_back(stemAtom.id);
        mesh->atoms.push_back(stemAtom);
    }
    
    // Connect main stem
    for (size_t i = 0; i < mainStemIds.size() - 1; ++i) {
        mesh->atoms[i].connectedAtomIds.push_back(mainStemIds[i + 1]);
        mesh->atoms[i + 1].connectedAtomIds.push_back(mainStemIds[i]);
    }
    
    // Generate primary branches
    for (int b = 0; b < primaryBranches; ++b) {
        int attachPoint = 1 + (mainStemSegments - 2) * b / primaryBranches;
        if (attachPoint >= mainStemSegments) attachPoint = mainStemSegments - 1;
        
        float branchAngle = angleDist(rng);
        float branchLength = height * 0.6f;
        
        std::vector<uint32_t> branchIds;
        int branchSegments = 3;
        
        for (int i = 0; i < branchSegments; ++i) {
            float t = static_cast<float>(i) / (branchSegments - 1);
            glm::vec3 branchPos = mesh->atoms[attachPoint].position + 
                glm::vec3(cos(branchAngle) * branchLength * t, 
                         t * height * 0.3f, 
                         sin(branchAngle) * branchLength * t);
            
            Atom branchAtom;
            branchAtom.id = atomIdCounter++;
            branchAtom.type = (i == 0) ? AtomType::BRANCH : AtomType::STEM;
            branchAtom.position = branchPos;
            branchAtom.color = glm::vec4(0.6f, 0.5f, 0.3f, 1.0f);
            branchAtom.parentMeshId = meshId;
            
            branchIds.push_back(branchAtom.id);
            mesh->atoms.push_back(branchAtom);
        }
        
        // Connect branch to stem
        mesh->atoms[attachPoint].connectedAtomIds.push_back(branchIds[0]);
        mesh->atoms.back().connectedAtomIds.push_back(mainStemIds[attachPoint]);
        
        // Connect branch segments
        for (size_t i = 0; i < branchIds.size() - 1; ++i) {
            mesh->atoms[branchIds[i]].connectedAtomIds.push_back(branchIds[i + 1]);
            mesh->atoms[branchIds[i + 1]].connectedAtomIds.push_back(branchIds[i]);
        }
        
        // Add leaves to branch
        for (uint32_t branchAtomId : branchIds) {
            for (int l = 0; l < 2; ++l) {
                Atom leafAtom;
                leafAtom.id = atomIdCounter++;
                leafAtom.type = AtomType::LEAF;
                
                float leafAngle = angleDist(rng);
                leafAtom.position = mesh->atoms[branchAtomId].position + 
                    glm::vec3(cos(leafAngle) * 1.2f, 0.2f, sin(leafAngle) * 1.2f);
                
                leafAtom.color = glm::vec4(0.3f, 0.7f, 0.2f, 0.9f);
                leafAtom.parentMeshId = meshId;
                
                leafAtom.connectedAtomIds.push_back(branchAtomId);
                mesh->atoms[branchAtomId].connectedAtomIds.push_back(leafAtom.id);
                
                mesh->atoms.push_back(leafAtom);
            }
        }
    }
    
    return mesh;
}

std::unique_ptr<AtomMesh> AtomGenerator::generateTree(
    uint32_t meshId,
    const glm::vec3& basePosition,
    float height,
    int branchingLevels)
{
    auto mesh = std::make_unique<AtomMesh>(meshId, "Tree", PlantType::TREE);
    
    // Simplified tree generation
    // Full implementation would use recursive fractal branching
    
    uint32_t atomIdCounter = 0;
    std::mt19937 rng(meshId);
    const float PI = 3.141592653589793f;
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * PI);
    
    // Trunk
    for (int i = 0; i < 8; ++i) {
        float t = static_cast<float>(i) / 8.0f;
        glm::vec3 trunkPos = basePosition + glm::vec3(0.0f, t * height * 0.6f, 0.0f);
        
        Atom trunkAtom;
        trunkAtom.id = atomIdCounter++;
        trunkAtom.type = AtomType::STEM;
        trunkAtom.position = trunkPos;
        trunkAtom.color = glm::vec4(0.4f, 0.3f, 0.1f, 1.0f); // Dark brown
        trunkAtom.parentMeshId = meshId;
        
        mesh->atoms.push_back(trunkAtom);
        
        if (i > 0) {
            mesh->atoms[i].connectedAtomIds.push_back(mesh->atoms[i - 1].id);
            mesh->atoms[i - 1].connectedAtomIds.push_back(mesh->atoms[i].id);
        }
    }
    
    // Crown (simplified dense foliage)
    glm::vec3 crownCenter = basePosition + glm::vec3(0.0f, height * 0.7f, 0.0f);
    float crownRadius = height * 0.4f;
    
    for (int i = 0; i < 30; ++i) {
        Atom leafAtom;
        leafAtom.id = atomIdCounter++;
        leafAtom.type = AtomType::LEAF;
        
        float theta = angleDist(rng);
        float phi = angleDist(rng);
        leafAtom.position = crownCenter + glm::vec3(
            sin(phi) * cos(theta) * crownRadius,
            cos(phi) * crownRadius * 0.8f,
            sin(phi) * sin(theta) * crownRadius
        );
        
        leafAtom.color = glm::vec4(0.2f, 0.6f, 0.1f, 0.95f); // Dark green
        leafAtom.parentMeshId = meshId;
        
        // Connect to nearest trunk atom
        int nearestTrunk = 5 + i % 3;
        if (nearestTrunk < mesh->atoms.size()) {
            leafAtom.connectedAtomIds.push_back(mesh->atoms[nearestTrunk].id);
            mesh->atoms[nearestTrunk].connectedAtomIds.push_back(leafAtom.id);
        }
        
        mesh->atoms.push_back(leafAtom);
    }
    
    return mesh;
}

std::unique_ptr<AtomMesh> AtomGenerator::generateTerrainPatch(
    uint32_t meshId,
    const glm::vec3& basePosition,
    float width,
    float depth,
    int gridResolution)
{
    auto mesh = std::make_unique<AtomMesh>(meshId, "Terrain", PlantType::GRASS);
    mesh->atoms.reserve(gridResolution * gridResolution);
    mesh->triangles.reserve((gridResolution - 1) * (gridResolution - 1) * 2);
    
    uint32_t atomIdCounter = 0;
    
    // Create grid of soil atoms
    std::vector<std::vector<uint32_t>> grid(gridResolution, 
                                            std::vector<uint32_t>(gridResolution));
    
    for (int x = 0; x < gridResolution; ++x) {
        for (int z = 0; z < gridResolution; ++z) {
            Atom soilAtom;
            soilAtom.id = atomIdCounter++;
            soilAtom.type = AtomType::SOIL;
            soilAtom.position = basePosition + glm::vec3(
                (static_cast<float>(x) / (gridResolution - 1)) * width - width / 2.0f,
                0.0f,
                (static_cast<float>(z) / (gridResolution - 1)) * depth - depth / 2.0f
            );
            soilAtom.color = glm::vec4(0.55f, 0.40f, 0.25f, 1.0f); // Brown
            soilAtom.parentMeshId = meshId;
            
            grid[x][z] = soilAtom.id;
            mesh->atoms.push_back(soilAtom);
        }
    }
    
    // Connect atoms to neighbors and create triangles
    for (int x = 0; x < gridResolution - 1; ++x) {
        for (int z = 0; z < gridResolution - 1; ++z) {
            uint32_t a = grid[x][z];
            uint32_t b = grid[x + 1][z];
            uint32_t c = grid[x][z + 1];
            uint32_t d = grid[x + 1][z + 1];
            
            // Create two triangles per quad
            mesh->triangles.emplace_back(a, b, c);
            mesh->triangles.emplace_back(b, d, c);
            
            // Register connections
            mesh->atoms[a].connectedAtomIds.push_back(b);
            mesh->atoms[a].connectedAtomIds.push_back(c);
            mesh->atoms[b].connectedAtomIds.push_back(a);
            mesh->atoms[b].connectedAtomIds.push_back(d);
            mesh->atoms[c].connectedAtomIds.push_back(a);
            mesh->atoms[c].connectedAtomIds.push_back(d);
            mesh->atoms[d].connectedAtomIds.push_back(b);
            mesh->atoms[d].connectedAtomIds.push_back(c);
        }
    }
    
    return mesh;
}

// ============================================================================
// ATOM OPERATIONS - Breaking, separation, degradation
// ============================================================================

bool AtomOperations::breakConnection(AtomMesh& mesh, uint32_t atomId1, uint32_t atomId2) {
    // Find atoms
    Atom* atom1 = nullptr;
    Atom* atom2 = nullptr;
    
    for (auto& atom : mesh.atoms) {
        if (atom.id == atomId1) atom1 = &atom;
        if (atom.id == atomId2) atom2 = &atom;
    }
    
    if (!atom1 || !atom2) return false;
    
    // Remove connections
    auto& conn1 = atom1->connectedAtomIds;
    auto& conn2 = atom2->connectedAtomIds;
    
    conn1.erase(std::remove(conn1.begin(), conn1.end(), atomId2), conn1.end());
    conn2.erase(std::remove(conn2.begin(), conn2.end(), atomId1), conn2.end());
    
    return true;
}

std::vector<uint32_t> AtomOperations::separateAtom(
    AtomMesh& mesh,
    uint32_t atomId,
    bool cascadeSeparation)
{
    std::vector<uint32_t> separated;
    Atom* targetAtom = nullptr;
    
    for (auto& atom : mesh.atoms) {
        if (atom.id == atomId) {
            targetAtom = &atom;
            break;
        }
    }
    
    if (!targetAtom) return separated;
    
    // Disconnect from all neighbors
    for (uint32_t neighborId : targetAtom->connectedAtomIds) {
        breakConnection(mesh, atomId, neighborId);
    }
    
    targetAtom->connectedAtomIds.clear();
    separated.push_back(atomId);
    
    // Cascade: remove detached components
    if (cascadeSeparation) {
        uint32_t removed = removeDetachedComponents(mesh);
        if (removed > 0) {
            // Mark removed atoms for cleanup
            for (auto& atom : mesh.atoms) {
                if (atom.isScheduledForRemoval) {
                    separated.push_back(atom.id);
                }
            }
        }
    }
    
    return separated;
}

void AtomOperations::degradeToSoil(AtomMesh& mesh, const std::vector<uint32_t>& atomIds) {
    for (uint32_t atomId : atomIds) {
        for (auto& atom : mesh.atoms) {
            if (atom.id == atomId) {
                atom.type = AtomType::SOIL;
                atom.color = glm::vec4(0.55f, 0.40f, 0.25f, 1.0f); // Brown soil
                atom.health = 1.0f;
                atom.connectedAtomIds.clear(); // Disconnect from structure
                break;
            }
        }
    }
}

float AtomOperations::calculateIntegrity(const AtomMesh& mesh) {
    if (mesh.atoms.empty()) return 0.0f;
    
    float totalHealth = 0.0f;
    uint32_t intactCount = 0;
    
    for (const auto& atom : mesh.atoms) {
        if (!atom.isScheduledForRemoval) {
            totalHealth += atom.health;
            if (atom.health > 0.0f) intactCount++;
        }
    }
    
    return intactCount > 0 ? totalHealth / mesh.atoms.size() : 0.0f;
}

uint32_t AtomOperations::removeDetachedComponents(AtomMesh& mesh, uint32_t rootAtomId) {
    if (mesh.atoms.empty()) return 0;
    
    // Find connected component starting from root
    std::unordered_set<uint32_t> connected = 
        std::unordered_set<uint32_t>(
            mesh.getConnectedComponent(mesh.atoms[0].id).begin(),
            mesh.getConnectedComponent(mesh.atoms[0].id).end()
        );
    
    // Mark detached atoms for removal
    uint32_t detachedCount = 0;
    for (auto& atom : mesh.atoms) {
        if (connected.find(atom.id) == connected.end()) {
            atom.isScheduledForRemoval = true;
            detachedCount++;
        }
    }
    
    return detachedCount;
}

// ============================================================================
// PARTICLE ATOM SYSTEM - Main implementation
// ============================================================================

ParticleAtomSystem::ParticleAtomSystem()
    : nextMeshId(1), nextAtomId(1),
      gravityDirection(0.0f, -9.81f, 0.0f),
      damping(0.95f)
{
}

ParticleAtomSystem::~ParticleAtomSystem() {
    clear();
}

void ParticleAtomSystem::update(float deltaTime) {
    updateDegradation(deltaTime);
    simulatePhysics(deltaTime);
    cleanupScheduledAtoms();
    
    for (auto& mesh : meshes) {
        mesh->integrityScore = AtomOperations::calculateIntegrity(*mesh);
    }
}

void ParticleAtomSystem::render() {
    // Implementation deferred to GraphicsEngine
    // Renders all triangles from all meshes
}

void ParticleAtomSystem::clear() {
    meshes.clear();
    meshIdToIndex.clear();
    soilAtomsThisFrame.clear();
}

uint32_t ParticleAtomSystem::createMesh(const std::string& name, PlantType type) {
    uint32_t id = nextMeshId++;
    auto mesh = std::make_unique<AtomMesh>(id, name, type);
    meshIdToIndex[id] = meshes.size();
    meshes.push_back(std::move(mesh));
    return id;
}

std::unique_ptr<AtomMesh> ParticleAtomSystem::createPlantMesh(
    const glm::vec3& position,
    PlantType plantType,
    float scale)
{
    uint32_t id = nextMeshId++;
    std::unique_ptr<AtomMesh> mesh;
    
    switch (plantType) {
        case PlantType::GRASS:
            mesh = AtomGenerator::generateHerbaceousPlant(id, position, 2.0f * scale);
            break;
        case PlantType::BUSH:
            mesh = AtomGenerator::generateBush(id, position, 4.0f * scale);
            break;
        case PlantType::TREE:
            mesh = AtomGenerator::generateTree(id, position, 8.0f * scale);
            break;
    }
    
    return mesh;
}

bool ParticleAtomSystem::removeMesh(uint32_t meshId) {
    auto it = meshIdToIndex.find(meshId);
    if (it == meshIdToIndex.end()) return false;
    
    size_t index = it->second;
    meshes.erase(meshes.begin() + index);
    meshIdToIndex.erase(it);
    
    // Rebuild index map
    for (size_t i = 0; i < meshes.size(); ++i) {
        meshIdToIndex[meshes[i]->id] = i;
    }
    
    return true;
}

AtomMesh* ParticleAtomSystem::getMesh(uint32_t meshId) {
    auto it = meshIdToIndex.find(meshId);
    if (it == meshIdToIndex.end()) return nullptr;
    return meshes[it->second].get();
}

const std::vector<std::unique_ptr<AtomMesh>>& ParticleAtomSystem::getAllMeshes() const {
    return meshes;
}

bool ParticleAtomSystem::damageAtom(uint32_t meshId, uint32_t atomId, float damageAmount) {
    AtomMesh* mesh = getMesh(meshId);
    if (!mesh) return false;
    
    for (auto& atom : mesh->atoms) {
        if (atom.id == atomId) {
            atom.health = glm::max(0.0f, atom.health - damageAmount);
            
            if (atom.health <= 0.0f) {
                atom.isScheduledForRemoval = true;
            }
            
            return true;
        }
    }
    
    return false;
}

bool ParticleAtomSystem::pruneAtom(uint32_t meshId, uint32_t atomId) {
    AtomMesh* mesh = getMesh(meshId);
    if (!mesh) return false;
    
    auto separated = AtomOperations::separateAtom(*mesh, atomId, true);
    if (!separated.empty()) {
        recalculateMeshProperties(*mesh);
        return true;
    }
    
    return false;
}

void ParticleAtomSystem::degradeMesh(uint32_t meshId) {
    AtomMesh* mesh = getMesh(meshId);
    if (!mesh) return;
    
    std::vector<uint32_t> allAtomIds;
    for (const auto& atom : mesh->atoms) {
        allAtomIds.push_back(atom.id);
    }
    
    AtomOperations::degradeToSoil(*mesh, allAtomIds);
    soilAtomsThisFrame.insert(soilAtomsThisFrame.end(), allAtomIds.begin(), allAtomIds.end());
}

std::vector<uint32_t> ParticleAtomSystem::getSoilAtomsGenerated() const {
    return soilAtomsThisFrame;
}

uint32_t ParticleAtomSystem::getTotalAtomCount() const {
    uint32_t total = 0;
    for (const auto& mesh : meshes) {
        total += mesh->getAtomCount();
    }
    return total;
}

uint32_t ParticleAtomSystem::getTotalTriangleCount() const {
    uint32_t total = 0;
    for (const auto& mesh : meshes) {
        total += mesh->getTriangleCount();
    }
    return total;
}

uint32_t ParticleAtomSystem::getTotalIntactAtomCount() const {
    uint32_t total = 0;
    for (const auto& mesh : meshes) {
        total += mesh->getIntactAtomCount();
    }
    return total;
}

float ParticleAtomSystem::getAverageIntegrity() const {
    if (meshes.empty()) return 1.0f;
    
    float totalIntegrity = 0.0f;
    for (const auto& mesh : meshes) {
        totalIntegrity += mesh->integrityScore;
    }
    
    return totalIntegrity / meshes.size();
}

void ParticleAtomSystem::applyForce(uint32_t meshId, uint32_t atomId, const glm::vec3& force) {
    AtomMesh* mesh = getMesh(meshId);
    if (!mesh) return;
    
    for (auto& atom : mesh->atoms) {
        if (atom.id == atomId) {
            atom.velocity += force;
            break;
        }
    }
}

void ParticleAtomSystem::simulatePhysics(float deltaTime) {
    for (auto& mesh : meshes) {
        for (auto& atom : mesh->atoms) {
            // Apply gravity
            atom.velocity += gravityDirection * deltaTime;
            
            // Apply damping
            atom.velocity *= damping;
            
            // Update position
            atom.position += atom.velocity * deltaTime;
        }
    }
}

void ParticleAtomSystem::updateDegradation(float deltaTime) {
    soilAtomsThisFrame.clear();
    
    for (auto& mesh : meshes) {
        for (auto& atom : mesh->atoms) {
            if (atom.type != AtomType::SOIL && atom.health > 0.0f) {
                atom.degradationTimer += deltaTime;
                
                if (atom.degradationTimer >= mesh->degradationRate) {
                    atom.type = AtomType::SOIL;
                    atom.color = glm::vec4(0.55f, 0.40f, 0.25f, 1.0f);
                    atom.degradationTimer = 0.0f;
                    soilAtomsThisFrame.push_back(atom.id);
                }
            }
        }
    }
}

void ParticleAtomSystem::cleanupScheduledAtoms() {
    for (auto& mesh : meshes) {
        mesh->atoms.erase(
            std::remove_if(mesh->atoms.begin(), mesh->atoms.end(),
                          [](const Atom& a) { return a.isScheduledForRemoval; }),
            mesh->atoms.end()
        );
    }
}

void ParticleAtomSystem::recalculateMeshProperties(AtomMesh& mesh) {
    if (mesh.atoms.empty()) return;
    
    // Recalculate center of mass
    glm::vec3 com(0.0f);
    for (const auto& atom : mesh.atoms) {
        com += atom.position;
    }
    mesh.centerOfMass = com / static_cast<float>(mesh.atoms.size());
    
    // Recalculate integrity
    mesh.integrityScore = AtomOperations::calculateIntegrity(mesh);
}

} // namespace ParticleAtom
