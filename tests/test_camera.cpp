#include "test_framework.h"
#include "../src/CameraSystem.h"
#include <glm/glm.hpp>
#include <cmath>

/**
 * ============================================================================
 * CAMERA SYSTEM TESTS
 * ============================================================================
 * 
 * Tests para validar el funcionamiento correcto de CameraSystem
 */

// Test 1: Rotación básica
TEST(CameraSystem, "Rotation updates correctly") {
    CameraSystem cam;
    cam.rotate(45.0f);
    float rotation = cam.getRotation();
    
    // Permitir pequeña tolerancia por floating point
    float tolerance = 0.01f;
    ASSERT_TRUE(std::abs(rotation - 45.0f) < tolerance, 
                "Camera rotation should be approximately 45.0f");
}

// Test 2: Rotación se resetea a 360
TEST(CameraSystem, "Rotation wraps at 360") {
    CameraSystem cam;
    cam.rotate(360.0f);
    float rotation = cam.getRotation();
    
    ASSERT_EQUAL(rotation, 0.0f, "360 degree rotation should wrap to 0");
}

// Test 3: Rotación negativa
TEST(CameraSystem, "Negative rotation works") {
    CameraSystem cam;
    cam.rotate(-45.0f);
    float rotation = cam.getRotation();
    
    ASSERT_TRUE(std::abs(rotation - 315.0f) < 0.01f, 
                "Negative rotation should wrap correctly");
}

// Test 4: Distance ajustment within bounds
TEST(CameraSystem, "Distance adjusts within bounds") {
    CameraSystem cam;
    
    // Distancia inicial
    float initialDistance = 35.0f;
    
    // Intentar ajustar
    cam.adjustDistance(5.0f);
    
    // No podemos acceder a distance directamente, pero position.length() cambia
    glm::vec3 pos1 = cam.getPosition();
    
    cam.adjustDistance(5.0f);
    glm::vec3 pos2 = cam.getPosition();
    
    // pos2 debería estar más lejos del target que pos1
    ASSERT_TRUE(glm::distance(cam.getTarget(), pos2) > 
                glm::distance(cam.getTarget(), pos1),
                "Increasing distance should move camera away");
}

// Test 5: Movement sin rotación
TEST(CameraSystem, "Pan target moves correctly") {
    CameraSystem cam;
    glm::vec3 initialTarget = cam.getTarget();
    
    cam.panTarget(1.0f, 1.0f);
    glm::vec3 newTarget = cam.getTarget();
    
    // Target debería haberse movido
    ASSERT_TRUE(glm::distance(initialTarget, newTarget) > 0.1f,
                "Pan should move target position");
}

// Test 6: Reset functionality
TEST(CameraSystem, "Reset returns to default state") {
    CameraSystem cam;
    
    // Cambiar diversos parámetros
    cam.rotate(90.0f);
    cam.panTarget(5.0f, 5.0f);
    
    // Reset
    cam.reset();
    
    // Verificar que vuelve a default
    ASSERT_EQUAL(cam.getRotation(), 0.0f, "Reset should return rotation to 0");
    ASSERT_EQUAL(cam.getTarget(), glm::vec3(0.0f), "Reset should return target to origin");
}

// Test 7: View matrix es válida
TEST(CameraSystem, "View matrix is valid") {
    CameraSystem cam;
    glm::mat4 viewMatrix = cam.getViewMatrix();
    
    // Una matriz válida de view no debería ser identity
    glm::mat4 identity(1.0f);
    ASSERT_FALSE(viewMatrix == identity, 
                 "View matrix should not be identity");
}

/**
 * PLAN DE TESTING GENERAL:
 * 
 * 1. CAMERA TESTS (aquí) ✓
 *    - Rotación
 *    - Distancia/Zoom
 *    - Movimiento
 *    - Matrices
 * 
 * 2. INPUT TESTS (próximo: test_input.cpp)
 *    - Detección de teclas
 *    - Detección de ratón
 *    - Mouse delta
 *    - Debouncing
 * 
 * 3. LOGGER TESTS (próximo: test_logger.cpp)
 *    - Logging a archivo
 *    - Filtrado por nivel
 *    - Timestamps
 * 
 * 4. INTEGRATION TESTS (futuro: test_integration.cpp)
 *    - Camera + Input
 *    - Logger en todo el sistema
 * 
 * CÓMO EJECUTAR:
 * 1. Crear main de tests:
 *    g++ tests/test_camera.cpp -I. -o tests/run_tests
 *    ./tests/run_tests
 */
