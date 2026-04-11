# ⚡ Quick Debugging Reference

Guía rápida para identificar y arreglar bugs rápidamente.

## 🎯 3-Step Bug Fix Framework

```
STEP 1: IDENTIFY (con Logger)
  ↓
STEP 2: LOCATE (en qué archivo)
  ↓
STEP 3: FIX (editar código)
```

---

## 🐛 Known Bugs & Quick Fixes

### Bug #1: Q/E Keys Still Rotating Camera

**Symptom**: Camera rotates when pressing Q or E  
**Should Be**: Q/E disabled (mouse right-click only)

**Quick Fix** (2 minutos):
```powershell
# 1. Search for Q/E processing in main.cpp
grep -n "keyQ\|keyE" src/main.cpp

# 2. Comment out these lines:
#    if (input.keyQ) camera.rotate(...);
#    if (input.keyE) camera.rotate(...);

# 3. Compile and test
.\build.ps1
.\build\3DEngine.exe
```

**Verify**:
- [ ] Q key does NOT rotate
- [ ] E key does NOT rotate
- [ ] Right-click drag SHOULD rotate (if working)

---

### Bug #2: Mouse Right-Click Not Rotating Camera

**Symptom**: Right-click drag doesn't rotate camera  
**Expected**: Smooth camera rotation

**Debug Steps** (5 minutos):

```powershell
# 1. Add debug logging to see mouse state
# Edit src/InputManager.h in update() method:

# Add after glfwGetCursorPos:
# Logger::debug("Mouse: (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")");
# Logger::debug("Delta: (" + std::to_string(mouseDeltaX) + ", " + std::to_string(mouseDeltaY) + ")");
# Logger::debug("RightPressed: " + std::to_string(mouseRightPressed));

# 2. Compile with Logger
.\build.ps1

# 3. Run and move mouse while holding right button
.\build\3DEngine.exe

# 4. Check logs
type game.log | Select-Object -Last 50

# ANALYZE:
# If mouseDeltaX = 0: Input not being read
# If RightPressed = false: Button detection broken
# If both non-zero: Problem is camera.rotate() method
```

**Quick Fix Options**:

**Option A**: InputManager not updating every frame
```cpp
// In main.cpp update() - MOVE THIS LINE:
// FROM: if (currentState == PLAYING) { inputManager->update(); ... }
// TO: Top of update() method
inputManager->update();  // Must be called EVERY frame!
```

**Option B**: Mouse delta calculation wrong
```cpp
// In InputManager.h - check update() logic:
// Should be:
currentState.mouseDeltaX = newMouseX - currentState.mouseX;
currentState.mouseDeltaY = newMouseY - currentState.mouseY;
currentState.mouseX = newMouseX;
currentState.mouseY = newMouseY;
```

**Option C**: Right mouse button constant wrong
```cpp
// In InputManager.h - verify:
// Should be: glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)
// NOT: GLFW_MOUSE_BUTTON_2 or other constant
```

---

## 🔧 Debug Workflow Template

When you encounter a bug:

```
1. DESCRIBE
   - What happens?
   - What should happen?
   - When does it happen?

2. LOCATE CODE
   - Which file?
   - Which function?
   - Which line(s)?

3. ADD LOGGING
   - Add Logger::debug() calls
   - Recompile
   - Run and check game.log

4. ANALYZE LOGS
   - Is variable X what I expect?
   - Is function Y being called?
   - Is order of execution correct?

5. FORM HYPOTHESIS
   - What is the root cause?
   - Test hypothesis with more logging

6. FIX
   - Make minimal change
   - Compile
   - Test

7. COMMIT
   git add -A
   git commit -m "fix: [BUG_NAME]"
   git push
```

---

## 📊 Logger Cheat Sheet

### Initialize Logger (do this in main())

```cpp
// Add at start of main() or in PLAYING state init:
Logger::init("game.log", Logger::Level::DEBUG);  // Level::DEBUG shows everything
```

### Add Debug Output

```cpp
// Simple string
Logger::debug("Message");

// With variables
Logger::debug("X=" + std::to_string(x) + ", Y=" + std::to_string(y));

// With GLM vectors
Logger::debug("Position: (" + std::to_string(pos.x) + ", " + 
              std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")");

// Conditional logging
if (someCondition) {
    Logger::warning("Unexpected condition!");
}
```

### View Logs

```powershell
# See entire log
type game.log

# Last 50 lines
type game.log | Select-Object -Last 50

# Find specific text
Select-String "ERROR\|WARNING" game.log

# Count occurrences
(Select-String "Mouse" game.log).Count

# Follow logs in real-time (Windows)
# Option 1: Use tail alternative
Get-Content game.log -Wait -Tail 20

# Option 2: Redirect to more readable output
type game.log | Format-Table -AutoSize
```

---

## ✅ Pre-Commit Checklist

Before committing a fix:

```
✓ Code compiles without errors: .\build.ps1
✓ Code compiles without warnings
✓ Tested manually in game
✓ Logs show expected behavior
✓ No new bugs introduced
✓ Commit message is descriptive
```

---

## 🚀 Command Reference

### Build & Run
```powershell
# Compile
.\build.ps1

# Run game
.\build\3DEngine.exe

# Kill game (if frozen)
Stop-Process -Name 3DEngine.exe -Force
```

### Git Workflow
```powershell
# Check status
git status

# Stage changes
git add -A

# Commit
git commit -m "fix: Issue description"

# Push
git push

# View recent commits
git log --oneline -10
```

### Logging Workflow
```powershell
# After running game:
type game.log

# Search for errors
Select-String "error\|Error\|ERROR" game.log

# Get statistics
$lines = Get-Content game.log
$errors = ($lines | Select-String "ERROR").Count
$warnings = ($lines | Select-String "WARNING").Count
Write-Host "Errors: $errors, Warnings: $warnings"
```

---

## 📋 Issue Template

When reporting a bug, use this template:

```
## Issue: [SHORT_DESCRIPTION]

**Symptom**: 
[What happens]

**Expected**:
[What should happen]

**Root Cause Hypothesis**:
[My guess for what's broken]

**Logs**:
[Relevant log output]

**Files Involved**:
- src/[FILE1].h
- src/[FILE2].cpp

**Fix Effort**: [TRIVIAL/EASY/MEDIUM/HARD]

**Test Method**:
[How to verify the fix]
```

---

## 💭 Debugging Mind Map

```
Bug Found
    ↓
Add Logger statements
    ↓
Recompile & Run
    ↓
Check game.log
    ↓
     ├─ Variable wrong? → Fix logic
     ├─ Function not called? → Add call
     ├─ Wrong order? → Reorder
     └─ Library issue? → Check version
    ↓
Fix code
    ↓
Recompile & Test
    ↓
Commit & Push
```

---

## 🎯 This Week's Debug Targets

**Q/E Bug**: 10 minutes  
→ Search for keyQ/keyE → Comment out → Test

**Mouse Rotation Bug**: 20 minutes  
→ Add Logger → Check delta → Find root cause → Fix

**Next Priority**: TBD by user

---

**Last Updated**: 2026-04-11  
**Framework Version**: 1.0  

