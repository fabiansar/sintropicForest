#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

/**
 * ============================================================================
 * SISTEMA DE LOGGING SIMPLE
 * ============================================================================
 * 
 * Sistema de logging para debugging y profiling.
 * 
 * PROPÓSITO:
 * Reemplazar std::cout con un sistema centralizado que pueda:
 * - Imprimir a consola
 * - Guardar a archivo
 * - Filtrar por nivel (INFO, WARNING, ERROR, DEBUG)
 * - Agregar timestamps
 * 
 * EJEMPLO DE USO FUTURO:
 * 
 * Logger::init("game.log");  // Inicializar, guardar en file
 * 
 * Logger::info("Game started");              // [INFO] Game started
 * Logger::warning("Plant count high", 500); // [WARNING] Plant count high: 500
 * Logger::error("Shader failed");            // [ERROR] Shader failed
 * Logger::debug("Plant at", x, y, z);       // [DEBUG] Plant at 10.5, 2, 20.3
 */

class Logger {
public:
    /** Niveles de logging */
    enum class Level {
        DEBUG = 0,      // Información de debugging
        INFO = 1,       // Información general
        WARNING = 2,    // Advertencia (algo inusual)
        ERROR = 3       // Error (algo rompió)
    };

private:
    static std::ofstream* logFile;
    static Level currentLevel;
    static bool useTimestamp;

public:
    /**
     * Inicializar logger
     * @param filename Archivo donde guardar logs
     * @param minLevel Nivel mínimo a mostrar
     * @param useTime Agregar timestamp a cada log?
     */
    static void init(const std::string* filename = nullptr, 
                     Level minLevel = Level::INFO,
                     bool useTime = true) {
        currentLevel = minLevel;
        useTimestamp = useTime;
        
        if (filename) {
            logFile = new std::ofstream(*filename, std::ios::app);
        }
    }

    /** Limpiar recursos */
    static void shutdown() {
        if (logFile) {
            logFile->close();
            delete logFile;
            logFile = nullptr;
        }
    }

    // Métodos de logging
    static void info(const std::string& message) { log(Level::INFO, message); }
    static void warning(const std::string& message) { log(Level::WARNING, message); }
    static void error(const std::string& message) { log(Level::ERROR, message); }
    static void debug(const std::string& message) { log(Level::DEBUG, message); }

private:
    static void log(Level level, const std::string& message) {
        if (static_cast<int>(level) < static_cast<int>(currentLevel)) {
            return;
        }

        std::string prefix = "[" + levelToString(level) + "] ";
        if (useTimestamp) {
            prefix = getTimestamp() + " " + prefix;
        }

        std::string output = prefix + message;

        std::cout << output << std::endl;

        if (logFile && logFile->is_open()) {
            *logFile << output << std::endl;
            logFile->flush();
        }
    }

    static std::string levelToString(Level level) {
        switch (level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR:   return "ERROR";
            default:             return "UNKNOWN";
        }
    }

    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        return ss.str();
    }
};

// Static member initialization
inline std::ofstream* Logger::logFile = nullptr;
inline Logger::Level Logger::currentLevel = Logger::Level::INFO;
inline bool Logger::useTimestamp = true;