#include "EcosystemSimulator.h"
#include <algorithm>
#include <cmath>
#include <random>

// ============================================================================
// INICIALIZACIÓN
// ============================================================================

EcosystemSimulator::EcosystemSimulator() {
    initializeSpeciesData();
}

void EcosystemSimulator::initializeSpeciesData() {
    // HIERBA (Gramíneas) - Pioneras
    speciesData[GRASS] = SpeciesData{
        .type = GRASS,
        .minGrowthTime = 3.0f,
        .maxGrowthTime = 8.0f,
        .lifespan = 30.0f,
        .maxHeight = 0.5f,
        .maxRadius = 0.8f,
        .leafAreaIndex = 2.0f,
        .seedMaturityAge = 5.0f,
        .seedProductionRate = 0.3f,
        .seedDispersal = 5.0f,
        .minLight = 0.3f,
        .optimalLight = 0.9f,
        .minWater = 0.4f,
        .optimalWater = 0.8f,
        .shadeCast = 0.2f,
        .waterConsumption = 0.05f,
    };
    
    // ARBUSTO (Transición) - Establecidas
    speciesData[BUSH] = SpeciesData{
        .type = BUSH,
        .minGrowthTime = 8.0f,
        .maxGrowthTime = 20.0f,
        .lifespan = 80.0f,
        .maxHeight = 2.5f,
        .maxRadius = 1.8f,
        .leafAreaIndex = 4.0f,
        .seedMaturityAge = 15.0f,
        .seedProductionRate = 0.15f,
        .seedDispersal = 10.0f,
        .minLight = 0.4f,
        .optimalLight = 0.7f,
        .minWater = 0.3f,
        .optimalWater = 0.6f,
        .shadeCast = 0.4f,
        .waterConsumption = 0.08f,
        .nitrogenFixation = 0.05f,  // Si es leguminosa
    };
    
    // ÁRBOL (Clímax) - Dominantes
    speciesData[TREE] = SpeciesData{
        .type = TREE,
        .minGrowthTime = 15.0f,
        .maxGrowthTime = 40.0f,
        .lifespan = 180.0f,
        .maxHeight = 6.0f,
        .maxRadius = 4.0f,
        .leafAreaIndex = 5.0f,
        .seedMaturityAge = 30.0f,
        .seedProductionRate = 0.08f,
        .seedDispersal = 15.0f,
        .minLight = 0.2f,
        .optimalLight = 0.6f,
        .minWater = 0.3f,
        .optimalWater = 0.7f,
        .shadeCast = 0.8f,
        .waterConsumption = 0.15f,
    };
}

void EcosystemSimulator::initialize() {
    plants.clear();
}

// ============================================================================
// ACTUALIZACIÓN PRINCIPAL
// ============================================================================

void EcosystemSimulator::update(float deltaTime) {
    accumulatedTime += deltaTime * simulationSpeed;
    
    if (accumulatedTime < updateInterval) {
        return;  // Actualizar cada updateInterval segundos
    }
    
    float dt = accumulatedTime;
    accumulatedTime = 0.0f;
    
    // ✅ FASE 4: Reconstruir spatial grid para optimizaciones
    rebuildSpatialGrid();
    
    // 1. Actualizar vida de cada planta
    for (auto& plant : plants) {
        updatePlantLife(plant, dt);
        updateEnvironmentalStress(plant);
        checkSuccession(plant);
        disperseSeeds(plant, dt);
    }
    
    // 2. Resolver competencia
    resolveCompetition();
    
    // 3. Aplicar interacciones planta-planta
    updateInteractions();
    
    // 4. Eliminar plantas muertas
    plants.erase(
        std::remove_if(plants.begin(), plants.end(),
            [](const PlantData& p) { return p.stage == DEAD; }),
        plants.end()
    );
}

// ============================================================================
// CICLO DE VIDA DE PLANTAS
// ============================================================================

