# DEBUGGING - Plant Creation Issue

**Problem**: Plants no se crean al click izquierdo
**Potential Causes**:

## 1. Input Detection Chain ✓
- InputManager detects GLFW_MOUSE_BUTTON_LEFT
- Sets currentState.mouseLeftClick = true for ONE frame
- **ISSUE**: Only true for single frame - might miss if GraphicsEngine not updating sync

## 2. GraphicsEngine Update Order
- handleInput() called in which frame?
- Does raycast get valid hitPos?
- Does addPlant() actually add to gameLogic->plants?

## 3. GameLogic::addPlant()
- Creates Plant struct
- Pushes to plants vector
- Should increment getPlantCount()

## 4. Rendering
- renderPlants() reads from gameLogic->getPlants()
- Should render all plants as GL_POINTS

## 5. Potential Fixes to Try
1. Add logging to verify plant creation
2. Check if mouseLeftClick frame timing is correct
3. Verify getPlants() returns non-empty vector
4. Check if renderPlants() is being called

## Hypothesis
Most likely: Plant is being created but NOT RENDERED
- gameLogic->plants has data
- But renderPlants() not displaying them correctly
