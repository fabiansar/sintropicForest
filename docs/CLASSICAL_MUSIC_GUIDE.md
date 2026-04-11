# 🎵 Classical Music Feature Guide

## Overview
The game now includes 5 iconic **public-domain classical compositions** encoded in 8-bit chiptune format using **π π π π (Pi) notation**.

> **All compositions are copyright-free** (composed >70 years ago) and safe for any use!

---

## Playing Music

### Keyboard Shortcuts

While in the **PLAYING** state, press:

| Key | Composition | Composer | Era |
|-----|-------------|----------|-----|
| **F1** | 🎵 Ode to Joy | Beethoven (1824) | Romantic |
| **F2** | 🎹 Für Elise | Beethoven (1810) | Romantic |
| **F3** | 🎻 Eine kleine Nachtmusik | Mozart (1787) | Classical |
| **F4** | 💫 Clair de Lune | Debussy (1890) | Impressionist |
| **F5** | 🎺 Marcha de Zacatecas | Mexican Traditional (1891) | Folk |

---

## Technical Details

### Encoding Format: π π π π Notation

Each melody is stored as pairs of 8-bit values:

```
[note_index, duration]
[note_index, duration]
... until END marker (255, 0)
```

### Note Palette (8-bit)

The system uses an **8-bit note palette** for compact storage:

```
π₀  = 0    → Rest (silence)
π₁  = 1    → C  (261.63 Hz)
π₂  = 2    → C# (277.18 Hz)
π₃  = 3    → D  (293.66 Hz)
π₄  = 4    → E  (329.63 Hz)  ← "E" in Ode to Joy
π₅  = 5    → F  (349.23 Hz)
π₆  = 6    → F# (369.99 Hz)
π₇  = 7    → G  (392.00 Hz)
π₈  = 8    → G# (415.30 Hz)
π₉  = 9    → A  (440.00 Hz)  ← Standard tuning (A4)
π₁₀ = 10   → A# (466.16 Hz)
π₁₁ = 11   → B  (493.88 Hz)
π₁₂ = 12   → C  (523.25 Hz)  ← Octave higher
... continue for higher octaves
```

### Duration Codes

```
Duration | Type        | Relative Length
---------|-------------|----------------
   1     | 16th note   | ♬♬♬♬ (very fast)
   2     | 8th note    | ♬♬ (fast)
   4     | Quarter     | ♩ (standard beat)
   8     | Half note   | 𝅗𝅥 (long)
   16    | Whole note  | 𝅝 (very long)
```

### Example Encoding

**Ode to Joy opening (E-E-F-G):**

```cpp
constexpr std::array<uint8_t, 64> ODE_TO_JOY = {
    4, 4,   // E (quarter note)
    4, 4,   // E (quarter note)
    5, 4,   // F (quarter note)
    7, 4,   // G (quarter note)
    // ... more notes
    255, 0  // END marker
};
```

---

## Frequency Calculation

From note index to actual frequency using **equal temperament tuning**:

```cpp
f = f₀ × 2^(n/12)
```

Where:
- `f₀` = Base frequency (C4 = 261.63 Hz)
- `n` = Semitone offset from C4
- All frequencies can be calculated from 8-bit palette

---

## Implementation Files

### Core Files

1. **src/ClassicalMusic.h**
   - 5 public-domain compositions encoded
   - `getNoteFrequency()` - Convert index to Hz
   - `getMusicName()` - Get composition name

2. **src/GraphicsEngine.h / .cpp**
   - `playClassicalMusic(int musicType)` - Trigger playback
   - Reads from InputManager F1-F5 keys

3. **src/InputManager.h**
   - `keyF1` through `keyF5` fields
   - Function key detection implemented

---

## How to Listen

1. **Run the application**
   ```bash
   cd build
   ./3DEngine.exe
   ```

2. **Enter PLAYING state** (click "Jugar" in menu)

3. **Press F1-F5** to hear different compositions

4. **Check console output** to see:
   - Composition name
   - Each note being played (frequency in Hz)
   - Duration of each note

---

## Console Output Example

When pressing F1 (Ode to Joy):

```
============================================================
Now playing: 🎵 Ode to Joy - Beethoven
Classical composition in 8-bit chiptune encoding (π π π π)
============================================================

  Note: 329.63 Hz (4 ticks)
  Note: 329.63 Hz (4 ticks)
  Note: 349.23 Hz (4 ticks)
  Note: 392 Hz (4 ticks)
  ...
🎼 Piece complete - Gracias por escuchar!
```

---

## Why These Compositions?

### Ode to Joy (F1) ⭐ Most Popular
- From Beethoven's 9th Symphony
- Symbol of European Union (anthem)
- Set to lyrics: "Freude" (Joy) in multiple languages
- UNESCO Memory of the World

### Für Elise (F2) 🎹 Most Recognizable
- Beethoven's most famous piano piece
- Instantly recognizable opening arpeggio
- Used in countless media/ringtones
- Still played in piano competitions

### Eine kleine Nachtmusik (F3) 🎻 Most Elegant
- Mozart's iconic serenade
- Epitome of Classical perfection
- Often played at formal events
- Chamber orchestra classic

### Clair de Lune (F4) 💫 Most Artistic
- Debussy's Impressionist masterpiece
- Evokes moonlit reflections
- Demonstrates beautiful color/harmony
- Often featured in films/anime

### Marcha de Zacatecas (F5) 🌎 Most Patriotic
- Mexican national treasure
- Energetic march style
- Used in celebrations
- Bridge between Western and Latin music

---

## Copyright Status

| Composition | Composer | Composed | Status |
|-------------|----------|----------|--------|
| Ode to Joy | Beethoven | 1824 | **PUBLIC DOMAIN** ✅ |
| Für Elise | Beethoven | 1810 | **PUBLIC DOMAIN** ✅ |
| Eine kleine Nachtmusik | Mozart | 1787 | **PUBLIC DOMAIN** ✅ |
| Clair de Lune | Debussy | 1890 | **PUBLIC DOMAIN** ✅ |
| Marcha de Zacatecas | Mexican Traditional | 1891 | **PUBLIC DOMAIN** ✅ |

All >130-135 years old → Zero copyright, free to use forever! 🎉

---

## Future Enhancements

- [ ] Full synth sound generation (not just console output)
- [ ] Playable instrument selection (piano, violin, flute, etc.)
- [ ] MIDI file export
- [ ] Real-time sheet music display
- [ ] Tempo adjustment (120 BPM default)
- [ ] User-uploaded compositions

---

## Fun Facts

🎵 **Beethoven** was deaf when he composed Ode to Joy!

🎹 **Für Elise** was lost for 40 years after Beethoven's death.

🎻 **Eine kleine Nachtmusik** doesn't have a lost score - Mozart's original is preserved.

💫 **Clair de Lune** translates to "Moonlight" and is perfect for meditation.

🎺 **Marcha de Zacatecas** plays every day at the Mexican town of Zacatecas!

---

*Enjoy the timeless beauty of classical music - π π π π* 🎼
