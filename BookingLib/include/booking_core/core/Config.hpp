#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>

class Config {
private:
    static std::unique_ptr<Config> instance_;
    std::unordered_map<std::string, std::string> configMap_;
    std::string configFilePath_;
    
    Config();
    void parseLine(const std::string& line);
    std::string trim(const std::string& str);
    
public:
    static Config& getInstance();
    void loadFromFile(const std::string& filePath);
    void saveToFile(const std::string& filePath = "");
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setDouble(const std::string& key, double value);
    void setBool(const std::string& key, bool value);
    
    bool hasKey(const std::string& key) const;
    std::string getConfigFilePath() const { return configFilePath_; }
};