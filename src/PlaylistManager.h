#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <iostream>
#include "ClassicalMusic.h"

#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H

/**
 * ============================================================================
 * PLAYLIST MANAGER
 * ============================================================================
 * 
 * Gestiona reproducción de múltiples canciones clásicas
 * - Compilación de todas las canciones disponibles
 * - Control de reproducción (play, pause, next, previous)
 * - Selección de canción
 * - Emparejamiento nota-frecuencia
 */

class PlaylistManager {
public:
    struct PlaylistEntry {
        const char* name;
        const char* composer;
        const char* emoji;
        const uint8_t* melody;
        int melodySize;
        int musicType;
    };

private:
    static constexpr int MAX_SONGS = 5;
    
    PlaylistEntry songs[MAX_SONGS] = {
        {
            "Ode to Joy",
            "Ludwig van Beethoven (1824)",
            "🎵",
            ClassicalMusic::ODE_TO_JOY.data(),
            ClassicalMusic::ODE_TO_JOY.size(),
            0
        },
        {
            "Für Elise",
            "Ludwig van Beethoven (1810)",
            "🎹",
            ClassicalMusic::FOR_ELISE.data(),
            ClassicalMusic::FOR_ELISE.size(),
            1
        },
        {
            "Eine kleine Nachtmusik",
            "Wolfgang Amadeus Mozart (1787)",
            "🎻",
            ClassicalMusic::EINE_KLEINE_NACHTMUSIK.data(),
            ClassicalMusic::EINE_KLEINE_NACHTMUSIK.size(),
            2
        },
        {
            "Clair de Lune",
            "Claude Debussy (1890)",
            "💫",
            ClassicalMusic::CLAIR_DE_LUNE.data(),
            ClassicalMusic::CLAIR_DE_LUNE.size(),
            3
        },
        {
            "Marcha de Zacatecas",
            "Genaro Codina (1891)",
            "🎺",
            ClassicalMusic::MARCHA_ZACATECAS.data(),
            ClassicalMusic::MARCHA_ZACATECAS.size(),
            4
        }
    };
    
    int currentSongIndex = 0;
    bool isPlaying = false;
    float playbackPosition = 0.0f;  // En segundos
    bool shuffleMode = false;
    bool repeatMode = false;

public:
    PlaylistManager() = default;
    
    int getTotalSongs() const {
        return MAX_SONGS;
    }
    
    const PlaylistEntry& getSong(int index) const {
        return songs[index % MAX_SONGS];
    }
    
    const PlaylistEntry& getCurrentSong() const {
        return songs[currentSongIndex];
    }
    
    int getCurrentSongIndex() const {
        return currentSongIndex;
    }
    
    void setSongIndex(int index) {
        currentSongIndex = index % MAX_SONGS;
        playbackPosition = 0.0f;
    }
    
    void playNext() {
        currentSongIndex = (currentSongIndex + 1) % MAX_SONGS;
        playbackPosition = 0.0f;
        isPlaying = true;
    }
    
    void playPrevious() {
        currentSongIndex = (currentSongIndex - 1 + MAX_SONGS) % MAX_SONGS;
        playbackPosition = 0.0f;
        isPlaying = true;
    }
    
    void play() {
        isPlaying = true;
        playbackPosition = 0.0f;
    }
    
    void pause() {
        isPlaying = false;
    }
    
    bool getIsPlaying() const {
        return isPlaying;
    }
    
    void setShuffleMode(bool shuffle) {
        shuffleMode = shuffle;
    }
    
    void setRepeatMode(bool repeat) {
        repeatMode = repeat;
    }
    
    bool getShuffleMode() const {
        return shuffleMode;
    }
    
    bool getRepeatMode() const {
        return repeatMode;
    }
    
    // Debug: Imprimir lista completa
    void printPlaylist() const {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "📻 PLAYLIST - " << MAX_SONGS << " Compositions" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        for (int i = 0; i < MAX_SONGS; ++i) {
            const auto& song = songs[i];
            std::cout << (i == currentSongIndex ? "► " : "  ");
            std::cout << (i + 1) << ". " << song.emoji << " " << song.name << std::endl;
            std::cout << "   by " << song.composer << std::endl;
        }
        
        std::cout << std::string(60, '=') << "\n" << std::endl;
    }
};

#endif  // PLAYLIST_MANAGER_H
