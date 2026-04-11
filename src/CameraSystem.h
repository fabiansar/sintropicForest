#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * ============================================================================
 * CAMERA SYSTEM
 * ============================================================================
 * 
 * Gestiona la cámara orbital tipo juego de gestión/estrategia.
 * Responsabilidades:
 * - Posición de cámara (órbita alrededor de target)
 * - Rotación de cámara
 * - Altura de cámara
 * - Distancia de cámara
 * - View matrix
 * 
 * Parámetros configurables en tiempo de ejecución.
 */
class CameraSystem {
private:
    // Posición objetivo (punto al que mira la cámara)
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    
    // Ángulo de rotación alrededor del target (en grados)
    float rotation = 0.0f;
    
    // Parámetros de órbita
    float distance = 35.0f;
    float height = 15.0f;
    
    // Límites
    float minDistance = 20.0f;
    float maxDistance = 60.0f;
    float minHeight = 5.0f;
    float maxHeight = 30.0f;

public:
    /**
     * Resetear cámara a posición por defecto
     */
    void reset() {
        target = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = 0.0f;
        distance = 35.0f;
        height = 15.0f;
    }

    /**
     * Actualizar rotación de cámara
     * @param dRotation: cambio en grados (positivo = CCW, negativo = CW)
     */
    void rotate(float dRotation) {
        rotation += dRotation;
        // ✅ FIX BUG-6: usar glm::mod() en lugar de while loops
        rotation = glm::mod(rotation, 360.0f);
        if (rotation < 0.0f) rotation += 360.0f;
    }

    /**
     * Mover target de la cámara
     * @param dx, dz: desplazamiento en coordenadas mundo
     */
    void panTarget(float dx, float dz) {
        target.x += dx;
        target.z += dz;
    }

    /**
     * Mover target de la cámara (rotado según rotación de cámara)
     * @param forward: movimiento adelante/atrás
     * @param right: movimiento izquierda/derecha
     */
    void panTargetRotated(float forward, float right) {
        float angle = glm::radians(rotation);
        float cosA = cos(angle);
        float sinA = sin(angle);
        
        // Matriz de rotación 2D correcta:
        // x_new = x * cos(θ) - z * sin(θ)
        // z_new = x * sin(θ) + z * cos(θ)
        // Donde forward es eje Z, right es eje X
        float x = forward * cosA - right * sinA;
        float z = forward * sinA + right * cosA;
        
        target.x += x;
        target.z += z;
    }

    /**
     * Ajustar distancia (zoom)
     * @param dDistance: cambio en distancia (negativo = cercana, positivo = lejana)
     */
    void adjustDistance(float dDistance) {
        distance = glm::clamp(distance + dDistance, minDistance, maxDistance);
        // ✅ FIX BUG-5: mantener height proporcional a distance
        float ratio = 15.0f / 35.0f;  // Ratio original: height/distance
        height = distance * ratio;
        height = glm::clamp(height, minHeight, maxHeight);
    }

    /**
     * Ajustar altura de cámara
     * @param dHeight: cambio en altura
     */
    void adjustHeight(float dHeight) {
        height = glm::clamp(height + dHeight, minHeight, maxHeight);
    }

    // ===== GETTERS =====

    glm::vec3 getTarget() const {
        return target;
    }

    float getRotation() const {
        return rotation;
    }

    glm::vec3 getPosition() const {
        float angle = glm::radians(rotation);
        return target + glm::vec3(
            cos(angle) * distance,
            height,
            sin(angle) * distance
        );
    }

    /**
     * Obtener matriz VIEW para pass a shaders
     */
    glm::mat4 getViewMatrix() const {
        glm::vec3 camPos = getPosition();
        return glm::lookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    /**
     * Obtener matriz PROJECTION (típicamente la misma para toda la sesión)
     */
    static glm::mat4 getProjectionMatrix(float windowWidth, float windowHeight) {
        return glm::perspective(
            glm::radians(45.0f),
            windowWidth / windowHeight,
            0.1f,
            100.0f
        );
    }

    /**
     * Obtener posición de cámara formateada (para debug)
     */
    glm::vec2 getTargetXZ() const {
        return glm::vec2(target.x, target.z);
    }
};
