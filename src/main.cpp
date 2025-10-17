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

std::string loadConfiguration() {
    auto& config = Config::getInstance();
    
    std::vector<std::string> configPaths = {
        "config/config.json",
        "../config/config.json", 
        "../../config/config.json",
        "./config.json"
    };
    
    bool configLoaded = false;
    for (const auto& path : configPaths) {
        if (fs::exists(path)) {
            config.loadFromFile(path);
            configLoaded = true;
            break;
        }
    }
    
    if (!configLoaded) {
        config.setString("database.connection_string", 
            "postgresql://dance_user:dance_password@localhost/dance_studio");
        config.setString("logging.level", "INFO");
        config.setString("logging.file_path", "logs/dance_studio.log");
    }
    
    return config.getString(
        "database.connection_string",
        "postgresql://dance_user:dance_password@localhost/dance_studio"
    );
}

void initializeLogging() {
    auto& config = Config::getInstance();
    auto& logger = Logger::getInstance();
    
    std::string logLevelStr = config.getString("logging.level", "INFO");
    std::string logFilePath = config.getString("logging.file_path", "logs/dance_studio.log");
    
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
        
        std::string connectionString = loadConfiguration();
        
        initializeLogging();
        
        auto& logger = Logger::getInstance();
        logger.info("Приложение запущено", "Main");
        
        TechUI techUI(connectionString);
        techUI.run();
        
        logger.info("Приложение завершено", "Main");
        std::cout << "👋 Завершение работы системы" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
}