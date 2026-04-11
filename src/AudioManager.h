#pragma once

#include <windows.h>
#include <thread>
#include <atomic>
#include <string>

/**
 * ============================================================================
 * AUDIO MANAGER
 * ============================================================================
 * 
 * Gestiona sonidos del juego usando Windows Beep API.
 * Reproduce tonos en hilos separados para no bloquear el game loop.
 */

class AudioManager {
public:
    enum SoundEffect {
        SOUND_PLACE_PLANT,
        SOUND_DELETE_PLANT,
        SOUND_UI_CLICK,
        SOUND_STATE_TRANSITION
    };

private:
    float masterVolume = 0.8f;
    std::atomic<bool> soundPlaying{false};

    /**
     * Reproducir tono en hilo separado (no bloqueante)
     */
    void playToneAsync(DWORD frequency, DWORD durationMs) {
        if (masterVolume <= 0.0f) return;
        
        std::thread([frequency, durationMs]() {
            Beep(frequency, durationMs);
        }).detach();
    }

    /**
     * Reproducir secuencia de tonos en hilo separado
     */
    void playSequenceAsync(DWORD freq1, DWORD dur1, DWORD freq2, DWORD dur2) {
        if (masterVolume <= 0.0f) return;
        
        std::thread([freq1, dur1, freq2, dur2]() {
            Beep(freq1, dur1);
            Beep(freq2, dur2);
        }).detach();
    }

public:
    AudioManager() = default;

    void playSound(SoundEffect sound) {
        if (masterVolume <= 0.0f) return;
        
        switch (sound) {
            case SOUND_PLACE_PLANT:
                playSequenceAsync(400, 80, 600, 80);
                break;
                
            case SOUND_DELETE_PLANT:
                playSequenceAsync(500, 80, 300, 100);
                break;
                
            case SOUND_UI_CLICK:
                playToneAsync(800, 50);
                break;
                
            case SOUND_STATE_TRANSITION:
                playSequenceAsync(600, 100, 800, 120);
                break;
        }
    }

    void setMasterVolume(float volume) {
        masterVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f ? 1.0f : volume);
    }

    float getMasterVolume() const {
        return masterVolume;
    }

    void setMuted(bool muted) {
        masterVolume = muted ? 0.0f : 0.8f;
    }

    bool isMuted() const {
        return masterVolume <= 0.0f;
    }
};
