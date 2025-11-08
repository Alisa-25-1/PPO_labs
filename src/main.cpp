#include <iostream>
#include <memory>
#include <filesystem>
#include <fstream>
#include "tech_ui/TechUI.hpp"
#include "core/Logger.hpp"
#include "core/Config.hpp"
#include "data/RepositoryFactoryCreator.hpp"
#include "data/DataMigrator.hpp"
#include "data/MongoDBGlobalInstance.hpp"  

namespace fs = std::filesystem;

const std::string LAST_DB_TYPE_FILE = "last_db.type";

void ensureDirectoriesExist() {
    fs::create_directories("logs");
    fs::create_directories("config");
    fs::create_directories("backups");
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
        config.setString("database.type", "postgres");
        config.setString("database.postgres.connection_string", 
            "postgresql://dance_user:dance_password@localhost/dance_studio");
        config.setString("database.mongodb.connection_string", "mongodb://localhost:27017");
        config.setString("database.mongodb.database_name", "dance_studio");
        config.setBool("database.auto_migrate", true);
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

std::string getLastDatabaseType() {
    std::ifstream file(LAST_DB_TYPE_FILE);
    std::string lastType;
    if (file.is_open()) {
        std::getline(file, lastType);
        file.close();
    }
    return lastType;
}

void setLastDatabaseType(const std::string& dbType) {
    std::ofstream file(LAST_DB_TYPE_FILE);
    if (file.is_open()) {
        file << dbType;
        file.close();
    }
}

std::shared_ptr<IRepositoryFactory> createFactoryFromType(const std::string& dbType, const Config& config) {
    if (dbType == "postgres") {
        std::string connectionString = config.getPostgresConnectionString();
        return std::make_shared<PostgreSQLRepositoryFactory>(connectionString);
    }
    else if (dbType == "mongodb") {
        std::string connectionString = config.getMongoConnectionString();
        std::string databaseName = config.getMongoDatabaseName();
        return std::make_shared<MongoDBRepositoryFactory>(connectionString, databaseName);
    }
    else {
        throw std::runtime_error("Unsupported database type: " + dbType);
    }
}

bool shouldMigrate(const Config& config) {
    std::string currentType = config.getDatabaseType();
    std::string lastType = getLastDatabaseType();
    
    // Если файл с последним типом не существует, то это первый запуск, миграция не нужна.
    if (lastType.empty()) {
        setLastDatabaseType(currentType);
        return false;
    }
    
    // Если тип базы не менялся, миграция не нужна.
    if (currentType == lastType) {
        return false;
    }
    
    // Проверяем, разрешена ли автоматическая миграция.
    return config.getBool("database.auto_migrate", true);
}

bool performMigration(const Config& config) {
    std::string currentType = config.getDatabaseType();
    std::string lastType = getLastDatabaseType();
    
    std::cout << "🔄 Миграция данных из " << lastType << " в " << currentType << std::endl;
    
    std::string migrationStrategy = config.getString("database.migration.strategy", "upsert");
    std::cout << "🔧 Стратегия миграции: " << migrationStrategy << std::endl;
    
    try {
        auto sourceFactory = createFactoryFromType(lastType, config);
        auto targetFactory = createFactoryFromType(currentType, config);
        
        DataMigrator migrator(sourceFactory, targetFactory, migrationStrategy);
        bool success = migrator.migrateAll();
        
        if (success) {
            setLastDatabaseType(currentType);
            std::cout << "✅ Миграция данных завершена успешно!" << std::endl;
        } else {
            std::cerr << "❌ Миграция данных завершена с ошибками!" << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Ошибка при миграции данных: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    std::cout << "🚀 Запуск системы управления танцевальной студией" << std::endl;
    
    try {
        ensureDirectoriesExist();
        loadConfiguration();
        initializeLogging();
        
        auto& logger = Logger::getInstance();
        auto& config = Config::getInstance();
        
        if (config.getDatabaseType() == "mongodb") {
            MongoDBGlobalInstance::initialize();
        }
        
        logger.info("Приложение запущено с БД: " + config.getDatabaseType(), "Main");
        
        if (shouldMigrate(config)) {
            std::cout << "🔄 Обнаружено изменение типа базы данных. Запуск миграции..." << std::endl;
            if (!performMigration(config)) {
                std::cerr << "💥 Миграция данных не удалась. Приложение будет остановлено." << std::endl;
                return 1;
            }
        }
        
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