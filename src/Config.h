#pragma once

#include <glm/glm.hpp>

/**
 * ============================================================================
 * CONFIGURACIÓN CENTRAL DEL PROYECTO
 * ============================================================================
 * 
 * Este archivo contiene TODAS las constantes, estructuras y enumeraciones
 * del proyecto. Modificar aquí afecta al comportamiento global sin necesidad
 * de recompilar multiple archivos.
 * 
 * SECCIONES:
 * 1. Ventana
 * 2. Sistema de Plantas
 * 3. Cámara
 * 4. Terreno
 * 5. Enumeraciones y Estructuras
 */

// ============================================================================
// 1. CONFIGURACIÓN DE VENTANA
// ============================================================================

const int WINDOW_WIDTH = 1280;   ///< Ancho de la ventana en píxeles
const int WINDOW_HEIGHT = 720;   ///< Alto de la ventana en píxeles

// ============================================================================
// 2. CONFIGURACIÓN DEL SISTEMA DE PLANTAS
// ============================================================================

/**
 * TIPOS DE PLANTAS
 * 
 * Cada tipo tiene características visuales diferentes:
 * - GRASS (Hierba): Pequeña, verde claro
 * - BUSH (Arbusto): Mediana, verde medio
 * - TREE (Árbol): Grande, verde oscuro
 * 
 * Para agregar un nuevo tipo:
 * 1. Agregar nuevo valor al enum
 * 2. Agregar constante de probabilidad
 * 3. Agregar caso en PlantFactory o addRandomLight()
 */
enum PlantType {
    GRASS = 0,   ///< Hierba - Pequeña (3px)
    BUSH = 1,    ///< Arbusto - Mediana (5px)
    TREE = 2     ///< Árbol - Grande (8px)
};

// Límites de plantas
const int NUM_LIGHTS = 0;        ///< Plantas iniciales (0 = mapa vacío)
const int MAX_LIGHTS = 500;      ///< Máximo de plantas permitidas simultáneamente
                                 ///< NOTA: Incrementar reduce FPS. Rango recomendado: 250-1000

// ============================================================================
// 2B. PROBABILIDADES DE PLANTAS
// ============================================================================

/**
 * DISTRIBUCIÓN DE TIPOS DE PLANTAS
 * 
 * Cuando se planta una nueva, se elige un tipo según estas probabilidades.
 * El total debe sumar APROXIMADAMENTE 1.0f
 * 
 * Ejemplo en línea 6:
 *   rand = 0.4  → < 0.05 (TREE)? NO  → < 0.20 (BUSH)? NO → GRASS ✓
 *   rand = 0.1  → < 0.05 (TREE)? NO  → < 0.20 (BUSH)? SÍ → BUSH ✓
 *   rand = 0.02 → < 0.05 (TREE)? SÍ                       → TREE ✓
 */
const float PLANT_PROBABILITY_TREE = 0.05f;    ///< 5% árboles
const float PLANT_PROBABILITY_BUSH = 0.15f;    ///< 15% arbustos
const float PLANT_PROBABILITY_GRASS = 0.80f;   ///< 80% hierba
                                               ///< TOTAL: 1.00f

// ============================================================================
// 2C. PROPIEDADES DE PLANTAS (PARA FUTURAS EXTENSIONES)
// ============================================================================

/**
 * Constantes para sistemas futuros de plantas más avanzadas.
 * Por ahora no se usan, pero están aquí como referencia.
 */
const float PLANT_GROWTH_RATE = 1.0f;          ///< Qué tan rápido crece
const int PLANT_MAX_AGE = 3600;                ///< Edad máxima en segundos (1 hora)
const float PLANT_DEATH_THRESHOLD = 50.0f;     ///< % de salud para morir

// ============================================================================
// 3. CONFIGURACIÓN DE CÁMARA (Vista Isométrica/Estrategia)
// ============================================================================

