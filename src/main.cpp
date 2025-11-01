#include <iostream>
#include <memory>
#include <filesystem>
#include "tech_ui/TechUI.hpp"
#include "core/Logger.hpp"
#include "core/Config.hpp"

namespace fs = std::filesystem;

void ensureDirectoriesExist() {
    fs::create_directories("logs");
    fs::create_directories("config");
}

void loadConfiguration() {
    auto& config = Config::getInstance();
    
    std::vector<std::string> configPaths = {
        "config/config.properties",  
        "../config/config.properties", 
        "../../config/config.properties",
        "./config.properties"
    };
    
    bool configLoaded = false;
    for (const auto& path : configPaths) {
        if (fs::exists(path)) {
            config.loadFromFile(path);
            configLoaded = true;
            std::cout << "✅ Конфигурация загружена из: " << path << std::endl;
            std::cout << "🔧 Тип БД: " << config.getDatabaseType() << std::endl;
            break;
        }
    }
    
    if (!configLoaded) {
        std::cout << "⚠️  Конфигурационный файл не найден, используются значения по умолчанию" << std::endl;
        // Устанавливаем значения по умолчанию
        config.setString("database.type", "postgres");
        config.setString("database.postgres.connection_string", 
            "postgresql://dance_user:dance_password@localhost/dance_studio");
        config.setString("logging.level", "INFO");
        config.setString("logging.file_path", "logs/dance_studio.log");
    }
}

void initializeLogging() {
    auto& config = Config::getInstance();
    auto& logger = Logger::getInstance();
    
    std::string logLevelStr = config.getLogLevel();
    std::string logFilePath = config.getLogFilePath();
    
    LogLevel logLevel = LogLevel::INFO;
    if (logLevelStr == "DEBUG") logLevel = LogLevel::DEBUG;
    else if (logLevelStr == "WARNING") logLevel = LogLevel::WARNING;
    else if (logLevelStr == "ERROR") logLevel = LogLevel::ERROR;
    
    logger.initialize(logFilePath, logLevel);
}

int main() {
    std::cout << "🚀 Запуск системы управления танцевальной студией" << std::endl;
    
    try {
        ensureDirectoriesExist();
        loadConfiguration();
        initializeLogging();
        
        auto& logger = Logger::getInstance();
        auto& config = Config::getInstance();
        
        logger.info("Приложение запущено с БД: " + config.getDatabaseType(), "Main");
        
        // Теперь передаем Config в TechUI
        TechUI techUI(config);
        techUI.run();
        
        logger.info("Приложение завершено", "Main");
        std::cout << "👋 Завершение работы системы" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
}