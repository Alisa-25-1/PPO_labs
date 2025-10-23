#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <iostream>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

class Logger {
private:
    static std::unique_ptr<Logger> instance_;
    std::ofstream logFile_;
    std::mutex logMutex_;
    LogLevel currentLevel_;
    std::string logFilePath_;
    
    Logger();
    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();
    
public:
    ~Logger();
    
    static Logger& getInstance();
    void initialize(const std::string& filePath, LogLevel level);
    void log(LogLevel level, const std::string& message, const std::string& module = "");
    void shutdown();
    
    void debug(const std::string& message, const std::string& module = "");
    void info(const std::string& message, const std::string& module = "");
    void warning(const std::string& message, const std::string& module = "");
    void error(const std::string& message, const std::string& module = "");
    void error(const std::exception& e, const std::string& module = "");
    
    LogLevel getLogLevel() const { return currentLevel_; }
    std::string getLogFilePath() const { return logFilePath_; }
    bool isInitialized() const { return logFile_.is_open(); }
};