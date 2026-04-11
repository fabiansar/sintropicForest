#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <random>
#include "Config.h"

/**
 * ============================================================================
 * PLANT GENOME - Procedural Plant Generation System (MODULABLE)
 * ============================================================================
 * 
 * Sistema de generación procedural completamente parametrizable
 * Características:
 * - Genoma procedural (semilla + parámetros)
 * - Crecimiento realista con fractales
 * - Podas y defoliación
 * - Escalabilidad (pequeño a grande)
 * - Aleatorización controlada
 * - Modularidad total (cada parte es independiente)
 */

namespace PlantProcedural {

// ============================================================================
// PLANT GENOME - Define la estructura genética de una planta
// ============================================================================

struct PlantGenome {
    // Parámetro principal: tipo de planta
    enum GrowthType {
        GRASS_SIMPLE,      // Lineal simple
        GRASS_TUFTED,      // Agrupado denso
        HERB_BRANCHED,     // Con ramificación
        BUSH_SPARSE,       // Arbusto disperso
        BUSH_DENSE,        // Arbusto denso
        TREE_PINE,         // Árbol tipo pino (cónico)
        TREE_OAK,          // Árbol tipo roble (redondeado)
        TREE_WILLOW,       // Árbol tipo sauce (llorón)
        CUSTOM             // Personalizado
    };
    
    // Estructura genética
    GrowthType type = GRASS_SIMPLE;
    uint32_t seed = 12345;  // Para reproducibilidad
    
    // Parámetros escalables
    float baseHeight = 1.0f;           // Altura base (metros)
    float baseRadius = 0.5f;           // Radio base (metros)
    float heightVariance = 0.2f;       // Variación en altura
    float scaleFactor = 1.0f;          // Factor de escala global
    
    // Ramificación fractal
    int maxBranchingLevels = 3;        // Niveles de recursión
    float branchAngle = 45.0f;         // Ángulo de ramificación (grados)
    float branchLengthRatio = 0.7f;    // Cada rama = 0.7 * rama padre
    float branchRadiusRatio = 0.6f;    // Radio de rama = 0.6 * radio padre
    int branchDensity = 3;             // Ramas por nodo (1-8)
    
    // Follaje
    float leafDensity = 1.0f;          // 0.0-1.0: qué tan lleno de hojas
    float leafSize = 0.5f;             // Tamaño relativo de hojas
    int leafClusters = 5;              // Agrupamientos de hojas
    
    // Tronco/Tallo
    float stemTaperRatio = 0.8f;       // Qué tan afina el tallo (0-1)
    float stemFlexibility = 0.0f;      // 0=rígido, 1=muy flexible (para sauce)
    int stemSegments = 8;              // Segmentos en el tronco
    
    // Características especiales
    bool hasSpines = false;            // Tiene espinas (cactus)
    bool isDeciduos = false;           // Pierde hojas en invierno
    float pruneLevel = 0.0f;           // 0.0-1.0: cuánta poda aplicada
    
    // Colores (pueden ser aleatorios dentro de rango)
    glm::vec3 stemColor = glm::vec3(0.4f, 0.3f, 0.1f);
    glm::vec3 leafColor = glm::vec3(0.2f, 0.6f, 0.1f);
    
    // Daño y degradación
    float healthFactor = 1.0f;         // 0.0-1.0: qué tan sano
    float brokenBranchRatio = 0.0f;    // 0.0-1.0: ramas rotas
};

// ============================================================================
// PROCEDURAL PLANT BUILDER
// ============================================================================

class ProceduralPlantBuilder {
private:
    PlantGenome genome;
    std::mt19937 rng;
    
public:
    ProceduralPlantBuilder(const PlantGenome& g) : genome(g), rng(g.seed) {
    }
    
    // Obtener parámetros calculados a partir del genoma
    float getEffectiveHeight() const {
        std::uniform_real_distribution<float> dist(-genome.heightVariance, genome.heightVariance);
        float randomHeight = dist(rng);
        return genome.baseHeight * genome.scaleFactor * (1.0f + randomHeight);
    }
    
    float getEffectiveRadius() const {
        return genome.baseRadius * genome.scaleFactor;
    }
    
    int getEffectiveBranchCount(int level) const {
        // Menos ramas en niveles más altos
        int count = (int)(genome.branchDensity * (1.0f - (float)level / genome.maxBranchingLevels));
        return std::max(1, count);
    }
    
