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