#include "Config.hpp"
#include <algorithm>
#include <cctype>

std::unique_ptr<Config> Config::instance_ = nullptr;

Config::Config() {}

Config& Config::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<Config>(new Config());
    }
    return *instance_;
}

void Config::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filePath);
    }
    
    configMap_.clear();
    configFilePath_ = filePath;
    
    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }
}

void Config::saveToFile(const std::string& filePath) {
    std::string savePath = filePath.empty() ? configFilePath_ : filePath;
    if (savePath.empty()) {
        throw std::runtime_error("No file path specified for saving config");
    }
    
    std::ofstream file(savePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file for writing: " + savePath);
    }
    
    for (const auto& [key, value] : configMap_) {
        file << key << " = " << value << std::endl;
    }
}

void Config::parseLine(const std::string& line) {
    std::string trimmed = trim(line);
    
    if (trimmed.empty() || trimmed[0] == '#') {
        return;
    }
    
    size_t pos = trimmed.find('=');
    if (pos == std::string::npos) {
        return;
    }
    
    std::string key = trim(trimmed.substr(0, pos));
    std::string value = trim(trimmed.substr(pos + 1));
    
    if (!key.empty()) {
        configMap_[key] = value;
    }
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = configMap_.find(key);
    return (it != configMap_.end()) ? it->second : defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto it = configMap_.find(key);
    if (it == configMap_.end()) return defaultValue;
    
    try {
        return std::stoi(it->second);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto it = configMap_.find(key);
    if (it == configMap_.end()) return defaultValue;
    
    try {
        return std::stod(it->second);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto it = configMap_.find(key);
    if (it == configMap_.end()) return defaultValue;
    
    std::string value = it->second;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    
    return (value == "true" || value == "1" || value == "yes" || value == "on");
}

void Config::setString(const std::string& key, const std::string& value) {
    configMap_[key] = value;
}

void Config::setInt(const std::string& key, int value) {
    configMap_[key] = std::to_string(value);
}

void Config::setDouble(const std::string& key, double value) {
    configMap_[key] = std::to_string(value);
}

void Config::setBool(const std::string& key, bool value) {
    configMap_[key] = value ? "true" : "false";
}

bool Config::hasKey(const std::string& key) const {
    return configMap_.find(key) != configMap_.end();
}

// Database configuration methods
std::string Config::getDatabaseType() const {
    return getString("database.type", "postgres");
}

std::string Config::getPostgresConnectionString() const {
    return getString("database.postgres.connection_string", 
        "postgresql://dance_user:dance_password@localhost:5432/dance_studio");
}

std::string Config::getMongoConnectionString() const {
    return getString("database.mongodb.connection_string", 
        "mongodb://localhost:27017");
}

std::string Config::getMongoDatabaseName() const {
    return getString("database.mongodb.database_name", "dance_studio");
}

int Config::getMaxConnections() const {
    if (getDatabaseType() == "postgres") {
        return getInt("database.postgres.max_connections", 10);
    } else {
        return getInt("database.mongodb.pool_size", 10);
    }
}

int Config::getConnectionTimeoutSeconds() const {
    if (getDatabaseType() == "postgres") {
        return getInt("database.postgres.connection_timeout_seconds", 30);
    } else {
        return getInt("database.mongodb.timeout_ms", 5000) / 1000;
    }
}

// Business logic configuration
int Config::getMaxBookingDaysAhead() const {
    return getInt("business_logic.max_booking_days_ahead", 30);
}

int Config::getLessonCancellationHours() const {
    return getInt("business_logic.lesson_cancellation_hours", 24);
}

int Config::getMaxParticipantsPerLesson() const {
    return getInt("business_logic.max_participants_per_lesson", 50);
}

int Config::getDefaultLessonDurationMinutes() const {
    return getInt("business_logic.default_lesson_duration_minutes", 60);
}

// Logging configuration
std::string Config::getLogLevel() const {
    return getString("logging.level", "INFO");
}

std::string Config::getLogFilePath() const {
    return getString("logging.file_path", "logs/dance_studio.log");
}

int Config::getMaxLogFileSizeMB() const {
    return getInt("logging.max_file_size_mb", 10);
}

int Config::getLogBackupCount() const {
    return getInt("logging.backup_count", 5);
}

// Application configuration
std::string Config::getApplicationName() const {
    return getString("application.name", "Dance Studio Management System");
}

std::string Config::getApplicationVersion() const {
    return getString("application.version", "1.0.0");
}

bool Config::isTechUIEnabled() const {
    return getBool("application.tech_ui_enabled", true);
}