void EcosystemSimulator::updatePlantLife(PlantData& plant, float deltaTime) {
    plant.age += deltaTime;
    
    const SpeciesData& species = speciesData[plant.type];
    float ageRatio = plant.age / species.lifespan;
    
    // Determinar etapa de vida
    if (ageRatio < 0.2f) {
        plant.stage = SEEDLING;
    } else if (ageRatio < 0.5f) {
        plant.stage = JUVENILE;
    } else if (ageRatio < 0.85f) {
        plant.stage = ADULT;
    } else if (ageRatio < 1.0f) {
        plant.stage = SENESCENT;
    } else {
        plant.stage = DEAD;
        plant.healthFactor = 0.0f;
        return;
    }
    
    // CRECIMIENTO VISUAL (animar de 0 a maxHeight)
    // Curva sigmoide para crecimiento natural
    float growthAge = glm::clamp(plant.age / species.maxGrowthTime, 0.0f, 1.0f);
    float growthCurve = growthAge * growthAge * (3.0f - 2.0f * growthAge); // Ease-in-out
    
    plant.currentHeight = species.maxHeight * growthCurve * plant.healthFactor;
    plant.currentRadius = species.maxRadius * growthCurve * plant.healthFactor;
    
    // PRODUCCIÓN DE SEMILLAS (si es adulta y sana)
    if (plant.stage == ADULT && plant.healthFactor > 0.5f) {
        plant.seedBankAccumulated += species.seedProductionRate * deltaTime;
    }
    
    // DEGRADACIÓN DE SALUD POR EDAD (senescencia)
    if (plant.stage == SENESCENT) {
        plant.healthFactor -= deltaTime / species.lifespan;
    }
    
    // MORTALIDAD POR ESTRÉS (si stress > 1.0, más probabilidad de muerte)
    if (plant.localStress > 1.0f) {
        float deathChance = (plant.localStress - 1.0f) * deltaTime * 0.1f;
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        if (dist(gen) < deathChance) {
            plant.stage = DEAD;
            plant.healthFactor = 0.0f;
        }
    }
}

// ============================================================================
// ESTRÉS AMBIENTAL
// ============================================================================

void EcosystemSimulator::updateEnvironmentalStress(PlantData& plant) {
    const SpeciesData& species = speciesData[plant.type];
    
    // Calcular luz disponible (simplificado: luz global - sombra de vecinas)
    plant.localLight = globalLight;
    for (const auto& other : plants) {
        if (other.type == plant.type) continue;
        
        float dist = glm::distance(glm::vec2(plant.position.x, plant.position.z),
                                   glm::vec2(other.position.x, other.position.z));
        if (dist < species.maxRadius + 1.0f) {
            // Las plantas más altas proyectan sombra
            if (other.currentHeight > plant.currentHeight) {
                float shadowIntensity = (other.currentHeight - plant.currentHeight) * 0.2f;
                plant.localLight -= other.currentRadius / (dist + 0.1f) * shadowIntensity;
            }
        }
    }
    plant.localLight = glm::clamp(plant.localLight, 0.0f, 1.0f);
    
    // Agua y nutrientes basados en posición (simplificado)
    plant.localWater = globalWater;
    plant.localNutrients = globalNutrients;
    
    // Calcular factor de stress (desviación de óptimo)
    float lightStress = 0.0f;
    if (plant.localLight < species.minLight) {
        lightStress = (species.minLight - plant.localLight) * 2.0f;
    } else if (plant.localLight < species.optimalLight) {
        lightStress = (species.optimalLight - plant.localLight) * 0.3f;
    }
    
    float waterStress = 0.0f;
    if (plant.localWater < species.minWater) {
        waterStress = (species.minWater - plant.localWater) * 2.0f;
    } else if (plant.localWater < species.optimalWater) {
        waterStress = (species.optimalWater - plant.localWater) * 0.3f;
    }
    
    float nutrientStress = 0.0f;
    if (plant.localNutrients < species.minNutrients) {
        nutrientStress = (species.minNutrients - plant.localNutrients) * 1.5f;
    }
    
    // Stress total
    plant.localStress = lightStress + waterStress + nutrientStress;
    
    // El stress reduce salud y crecimiento
    if (plant.localStress > 0.0f) {
        plant.healthFactor -= plant.localStress * 0.01f;
        plant.healthFactor = glm::clamp(plant.healthFactor, 0.1f, 1.0f);
    } else {
        // Sin stress, recuperar salud lentamente
        plant.healthFactor = glm::min(plant.healthFactor + 0.005f, 1.0f);
    }
}

// ============================================================================
// SUCESIÓN ECOLÓGICA
// ============================================================================

void EcosystemSimulator::checkSuccession(PlantData& plant) {
    // Una hierba madura y sana en zona estable puede convertirse en arbusto
    if (plant.type == GRASS && plant.stage == ADULT && 
        plant.healthFactor > 0.8f && plant.age > 15.0f) {
        
        // Si hay sombra de árboles, convertir a arbusto
        bool nearTree = false;
        for (const auto& other : plants) {
            if (other.type == TREE) {
                float dist = glm::distance(glm::vec2(plant.position.x, plant.position.z),
                                          glm::vec2(other.position.x, other.position.z));
                if (dist < other.currentRadius * 2.0f && other.currentHeight > 3.0f) {
                    nearTree = true;
                    break;
                }
            }
        }
        
        if (nearTree) {
            // 5% de probabilidad por segundo de transformarse
            static std::mt19937 gen(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(gen) < 0.05f * 0.1f) {  // 0.1 deltaTime
                plant.type = BUSH;
                plant.age = 0.0f;
            }
        }
    }
    
    // Un arbusto viejo puede convertirse en árbol (si hay espacio)
    if (plant.type == BUSH && plant.stage == ADULT && 
        plant.healthFactor > 0.7f && plant.age > 30.0f) {
        
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        if (dist(gen) < 0.02f * 0.1f) {  // 0.1 deltaTime
            plant.type = TREE;
            plant.age = 0.0f;
        }
    }
}