    glm::vec3 getRandomBranchDirection(int seed) const {
        std::mt19937 branchRng(seed);
        std::uniform_real_distribution<float> angleDist(0.0f, 360.0f);
        std::uniform_real_distribution<float> elevationDist(0.0f, genome.branchAngle);
        
        float azimuth = glm::radians(angleDist(branchRng));
        float elevation = glm::radians(elevationDist(branchRng));
        
        return glm::vec3(
            sin(elevation) * cos(azimuth),
            cos(elevation),
            sin(elevation) * sin(azimuth)
        );
    }
    
    // Aplicar poda (reduce follaje)
    void applyPrune(float pruneAmount) {
        genome.pruneLevel = glm::clamp(pruneAmount, 0.0f, 1.0f);
        // Reduce leafDensity proporcionalmente
        genome.leafDensity *= (1.0f - pruneAmount * 0.5f);
    }
    
    // Aplicar daño (rompe ramas, reduce salud)
    void applyDamage(float damageAmount) {
        genome.healthFactor *= (1.0f - damageAmount);
        genome.brokenBranchRatio = glm::clamp(damageAmount, 0.0f, 0.8f);
    }
    
    // Escalar la planta
    void scale(float factor) {
        genome.scaleFactor *= factor;
    }
    
    // Obtener genoma actual
    const PlantGenome& getGenome() const {
        return genome;
    }
    
    // Genomas preestablecidos
    static PlantGenome createGrassGenome() {
        PlantGenome g;
        g.type = PlantGenome::GRASS_SIMPLE;
        g.baseHeight = 0.3f;
        g.baseRadius = 0.1f;
        g.maxBranchingLevels = 1;
        g.branchDensity = 4;
        g.leafDensity = 0.8f;
        g.stemSegments = 4;
        return g;
    }
    
    static PlantGenome createBushGenome() {
        PlantGenome g;
        g.type = PlantGenome::BUSH_DENSE;
        g.baseHeight = 1.5f;
        g.baseRadius = 1.0f;
        g.maxBranchingLevels = 2;
        g.branchDensity = 5;
        g.branchAngle = 60.0f;
        g.leafDensity = 0.9f;
        g.stemTaperRatio = 0.7f;
        g.stemSegments = 6;
        return g;
    }
    
    static PlantGenome createTreeGenome() {
        PlantGenome g;
        g.type = PlantGenome::TREE_OAK;
        g.baseHeight = 6.0f;
        g.baseRadius = 2.0f;
        g.maxBranchingLevels = 4;
        g.branchDensity = 4;
        g.branchAngle = 45.0f;
        g.branchLengthRatio = 0.6f;
        g.branchRadiusRatio = 0.5f;
        g.leafDensity = 0.95f;
        g.stemTaperRatio = 0.8f;
        g.leafClusters = 8;
        g.stemSegments = 10;
        return g;
    }
    
    static PlantGenome createWillowGenome() {
        PlantGenome g;
        g.type = PlantGenome::TREE_WILLOW;
        g.baseHeight = 5.0f;
        g.baseRadius = 1.5f;
        g.maxBranchingLevels = 3;
        g.branchDensity = 6;
        g.branchAngle = 70.0f;  // Más colgante
        g.branchLengthRatio = 0.75f;
        g.stemFlexibility = 0.8f;
        g.leafDensity = 0.85f;
        g.leafColor = glm::vec3(0.3f, 0.7f, 0.2f);  // Verde más brillante
        return g;
    }
};

// ============================================================================
// PLANT SPECIMEN - Instancia de una planta con estado
// ============================================================================

struct PlantSpecimen {
    PlantGenome genome;
    float age = 0.0f;                 // Edad en "meses" de crecimiento
    float maxAge = 100.0f;            // Edad máxima (senescencia)
    float growthProgress = 0.0f;       // 0.0-1.0: juventud a madurez
    
    // Podas aplicadas
    std::vector<glm::vec3> lastPruneCuts;  // Ubicaciones donde se podó
    
    // Daño acumulado
    float accumulatedDamage = 0.0f;
    std::vector<uint32_t> damagedBranchIds;  // IDs de ramas dañadas
};

} // namespace PlantProcedural

#include <iostream>