/**
 * SISTEMA DE CÁMARA
 * 
 * La cámara orbita alrededor de un punto objetivo (cameraTarget).
 * Ángulos:
 *   0°   = Este (derecha)
 *   90°  = Sur (arriba)
 *   180° = Oeste (izquierda)
 *   270° = Norte (abajo)
 */

const float CAMERA_HEIGHT = 50.0f;           ///< Altura absoluta de la cámara (eje Y)
const float CAMERA_DISTANCE = 45.0f;         ///< Radio de órbita alrededor del objetivo
const float CAMERA_ROTATION_SPEED = 3.0f;    ///< Multiplicador de velocidad con Q/E
const float CAMERA_MOVEMENT_SPEED = 20.0f;   ///< Velocidad de movimiento WASD

// Sugerencias para ajustar:
// - Aumentar CAMERA_HEIGHT para ver más área, reducir para detalle
// - Aumentar CAMERA_DISTANCE para vista más lejana
// - Incrementar ROTATION_SPEED para rotación más rápida
// - MOVEMENT_SPEED afecta WASD (flechas siempre 0.3f codeados)

// ============================================================================
// 4. CONFIGURACIÓN DEL TERRENO
// ============================================================================

/**
 * GENERACIÓN DEL TERRENO
 * 
 * El terreno es un grid (malla cuadrada) de vértices.
 * 
 * TERRAIN_SIZE = 100 unidades
 * TERRAIN_RESOLUTION = 50 = 50 cuadrados en cada dirección
 * 
 * Cálculo: Cada cuadrado = 100/50 = 2x2 unidades
 *          Total de triángulos = 50*50*2 = 5000
 * 
 * Rendering: glDrawElements renderiza todos en una pasada
 */

const int TERRAIN_SIZE = 100;        ///< Tamaño del terreno (100x100 unidades)
                                     ///< NOTA: Límites van de -50 a +50 en X y Z

const int TERRAIN_RESOLUTION = 50;   ///< Resolución de la malla (50x50 cuadrados)
                                     ///< Reducir para mejorar FPS (ej: 25), 
                                     ///< Aumentar para más detalle (ej: 100)

const float TERRAIN_COLOR_CHANGE_SPEED = 2.0f;  ///< Velocidad de animación del terreno
                                               ///< (cambio interpolado entre colores)

const float LIGHT_INFLUENCE_DISTANCE = 15.0f;   ///< Distancia a la que plantas oscurecen
                                               ///< el terreno (solo visual, no interactivo)

// ============================================================================
// 4C. COLORES DE PLANTAS (VERDE-MARRÓN)
// ============================================================================

const glm::vec3 PLANT_COLOR_GRASS = glm::vec3(0.4f, 0.8f, 0.2f);   // Verde claro
const glm::vec3 PLANT_COLOR_BUSH = glm::vec3(0.3f, 0.6f, 0.2f);    // Verde oscuro
const glm::vec3 PLANT_COLOR_TREE = glm::vec3(0.5f, 0.3f, 0.1f);    // Marrón

// Tamaños de punto para cada tipo (en píxeles OpenGL)
const float PLANT_SIZE_GRASS = 3.0f;   // Pequeño
const float PLANT_SIZE_BUSH = 6.0f;    // Medio
const float PLANT_SIZE_TREE = 10.0f;   // Grande

// Animación al crear (flash de color)
const float PLANT_CREATION_FLASH_DURATION = 0.5f;  // Segundos

// ============================================================================
// 4B. CONFIGURACIÓN DE PERLIN NOISE - TERRENO REALISTA
// ============================================================================

const float TERRAIN_NOISE_SCALE = 0.1f;        ///< Escala del ruido (menor = más ondulado)
const float TERRAIN_NOISE_HEIGHT = 10.0f;      ///< Altura máxima del terreno (aumentado para mayor visibilidad)
const int TERRAIN_NOISE_OCTAVES = 4;           ///< Octavas de Perlin (más = más detalle)
const float TERRAIN_NOISE_PERSISTENCE = 0.5f;  ///< Persistencia del ruido (0-1)