// ============================================================================
// DISPERSIÓN DE SEMILLAS
// ============================================================================

void EcosystemSimulator::disperseSeeds(PlantData& plant, float deltaTime) {
    if (plant.seedBankAccumulated < 1.0f || plants.size() >= (size_t)maxPlants) {
        return;
    }
    
    static std::mt19937 gen(std::random_device{}());
    
    // Cada semilla tiene oportunidad de germinar
    int seedsToDisperse = (int)plant.seedBankAccumulated;
    plant.seedBankAccumulated -= seedsToDisperse;
    
    const SpeciesData& species = speciesData[plant.type];
    
    for (int i = 0; i < seedsToDisperse; ++i) {
        // Dispersión aleatoria dentro del radio
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
        std::uniform_real_distribution<float> radiusDist(0.0f, species.seedDispersal);
        
        float angle = angleDist(gen);
        float r = radiusDist(gen);
        
        glm::vec3 seedPos = plant.position + glm::vec3(
            cos(angle) * r,
            0.0f,
            sin(angle) * r
        );
        
        // Clampar dentro del terreno
        seedPos.x = glm::clamp(seedPos.x, 0.0f, terrainSize.x);
        seedPos.z = glm::clamp(seedPos.z, 0.0f, terrainSize.y);
        
        // 40% de probabilidad de que germine
        std::uniform_real_distribution<float> germDist(0.0f, 1.0f);
        if (germDist(gen) < 0.4f) {
            addPlant(seedPos, plant.type);
        }
    }
}

// ============================================================================
// COMPETENCIA
// ============================================================================

void EcosystemSimulator::resolveCompetition() {
    // ✅ FASE 4 OPTIMIZADO: Usar spatial grid en lugar de O(n²)
    // Antes: 500 plantas × 500 = 250,000 distancias/frame
    // Después: 500 plantas × ~9 vecinas = 4,500 distancias/frame (~55x speedup)
    
    for (size_t i = 0; i < plants.size(); ++i) {
        auto& plant = plants[i];
        plant.competitorsNearby = 0;
        const SpeciesData& species = speciesData[plant.type];
        
        // Obtener solo plantas vecinas usando spatial grid
        auto nearbyIndices = getNearbyCells(i, species.maxRadius + 1.0f);
        
        for (size_t otherIdx : nearbyIndices) {
            if (otherIdx == i) continue;  // No contar a sí misma
            const auto& other = plants[otherIdx];
            
            if (other.type == plant.type) continue;  // Solo competidores de tipo diferente
            
            float dist = glm::distance(glm::vec2(plant.position.x, plant.position.z),
                                      glm::vec2(other.position.x, other.position.z));
            
            // Si está dentro del radio de competencia
            if (dist < species.maxRadius + 1.0f) {
                plant.competitorsNearby++;
            }
        }
    }
}

// ============================================================================
// INTERACCIONES
// ============================================================================

void EcosystemSimulator::updateInteractions() {
    // ✅ FASE 4 OPTIMIZADO: Usar spatial grid para buscar simbionts
    
    for (size_t i = 0; i < plants.size(); ++i) {
        auto& plant = plants[i];
        
        // Simbiosis (árboles con leguminosas cercanas)
        if (plant.type == TREE) {
            // Buscar solo en celdas cercanas
            auto nearbyIndices = getNearbyCells(i, 5.0f);
            
            for (size_t otherIdx : nearbyIndices) {
                if (otherIdx == i) continue;
                auto& other = plants[otherIdx];
                
                if (other.type == BUSH) {
                    float dist = glm::distance(glm::vec2(plant.position.x, plant.position.z),
                                              glm::vec2(other.position.x, other.position.z));
                    if (dist < 5.0f) {
                        plant.hasSymbionts = true;
                        // El árbol se beneficia de nitrógeno
                        plant.healthFactor = glm::min(plant.healthFactor + 0.01f, 1.0f);
                    }
                }
            }
        }
    }
}

// ============================================================================
// INTERACCIÓN CON SISTEMA
// ============================================================================

bool EcosystemSimulator::addPlant(const glm::vec3& position, PlantType type) {
    if (plants.size() >= (size_t)maxPlants) {
        return false;
    }
    
    PlantData newPlant;
    newPlant.position = position;
    newPlant.type = type;
    newPlant.createdTime = 0.0f;
    newPlant.age = 0.0f;
    newPlant.stage = SEEDLING;
    newPlant.currentHeight = 0.1f;
    newPlant.currentRadius = 0.1f;
    newPlant.healthFactor = 1.0f;
    
    plants.push_back(newPlant);
    return true;
}

