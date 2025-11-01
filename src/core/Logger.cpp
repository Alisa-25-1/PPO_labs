#include "Logger.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>

std::unique_ptr<Logger> Logger::instance_ = nullptr;

Logger::Logger() : currentLevel_(LogLevel::INFO) {
}

Logger::~Logger() {
    shutdown();
}

Logger& Logger::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<Logger>(new Logger());
    }
    return *instance_;
}

void Logger::initialize(const std::string& filePath, LogLevel level) { 
    std::lock_guard<std::mutex> lock(logMutex_);
    
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0) {
        if (access(filePath.c_str(), W_OK) != 0) {
            throw std::runtime_error("No write permissions for log file: " + filePath);
        }
    } else {
        std::string dirPath = filePath.substr(0, filePath.find_last_of('/'));
        if (access(dirPath.c_str(), W_OK) != 0) {
            throw std::runtime_error("No write permissions for log directory: " + dirPath);
        }
    }
    
    if (logFile_.is_open()) {
        logFile_.close();
    }
    
    logFile_.open(filePath, std::ios::app);
    
    if (!logFile_.is_open()) {
        throw std::runtime_error("Cannot open log file: " + filePath);
    }
    
    logFile_ << "[INIT] Logger initialized successfully" << std::endl;
    if (logFile_.fail()) {
        logFile_.close();
        throw std::runtime_error("Cannot write to log file: " + filePath);
    }
    
    logFilePath_ = filePath;
    currentLevel_ = level;
    
    logFile_.flush();
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (logFile_.is_open()) {
        logFile_ << "[SHUTDOWN] Logger shutting down" << std::endl;
        logFile_.close();
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    try {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        
        std::tm tm_buf;
        std::tm* tm = std::localtime(&time_t);
        if (!tm) {
            return "0000-00-00 00:00:00.000";
        }
        tm_buf = *tm; // Копируем для безопасности
        
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        return oss.str();
    } catch (const std::exception& e) {
        
        return "0000-00-00 00:00:00.000";
    }
}

void Logger::log(LogLevel level, const std::string& message, const std::string& module) {
    if (level < currentLevel_) return;
    
    std::lock_guard<std::mutex> lock(logMutex_);
    
    try {
        std::ostringstream logEntry;
        logEntry << "[" << getCurrentTimestamp() << "] "
                 << "[" << levelToString(level) << "] ";
        
        if (!module.empty()) {
            logEntry << "[" << module << "] ";
        }
        
        logEntry << message;
        
       
        if (logFile_.is_open()) {
            logFile_ << logEntry.str() << std::endl;
            logFile_.flush();
        }
        
        if (level >= LogLevel::WARNING) {
            std::cerr << logEntry.str() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[LOGGER ERROR] " << e.what() << std::endl;
    }
}

void Logger::debug(const std::string& message, const std::string& module) {
    log(LogLevel::DEBUG, message, module);
}

void Logger::info(const std::string& message, const std::string& module) {
    log(LogLevel::INFO, message, module);
}

void Logger::warning(const std::string& message, const std::string& module) {
    log(LogLevel::WARNING, message, module);
}

void Logger::error(const std::string& message, const std::string& module) {
    log(LogLevel::ERROR, message, module);
}

void Logger::error(const std::exception& e, const std::string& module) {
    log(LogLevel::ERROR, std::string("Exception: ") + e.what(), module);
}