// ============================================================================
// 5A. ENUMERACIONES - ESTADOS DEL JUEGO
// ============================================================================

/**
 * MÁQUINA DE ESTADOS DEL PROGRAMA
 * 
 * Flujo típico:
 *   SPLASH (3 seg) → MENU → PLAYING (jugando) ↔ MENU
 *                       ↘ SETTINGS (slider volumen)
 *                       ↘ PLAYLIST (reproducir canciones)
 *                       ↘ CREDITS (créditos)
 * 
 * Cada estado tiene su RENDER función específica.
 */
enum GameState {
    SPLASH,       ///< Pantalla inicial "Made by Fabian" (dura 3 segundos)
    MENU,         ///< Menú principal con botones (Play, Playlist, Settings, Credits)
    PLAYING,      ///< Escena 3D con plantas, terreno, lógica
    PLAYLIST,     ///< Reproductor de música clásica (nueva opción)
    SETTINGS,     ///< Configuración (volumen, opciones de plantas, etc.)
    CREDITS       ///< Pantalla de créditos
};

// ============================================================================
// 5B. ESTRUCTURA - PLANTA / PUNTO DE LUZ
// ============================================================================

/**
 * ESTRUCTURA PLANT
 * 
 * Representa UNA PLANTA individual en el mundo 3D.
 * Renderizada como geometría modular con átomos (de ParticleAtomSystem).
 * 
 * Campos:
 * - position: Ubicación en el mundo (X, Y, Z)
 * - type: Tipo de planta (GRASS=pequeña, BUSH=media, TREE=grande)
 * - createdTime: Para animación de color al crear
 * - atomMeshId: ID de la malla atómica en ParticleAtomSystem
 */
struct Plant {
    glm::vec3 position;        ///< Posición en el mundo
    int type;                  ///< PlantType: GRASS=0, BUSH=1, TREE=2
    float createdTime;         ///< Tiempo de creación para animación
    uint32_t atomMeshId = 0;   ///< Reference to ParticleAtomSystem mesh
    
    Plant() : position(0.0f), type(0), createdTime(0.0f), atomMeshId(0) {}
};

// ============================================================================
// 6. MODULAR PLANT GENERATION (ParticleAtomSystem)
// ============================================================================

// Atom complexity per plant type (balanced approach)
const int GRASS_ATOM_COUNT = 12;        // Simple stem + leaves
const int BUSH_ATOM_COUNT = 40;         // Branched structure
const int TREE_ATOM_COUNT = 70;         // Complex multi-branch

// Generation parameters
const float PLANT_HEIGHT_GRASS = 2.0f;
const float PLANT_HEIGHT_BUSH = 4.0f;
const float PLANT_HEIGHT_TREE = 6.5f;

// Destruction parameters
const float ATOM_DAMAGE_PER_CLICK = 0.5f;     // Damage on right-click
const float ATOM_HEALTH_THRESHOLD = 0.0f;    // Health below = broken
const float DEGRADATION_TIME = 15.0f;        // Seconds to degrade to soil

// ============================================================================
// 7. INFORMACIÓN Y DOCUMENTACIÓN
// ============================================================================

/**
 * CÓMO MODIFICAR ESTE ARCHIVO
 * 
 * Cambio Fácil (1 minuto):
 * - MAX_LIGHTS: Límite superior de plantas
 * - Probabilidades: Ajustar ratios TREE/BUSH/GRASS
 * 
 * Cambio Medio (5 minutos):
 * - CAMERA_*: Cómo funciona la cámara
 * - TERRAIN_*: Cómo se genera el mapa
 * 
 * Cambio Avanzado (15-30 minutos):
 * - Agregar nuevo PlantType (requiere cambios en otros archivos)
 * - Agregar nuevo GameState (requiere agregar renderXXX())
 * - Cambiar estructura Light (requiere actualizar GraphicsEngine)
 * 
 * RECORDAR: Siempre compilar después de cambios con:
 *   powershell .\build.ps1
 */

// ============================================================================
// FIN DE CONFIGURACIÓN
// ============================================================================

