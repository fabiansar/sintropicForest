#pragma once

#include <glm/glm.hpp>
#include <string>

/**
 * ============================================================================
 * CLASE BASE PARA TODAS LAS ENTIDADES
 * ============================================================================
 * 
 * Esta es una PROPUESTA de refactorización para mejor escalabilidad.
 * Actualmente NO se usa (graphicsEngine es monolítica), pero proporciona
 * el patrón para futuras extensiones.
 * 
 * PROPÓSITO:
 * Permitir agregar cualquier tipo de entidad (plantas, árboles, animales,
 * objetos interactivos) sin duplicar código.
 * 
 * EJEMPLO DE USO FUTURO:
 *   class Plant : public Entity { ... };
 *   class Animal : public Entity { ... };
 *   class RockFormation : public Entity { ... };
 * 
 * Todos podrían estar en un EntityManager:
 *   entityManager.addEntity(std::make_unique<Plant>(...));
 *   entityManager.updateAll(deltaTime);  // Actualiza plantas + animales + rocks
 *   entityManager.renderAll(renderer);   // Renderiza todo
 */

// Forward declarations
class Renderer;

class Entity {
protected:
    /** Posición en el mundo 3D */
    glm::vec3 position;
    
    /** Orientación (rotación) */
    glm::vec3 orientation;
    
    /** Si está activa o no (para activar/desactivar sin eliminar) */
    bool active;
    
    /** ID único para identificar esta entidad */
    unsigned int entityID;

public:
    /** Constructor */
    Entity(const glm::vec3& pos = glm::vec3(0.0f))
        : position(pos), orientation(glm::vec3(0.0f)), active(true), entityID(0) {}
    
    /** Destructor virtual (importante para herencia) */
    virtual ~Entity() = default;

    // ========================================================================
    // MÉTODOS VIRTUALES PUROS (las subclases DEBEN implementar)
    // ========================================================================

    /**
     * Actualizar lógica de la entidad
     * @param deltaTime Tiempo desde el último frame en segundos
     */
    virtual void update(float deltaTime) = 0;

    /**
     * Renderizar esta entidad usando el renderer proporcionado
     * @param renderer Puntero a renderizador para dibujar
     */
    virtual void render(Renderer* renderer) = 0;

    /**
     * Obtener tipo de entidad como string
     * @return Nombre del tipo (ej: "Plant", "Animal", "Rock")
     */
    virtual std::string getType() const = 0;

    // ========================================================================
    // MÉTODOS PÚBLICOS (utilidades)
    // ========================================================================

    /** Getter - posición */
    glm::vec3 getPosition() const { return position; }

    /** Setter - posición */
    void setPosition(const glm::vec3& newPos) { position = newPos; }

    /** Getter - orientación */
    glm::vec3 getOrientation() const { return orientation; }

    /** Setter - orientación */
    void setOrientation(const glm::vec3& newOri) { orientation = newOri; }

    /** Getter - está activa? */
    bool isActive() const { return active; }

    /** Setter - activar/desactivar */
    void setActive(bool newState) { active = newState; }

    /** Getter - ID único */
    unsigned int getEntityID() const { return entityID; }

    /** Setter - ID único */
    void setEntityID(unsigned int newID) { entityID = newID; }

    /**
     * Calcular distancia a otra entidad
     * @param other Otra entidad
     * @return Distancia en unidades 3D
     */
    float distanceTo(const Entity& other) const {
        return glm::distance(position, other.position);
    }

    /**
     * Verificar si está dentro de un radio de otra entidad
     * @param other Otra entidad
     * @param radius Radio en unidades
     * @return true si distancia <= radius
     */
    bool isWithinRadius(const Entity& other, float radius) const {
        return distanceTo(other) <= radius;
    }
};

/**
 * ============================================================================
 * NOTAS DE IMPLEMENTACIÓN
 * ============================================================================
 * 
 * PASO 1: Crear subclase Plant
 * ───────────────────────────
 * class Plant : public Entity {
 *   private:
 *     PlantType type;
 *     float age;
 *     ...
 *   public:
 *     void update(float dt) override { ... }
 *     void render(Renderer* r) override { ... }
 *     std::string getType() const override { return "Plant"; }
 * };
 * 
 * PASO 2: Crear EntityManager
 * ────────────────────────
 * class EntityManager {
 *   private:
 *     std::vector<std::unique_ptr<Entity>> entities;
 *   public:
 *     void addEntity(std::unique_ptr<Entity> e);
 *     void updateAll(float dt);
 *     void renderAll(Renderer* r);
 * };
 * 
 * PASO 3: Usar en GameManager
 * ────────────────────────
 * class GameManager {
 *   private:
 *     EntityManager entityMgr;
 *   public:
 *     void update(float dt) {
 *       entityMgr.updateAll(dt);
 *     }
 * };
 * 
 * BENEFICIOS:
 * ✓ Agregar nuevo tipo = solo crear subclase
 * ✓ Actualización centralizada
 * ✓ Renderizado centralizado
 * ✓ Fácil de testear
 * ✓ Fácil de depurar
 */
