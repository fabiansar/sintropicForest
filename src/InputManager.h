#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

/**
 * ============================================================================
 * INPUT MANAGER
 * ============================================================================
 * 
 * Gestiona toda la entrada del usuario (teclado, ratón).
 * Responsabilidades:
 * - Detectar inputs de teclado
 * - Detectar clicks del ratón
 * - Realizar raycast mouse-to-world
 * - Mantener estado de inputs
 * 
 * Desacoplado de lógica de game - solo reporta inputs crudos.
 */
class InputManager {
public:
    struct InputState {
        // Keyboard
        bool keyW = false;
        bool keyS = false;
        bool keyA = false;
        bool keyD = false;
        bool keyQ = false;
        bool keyE = false;
        bool keyUp = false;
        bool keyDown = false;
        bool keyLeft = false;
        bool keyRight = false;
        bool keyEscape = false;
        
        // Mouse
        bool mouseLeftClick = false;
        bool mouseRightClick = false;
        double mouseX = 0.0;
        double mouseY = 0.0;
        double scrollY = 0.0;  // Cambio en scroll wheel este frame
    };

private:
    GLFWwindow* window;
    InputState currentState;
    InputState previousState;
    bool escapePressed = false;  // Escape debounce
    bool leftMousePressed = false;  // Mouse left debounce
    bool rightMousePressed = false;  // Mouse right debounce
    double scrollAccumulator = 0.0;  // Acumulador de scroll

    // Static callback para scroll wheel (GLFW solo permite function pointers)
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        if (manager) {
            manager->scrollAccumulator += yoffset;
        }
    }

public:
    InputManager(GLFWwindow* win) : window(win) {
        // Registrar callback de scroll wheel
        glfwSetWindowUserPointer(win, this);
        glfwSetScrollCallback(win, scrollCallback);
    }

    /**
     * Actualizar estado de inputs - debe llamarse cada frame
     */
    void update() {
        previousState = currentState;
        
        // Reiniciar valores que solo duran un frame
        currentState.scrollY = scrollAccumulator;
        scrollAccumulator = 0.0;
        
        // Keyboard state
        currentState.keyW = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        currentState.keyS = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        currentState.keyA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        currentState.keyD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        currentState.keyQ = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
        currentState.keyE = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
        currentState.keyUp = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        currentState.keyDown = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        currentState.keyLeft = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        currentState.keyRight = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        
        // ESC with debounce
        bool escapeRaw = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
        if (escapeRaw && !escapePressed) {
            currentState.keyEscape = true;
            escapePressed = true;
        } else {
            currentState.keyEscape = false;
            if (!escapeRaw) {
                escapePressed = false;
            }
        }
        
        // Mouse state
        glfwGetCursorPos(window, &currentState.mouseX, &currentState.mouseY);
        
        // Left mouse button with debounce
        int mouseButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseButtonState == GLFW_PRESS && !leftMousePressed) {
            currentState.mouseLeftClick = true;
            leftMousePressed = true;
        } else {
            currentState.mouseLeftClick = false;
            if (mouseButtonState == GLFW_RELEASE) {
                leftMousePressed = false;
            }
        }
        
        // Right mouse button with debounce
        int rightMouseButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        if (rightMouseButtonState == GLFW_PRESS && !rightMousePressed) {
            currentState.mouseRightClick = true;
            rightMousePressed = true;
        } else {
            currentState.mouseRightClick = false;
            if (rightMouseButtonState == GLFW_RELEASE) {
                rightMousePressed = false;
            }
        }
    }

    /**
     * Acceso a estado actual
     */
    const InputState& getState() const {
        return currentState;
    }

    /**
     * Consultas de teclas individuales
     */
    bool isKeyPressed(int glfwKey) const {
        return glfwGetKey(window, glfwKey) == GLFW_PRESS;
    }

    bool isKeyJustPressed(int glfwKey) const {
        return isKeyPressed(glfwKey) && !wasKeyPressed(glfwKey);
    }

    bool wasKeyPressed(int glfwKey) const {
        // Para esta versión, solo verificamos debounce para ESC y mouse
        // Extender si necesitas eventos para otras teclas
        return false;
    }

    /**
     * Posición actual del ratón
     */
    glm::vec2 getMousePosition() const {
        return glm::vec2(currentState.mouseX, currentState.mouseY);
    }

    /**
     * ¿Se presionó el botón izquierdo del ratón este frame?
     */
    bool wasMouseLeftClickedThisFrame() const {
        return currentState.mouseLeftClick;
    }

    /**
     * ¿Se presionó el botón derecho del ratón este frame?
     */
    bool wasMouseRightClickedThisFrame() const {
        return currentState.mouseRightClick;
    }

    /**
     * Obtener valor del scroll wheel este frame
     * Positivo = arriba (zoom in), Negativo = abajo (zoom out)
     */
    double getScrollY() const {
        return currentState.scrollY;
    }
};
