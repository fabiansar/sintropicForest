#pragma once

#include "Config.h"
#include <chrono>

/**
 * ============================================================================
 * STATE MANAGER
 * ============================================================================
 * 
 * Gestiona la máquina de estados de la aplicación.
 * Estados:
 * - SPLASH: Pantalla inicial (3 segundos)
 * - MENU: Menú principal
 * - PLAYING: Juego en progreso
 * - SETTINGS: Configuración
 * - CREDITS: Créditos
 * 
 * Responsabilidades:
 * - Transiciones entre estados
 * - Validación de transiciones
 * - Timing de splash screen
 * - Historial de estados
 */

class StateManager {
private:
    GameState currentState = SPLASH;
    GameState nextState = SPLASH;
    std::chrono::steady_clock::time_point splashStartTime;

public:
    StateManager() {
        splashStartTime = std::chrono::steady_clock::now();
    }

    /**
     * Actualizar máquina de estados
     * Debe llamarse cada frame
     */
    void update() {
        // Manejar transición de SPLASH después de 3 segundos
        if (currentState == SPLASH) {
            auto elapsed = std::chrono::steady_clock::now() - splashStartTime;
            if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= 3) {
                currentState = MENU;
                nextState = MENU;
            }
        } else {
            // Aplicar cualquier transición pendiente
            currentState = nextState;
        }
    }

    /**
     * Solicitar transición a nuevo estado
     * @param newState: estado destino
     * @return true si la transición es válida
     */
    bool requestTransition(GameState newState) {
        // Validar transiciones (cualquier estado puede volver a MENU)
        if (newState == MENU) {
            nextState = MENU;
            return true;
        }
        
        // MENU puede ir a PLAYING, SETTINGS, CREDITS
        if (currentState == MENU || nextState == MENU) {
            if (newState == PLAYING || newState == SETTINGS || newState == CREDITS) {
                nextState = newState;
                return true;
            }
        }
        
        // PLAYING puede ir a MENU
        if (currentState == PLAYING || nextState == PLAYING) {
            if (newState == MENU) {
                nextState = MENU;
                return true;
            }
        }
        
        // SETTINGS puede ir a MENU
        if (currentState == SETTINGS || nextState == SETTINGS) {
            if (newState == MENU) {
                nextState = MENU;
                return true;
            }
        }
        
        // CREDITS puede ir a MENU
        if (currentState == CREDITS || nextState == CREDITS) {
            if (newState == MENU) {
                nextState = MENU;
                return true;
            }
        }
        
        return false;  // Transición inválida
    }

    /**
     * ¿Estamos en un estado específico?
     */
    bool isInState(GameState state) const {
        return currentState == state;
    }

    /**
     * ¿Acabamos de entrar en este estado este frame?
     */
    bool justEnteredState(GameState state) const {
        // Para versión simple, no rastreamos transiciones
        // Extender si necesitas este comportamiento
        return false;
    }

    // ===== GETTERS =====

    GameState getCurrentState() const {
        return currentState;
    }

    GameState getNextState() const {
        return nextState;
    }

    const char* getStateName(GameState state) const {
        switch (state) {
            case SPLASH: return "SPLASH";
            case MENU: return "MENU";
            case PLAYING: return "PLAYING";
            case SETTINGS: return "SETTINGS";
            case CREDITS: return "CREDITS";
            default: return "UNKNOWN";
        }
    }

    const char* getCurrentStateName() const {
        return getStateName(currentState);
    }
};
