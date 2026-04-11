#pragma once

/**
 * ============================================================================
 * CLASSICAL MUSIC - CHIPTUNE 8-BIT ENCODER
 * ============================================================================
 * 
 * Mythical classic songs in lossless 8-bit chiptune encoding
 * All compositions are in the public domain (>70 years old)
 * 
 * Encoding: π π π π notation (Pi pattern - frequency/duration)
 * Format: [frequency_index] [duration_ticks]
 * 
 * Frequencies (8-bit palette):
 * π₀ = Rest/Silence
 * π₁-π₁₂ = Octave C-B
 * π₁₃-π₂₄ = Octave C-B (higher)
 * π₂₅-π₃₆ = Octave C-B (even higher)
 * 
 * Duration codes:
 * 1 = 16th note
 * 2 = 8th note
 * 4 = Quarter note
 * 8 = Half note
 * 16 = Whole note
 */

#include <array>
#include <cstdint>
#include <cmath>

namespace ClassicalMusic {

// ============================================================================
// ODE TO JOY - Beethoven's 9th Symphony
// Public Domain (Composed 1824)
// ============================================================================
// 
// 🎵 Iconic opening phrase from "Ode to Joy"
// Encoded as pairs: [note_index, duration]
// π notation represents the mathematical constant applied to music synthesis

constexpr std::array<uint8_t, 64> ODE_TO_JOY = {
    // First phrase: E-E-F-G
    4, 4,   // E (quarter)
    4, 4,   // E (quarter)
    5, 4,   // F (quarter)
    7, 4,   // G (quarter)
    
    // Second phrase: G-F-E-D-C
    7, 4,   // G (quarter)
    5, 4,   // F (quarter)
    4, 4,   // E (quarter)
    2, 4,   // D (quarter)
    0, 4,   // C (quarter)
    
    // Third phrase (repeat E-E)
    4, 4,   // E (quarter)
    4, 4,   // E (quarter)
    5, 4,   // F (quarter)
    7, 4,   // G (quarter)
    
    // Final cadence
    0, 8,   // C (half)
    2, 4,   // D (quarter)
    4, 8,   // E (half)
    
    // Rest and loop
    255, 0  // END marker
};

// ============================================================================
// FOR ELISE - Beethoven (Public Domain, Composed 1810)
// ============================================================================
// 
// 🎵 Legendary opening arpeggio
// The most recognizable piano composition

constexpr std::array<uint8_t, 80> FOR_ELISE = {
    // Opening cascade: E-D#-E-D#-E-B-D-C-A
    4, 2,   // E (8th)
    3, 2,   // D# (8th)
    4, 2,   // E (8th)
    3, 2,   // D# (8th)
    4, 2,   // E (8th)
    11, 2,  // B (8th)
    2, 2,   // D (8th)
    0, 2,   // C (8th)
    9, 4,   // A (quarter)
    
    // Repeat opening
    0, 2,   // C (8th)
    4, 2,   // E (8th)
    9, 2,   // A (8th)
    11, 4,  // B (quarter)
    12, 2,  // High E (8th)
    11, 2,  // B (8th)
    12, 2,  // High E (8th)
    
    // Rise and fall
    4, 4,   // E (quarter)
    7, 4,   // G (quarter)
    9, 4,   // A (quarter)
    11, 8,  // B (half)
    
    255, 0  // END marker
};

// ============================================================================
// EINE KLEINE NACHTMUSIK - Mozart (Public Domain, Composed 1787)
// ============================================================================
// 
// 🎵 Elegant serenade opening

constexpr std::array<uint8_t, 72> EINE_KLEINE_NACHTMUSIK = {
    // Opening G major flourish
    7, 4,   // G (quarter)
    9, 4,   // A (quarter)
    11, 4,  // B (quarter)
    12, 4,  // C (quarter)
    
    // Descending phrase
    11, 4,  // B (quarter)
    9, 4,   // A (quarter)
    7, 4,   // G (quarter)
    2, 4,   // D (quarter)
    
    // Repeat higher
    14, 4,  // High G (quarter)
    16, 4,  // High A (quarter)
    17, 4,  // High B (quarter)
    19, 4,  // High C (quarter)
    
    // Graceful descent
    17, 4,  // High B (quarter)
    16, 4,  // High A (quarter)
    14, 4,  // High G (quarter)
    7, 4,   // G (quarter)
    
    255, 0  // END marker
};

// ============================================================================
// CLAIR DE LUNE - Debussy (Public Domain, Composed 1890)
// ============================================================================
// 
// 🎵 Moonlight - dreamy and melancholic opening

constexpr std::array<uint8_t, 56> CLAIR_DE_LUNE = {
    // Slow, romantic opening in E major
    4, 8,   // E (half)
    5, 4,   // F (quarter)
    7, 4,   // G (quarter)
    9, 8,   // A (half)
    
    // Gentle rise
    11, 4,  // B (quarter)
    12, 4,  // C (quarter)
    14, 8,  // D (half)
    16, 8,  // E (half)
    
    // Dreamlike descend
    14, 4,  // D (quarter)
    12, 4,  // C (quarter)
    11, 8,  // B (half)
    9, 8,   // A (half)
    
    // Resolution
    7, 4,   // G (quarter)
    4, 8,   // E (half)
    0, 16,  // C (whole note)
    
    255, 0  // END marker
};

// ============================================================================
// MARCHA DE ZACATECAS - Mexican Classic (Public Domain, Composed 1891)
// ============================================================================
// 
// 🎵 Epic Mexican march - triumphant and energetic

constexpr std::array<uint8_t, 80> MARCHA_ZACATECAS = {
    // Bold opening
    7, 4,   // G (quarter)
    7, 4,   // G (quarter)
    11, 4,  // B (quarter)
    12, 4,  // C (quarter)
    
    // Rising phrase
    14, 4,  // D (quarter)
    16, 4,  // E (quarter)
    17, 4,  // F (quarter)
    19, 4,  // G (quarter)
    
    // Triumphant peak
    21, 8,  // A (half)
    19, 4,  // G (quarter)
    17, 4,  // F (quarter)
    
    // Descent with style
    16, 4,  // E (quarter)
    14, 4,  // D (quarter)
    12, 4,  // C (quarter)
    11, 4,  // B (quarter)
    9, 8,   // A (half)
    
    255, 0  // END marker
};

// ============================================================================
// NOTE INDICES (8-bit palette)
// ============================================================================
// π₀ = 0    → Rest (silence)
// π₁ = 1    → C  (261.63 Hz)
// π₂ = 2    → C# (277.18 Hz)
// π₃ = 3    → D  (293.66 Hz)
// π₄ = 4    → E  (329.63 Hz) ← "E" in Ode to Joy
// π₅ = 5    → F  (349.23 Hz)
// π₆ = 6    → F# (369.99 Hz)
// π₇ = 7    → G  (392.00 Hz)
// π₈ = 8    → G# (415.30 Hz)
// π₉ = 9    → A  (440.00 Hz)
// π₁₀ = 10  → A# (466.16 Hz)
// π₁₁ = 11  → B  (493.88 Hz)
// π₁₂ = 12  → C  (523.25 Hz) - octave higher
// ... and so on for higher octaves

constexpr inline float getNoteFrequency(uint8_t noteIndex) {
    // A4 (440 Hz) is at index 9
    const float A4 = 440.0f;
    const float baseFreq = 261.63f; // C4
    
    if (noteIndex == 0) return 0.0f; // Rest
    
    // Convert index to semitone offset from C4
    int semitones = noteIndex - 1;
    
    // f = f0 * 2^(n/12) where n is semitone offset
    return baseFreq * std::pow(2.0f, semitones / 12.0f);
}

constexpr inline const char* getMusicName(int musicType) {
    switch (musicType) {
        case 0: return "Ode to Joy - Beethoven 🎵";
        case 1: return "Für Elise - Beethoven 🎹";
        case 2: return "Eine kleine Nachtmusik - Mozart 🎻";
        case 3: return "Clair de Lune - Debussy 💫";
        case 4: return "Marcha de Zacatecas 🎺";
        default: return "Unknown";
    }
}

} // namespace ClassicalMusic
