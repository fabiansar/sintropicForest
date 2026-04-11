#pragma once

#include "Config.h"
#include <glm/glm.hpp>
#include <random>

/**
 * ============================================================================
 * GAME LOGIC
 * ============================================================================
 * 
 * Gestiona la lógica del juego:
 * - Plantas y su ciclo de vida
 * - Probabilidades de tipos de plantas
 * - Limitaciones (máximo de plantas)
 * - Actualización de estado en cada frame
 * 
 * Completamente desacoplado de rendering y input.
 * Desacoplado de GLFW y OpenGL.
 */

class GameLogic {
private:
    std::vector<Plant> plants;
    
    // Configuración de plantas
    float plantProbabilityTree = 0.05f;
    float plantProbabilityBush = 0.15f;
    // Grass es el resto (implícitamente 0.80)
    
    int maxPlants = 500;
    
    // Tiempo transcurrido desde inicio
    float elapsedTime = 0.0f;
    
    // Random number generator
    std::mt19937 rng;

public:
    GameLogic() : rng(std::random_device{}()) {}

    /**
     * Resetear estado del juego
     */
    void reset() {
        plants.clear();
        elapsedTime = 0.0f;
    }

    /**
     * Actualizar lógica cada frame
     * @param deltaTime: tiempo desde último frame (segundos)
     */
    void update(float deltaTime) {
        elapsedTime += deltaTime;
        
        // Aquí irían actualizaciones de plantas si tuvieran ciclo de vida
        // Por ahora, las plantas son estáticas (solo animación visual)
    }

    /**
     * Agregar planta en posición específica
     * El tipo se determina por probabilidad
     * 
     * @param position: posición donde plantar
     * @return true si se agregó, false si se alcanzó límite
     */
    bool addPlant(const glm::vec3& position) {
        if (plants.size() >= (size_t)maxPlants) {
            return false;  // Límite alcanzado
        }
        
        Plant newPlant;
        newPlant.position = position;
        newPlant.createdTime = elapsedTime;
        
        // Determinar tipo por probabilidad
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float rand = dist(rng);
        
        if (rand < plantProbabilityTree) {
            newPlant.type = TREE;
        } else if (rand < plantProbabilityTree + plantProbabilityBush) {
            newPlant.type = BUSH;
        } else {
            newPlant.type = GRASS;
        }
        
        plants.push_back(newPlant);
        return true;
    }

    /**
     * Eliminar planta más cercana a la posición dada
     * Se usa para eliminar plantas haciendo click derecho
     * 
     * @param position: posición del raycast desde donde buscar
     * @param maxDistance: distancia máxima para considerar como "cercana" (default 3.0)
     * @return true si se eliminó una planta, false si no había ninguna cerca
     */
    bool removeNearestPlant(const glm::vec3& position, float maxDistance = 3.0f) {
        if (plants.empty()) {
            return false;
        }
        
        int closestIndex = -1;
        float closestDistance = maxDistance;
        
        for (size_t i = 0; i < plants.size(); ++i) {
            float dist = glm::distance(plants[i].position, position);
            if (dist < closestDistance) {
                closestDistance = dist;
                closestIndex = (int)i;
            }
        }
        
        if (closestIndex >= 0) {
            plants.erase(plants.begin() + closestIndex);
            return true;
        }
        
        return false;
    }

    /**
     * Obtener contador de plantas por tipo
     */
    void getPlantCounts(int& grass, int& bush, int& tree) const {
        grass = bush = tree = 0;
        for (const auto& plant : plants) {
            if (plant.type == GRASS) grass++;
            else if (plant.type == BUSH) bush++;
            else if (plant.type == TREE) tree++;
        }
    }

    // ===== GETTERS =====

    const std::vector<Plant>& getPlants() const {
        return plants;
    }

    size_t getPlantCount() const {
        return plants.size();
    }

    float getElapsedTime() const {
        return elapsedTime;
    }

    float getTreeProbability() const {
        return plantProbabilityTree;
    }

    float getBushProbability() const {
        return plantProbabilityBush;
    }

    float getGrassProbability() const {
        return 1.0f - plantProbabilityTree - plantProbabilityBush;
    }

    int getMaxPlants() const {
        return maxPlants;
    }

    // ===== SETTERS =====

    void setTreeProbability(float prob) {
        plantProbabilityTree = glm::clamp(prob, 0.0f, 0.5f);
    }

    void setBushProbability(float prob) {
        plantProbabilityBush = glm::clamp(prob, 0.0f, 0.5f);
    }

    void setMaxPlants(int max) {
        maxPlants = glm::max(max, 1);
    }
};
