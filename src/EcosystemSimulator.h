#pragma once

#include "Config.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <map>

/**
 * ============================================================================
 * ECOSYSTEM SIMULATOR - Core de Simulación Ecológica
 * ============================================================================
 * 
 * Gestiona la simulación completa de sucesión ecológica, crecimiento de plantas
 * y dinámicas agroforestales.
 * 
 * Responsabilidades:
 * - Ciclo de vida de plantas (nacimiento → crecimiento → madurez → muerte)
 * - Sucesión ecológica (hierba → arbusto → árbol)
 * - Interacciones planta-planta (competencia, simbiosis, alelopatía)
 * - Dispersión de semillas
 * - Sistema ambiental (luz, agua, nutrientes)
 * - Actualización de simulación por frame
 */

// ============================================================================
// DATOS DE ESPECIE (Define comportamiento de cada tipo)
// ============================================================================

struct SpeciesData {
    PlantType type;                    // GRASS, BUSH, TREE
    
    // Ciclo de vida (en segundos de juego)
    float minGrowthTime = 5.0f;        // Mínimo para alcanzar tamaño adulto
    float maxGrowthTime = 15.0f;       // Máximo (con estrés)
    float lifespan = 60.0f;            // Tiempo total de vida
    
    // Características de crecimiento
    float maxHeight = 2.0f;            // Altura máxima (escala visual)
    float maxRadius = 1.5f;            // Radio de copa/cobertura
    float leafAreaIndex = 3.0f;        // Área de hojas (competencia por luz)
    
    // Reproducción
    float seedMaturityAge = 15.0f;     // Edad mínima para producir semillas
    float seedProductionRate = 0.1f;   // Semillas/segundo cuando es reproductivo
    float seedDispersal = 8.0f;        // Radio de dispersión (metros)
    
    // Requerimientos ambientales
    float minLight = 0.2f;             // Luz mínima (0-1)
    float optimalLight = 0.8f;         // Luz óptima
    float minWater = 0.3f;             // Humedad mínima (0-1)
    float optimalWater = 0.7f;         // Humedad óptima
    float minNutrients = 0.2f;         // Nutrientes mínimos
    float optimalNutrients = 0.7f;     // Nutrientes óptimos
    
    // Efectos sobre el entorno
    float shadeCast = 0.5f;            // Cantidad de sombra que genera (0-1)
    float waterConsumption = 0.1f;     // Agua que consume por segundo
    float nutrientDepletion = 0.05f;   // Nutrientes que extrae del suelo
    float nitrogenFixation = 0.0f;     // Leguminosas: nitrato que agregan
};

// ============================================================================
// ESTADOS DE PLANTA
// ============================================================================

enum PlantLifeStage {
    SEEDLING,      // Germinación (0-20% de edad)
    JUVENILE,      // Crecimiento (20-50%)
    ADULT,         // Madurez (50-80%)
    SENESCENT,     // Envejecimiento (80-100%)
    DEAD           // Muerta
};

// ============================================================================
// PLANTA MEJORADA CON DATOS ECOLÓGICOS
// ============================================================================

struct PlantData {
    glm::vec3 position;           // Posición en el mundo
    PlantType type;               // Tipo (hierba, arbusto, árbol)
    
    // Ciclo de vida
    float age = 0.0f;             // Edad actual (segundos)
    float createdTime = 0.0f;     // Timestamp de creación
    PlantLifeStage stage = SEEDLING;
    
    // Crecimiento visual
    float currentHeight = 0.2f;   // Altura actual (se anima)
    float currentRadius = 0.1f;   // Radio actual
    float healthFactor = 1.0f;    // 0-1: salud de la planta
    
    // Reproducción
    float seedBankAccumulated = 0.0f;  // Semillas producidas
    
    // Estado ambiental local
    float localLight = 0.5f;      // Luz recibida
    float localWater = 0.5f;      // Humedad disponible
    float localNutrients = 0.5f;  // Nutrientes disponibles
    float localStress = 0.0f;     // Estrés acumulado (0-1)
    
    // Interacciones
    int competitorsNearby = 0;    // Plantas similares cerca
    bool hasSymbionts = false;    // ¿Tiene relaciones simbióticas?
    float allelopathyDamage = 0.0f; // Daño por alelopatía
};

// ============================================================================
// ECOSYSTEM SIMULATOR
// ============================================================================

class EcosystemSimulator {
private:
    std::vector<PlantData> plants;
    std::map<PlantType, SpeciesData> speciesData;
    
    // Condiciones ambientales globales
    glm::vec2 terrainSize = glm::vec2(100.0f, 100.0f);
    float globalLight = 0.8f;      // Luz global (afectada por nubosidad)
    float globalWater = 0.6f;      // Humedad global
    float globalNutrients = 0.5f;  // Nutrientes promedio del suelo
    
    // Parámetros de simulación
    float simulationSpeed = 1.0f;  // 1.0 = tiempo real, 2.0 = 2x más rápido
    int maxPlants = 500;
    float updateInterval = 0.1f;   // Actualizar cada 0.1 segundos
    float accumulatedTime = 0.0f;

    // Métodos privados
    void initializeSpeciesData();
    void updatePlantLife(PlantData& plant, float deltaTime);
    void updateEnvironmentalStress(PlantData& plant);
    void checkSuccession(PlantData& plant);
    void disperseSeeds(const PlantData& plant, float deltaTime);
    void resolveCompetition();
    void updateInteractions();
    glm::vec3 findRandomGrowthSpot(const PlantData& parent) const;

public:
    EcosystemSimulator();
    
    /**
     * Inicializar simulador con parámetros
     */
    void initialize();
    
    /**
     * Actualizar simulación cada frame
     */
    void update(float deltaTime);
    
    /**
     * Agregar planta en posición específica
     */
    bool addPlant(const glm::vec3& position, PlantType type);
    
    /**
     * Remover planta (muerte forzada o cosecha)
     */
    bool removePlant(size_t index);
    
    /**
     * Obtener datos de especie
     */
    const SpeciesData& getSpeciesData(PlantType type) const;
    
    // ===== GETTERS =====
    
    const std::vector<PlantData>& getPlants() const {
        return plants;
    }
    
    size_t getPlantCount() const {
        return plants.size();
    }
    
    glm::vec3 getAveragePosition() const;
    
    /**
     * Estadísticas de ecosistema
     */
    struct EcosystemStats {
        int grazeCount = 0;
        int bushCount = 0;
        int treeCount = 0;
        float averageHealth = 1.0f;
        float biodiversity = 0.0f;  // Shannon index
        float biomass = 0.0f;        // Total biomass
    };
    
    EcosystemStats getStatistics() const;
    
    // ===== SETTERS =====
    
    void setGlobalLight(float light) {
        globalLight = glm::clamp(light, 0.0f, 1.0f);
    }
    
    void setGlobalWater(float water) {
        globalWater = glm::clamp(water, 0.0f, 1.0f);
    }
    
    void setGlobalNutrients(float nutrients) {
        globalNutrients = glm::clamp(nutrients, 0.0f, 1.0f);
    }
    
    void setSimulationSpeed(float speed) {
        simulationSpeed = glm::max(speed, 0.1f);
    }
    
    void setMaxPlants(int max) {
        maxPlants = glm::max(max, 1);
    }
};