bool EcosystemSimulator::removePlant(size_t index) {
    if (index >= plants.size()) {
        return false;
    }
    
    plants.erase(plants.begin() + index);
    return true;
}

const SpeciesData& EcosystemSimulator::getSpeciesData(PlantType type) const {
    auto it = speciesData.find(type);
    if (it != speciesData.end()) {
        return it->second;
    }
    return speciesData.at(GRASS);  // Default
}

// ============================================================================
// OPTIMIZACIÓN FASE 4: SPATIAL GRID
// ============================================================================

void EcosystemSimulator::rebuildSpatialGrid() {
    /**
     * Reconstruir spatial grid particionando el terreno en celdas
     * 
     * Cada celda es una cuadrícula de GRID_CELL_SIZE × GRID_CELL_SIZE metros
     * Las plantas se distribuyen en sus celdas correspondientes
     * 
     * Beneficio: búsquedas de vecinos en O(1) en lugar de O(n)
     */
    spatialGrid.clear();
    
    for (size_t i = 0; i < plants.size(); ++i) {
        const auto& plant = plants[i];
        
        // Calcular coordenadas de celda
        int cellX = (int)(plant.position.x / GRID_CELL_SIZE);
        int cellZ = (int)(plant.position.z / GRID_CELL_SIZE);
        
        GridCell cell{cellX, cellZ};
        spatialGrid[cell].push_back(i);
    }
}

std::vector<size_t> EcosystemSimulator::getNearbyCells(size_t plantIndex, float radius) const {
    /**
     * Obtener índices de todas las plantas en celdas cercanas
     * 
     * Para una planta en posición (x, z) con radio r:
     * 1. Calcular su celda actual
     * 2. Iterar celdas vecinas en cuadrado de (r/GRID_CELL_SIZE)
     * 3. Retornar todos los índices en esas celdas
     * 
     * Complejidad: O(k²) donde k = ceil(radius / GRID_CELL_SIZE)
     * Para radius=5 y GRID_CELL_SIZE=10: k=1, máx 9 celdas
     */
    std::vector<size_t> nearby;
    
    if (plantIndex >= plants.size()) {
        return nearby;
    }
    
    const auto& plant = plants[plantIndex];
    int cellX = (int)(plant.position.x / GRID_CELL_SIZE);
    int cellZ = (int)(plant.position.z / GRID_CELL_SIZE);
    
    // Calcular rango de celdas a buscar
    int cellRange = (int)std::ceil(radius / GRID_CELL_SIZE);
    
    // Iterar celdas cercanas (cuadrado de búsqueda)
    for (int dx = -cellRange; dx <= cellRange; ++dx) {
        for (int dz = -cellRange; dz <= cellRange; ++dz) {
            GridCell neighborCell{cellX + dx, cellZ + dz};
            
            auto it = spatialGrid.find(neighborCell);
            if (it != spatialGrid.end()) {
                // Agregar todos los índices de esta celda
                for (size_t idx : it->second) {
                    nearby.push_back(idx);
                }
            }
        }
    }
    
    return nearby;
}

// ============================================================================
// ESTADÍSTICAS
// ============================================================================

glm::vec3 EcosystemSimulator::getAveragePosition() const {
    if (plants.empty()) return glm::vec3(0.0f);
    
    glm::vec3 sum(0.0f);
    for (const auto& plant : plants) {
        sum += plant.position;
    }
    return sum / (float)plants.size();
}

EcosystemSimulator::EcosystemStats EcosystemSimulator::getStatistics() const {
    EcosystemStats stats;
    
    float totalHealth = 0.0f;
    std::map<PlantType, int> counts;
    
    for (const auto& plant : plants) {
        counts[plant.type]++;
        totalHealth += plant.healthFactor;
    }
    
    stats.grazeCount = counts[GRASS];
    stats.bushCount = counts[BUSH];
    stats.treeCount = counts[TREE];
    stats.averageHealth = plants.empty() ? 0.0f : totalHealth / plants.size();
    
    // Biodiversity (Shannon index simplificado)
    float total = plants.size();
    if (total > 0.0f) {
        float h = 0.0f;
        for (const auto& [type, count] : counts) {
            float p = count / total;
            if (p > 0.0f) {
                h -= p * log(p);
            }
        }
        stats.biodiversity = h;
    }
    
    // Biomass (suma de alturas como proxy)
    for (const auto& plant : plants) {
        stats.biomass += plant.currentHeight;
    }
    
    return stats;
}
