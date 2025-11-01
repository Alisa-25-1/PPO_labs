#pragma once
#include <memory>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

class DatabaseHealthService {
private:
    static std::atomic<bool> isDatabaseHealthy_;
    static std::chrono::system_clock::time_point lastUnhealthyTime_;
    static std::mutex checkMutex_;
    static std::atomic<bool> monitoringActive_;
    static std::thread monitoringThread_;
    static std::condition_variable monitoringCondition_;
    static std::mutex monitoringMutex_;
    
    static const std::chrono::seconds RETRY_INTERVAL_;
    static const std::chrono::seconds MONITORING_INTERVAL_;

    static void monitoringLoop();

public:
    static void startMonitoring();
    static void stopMonitoring();
    static bool isDatabaseHealthy();
    static void markDatabaseHealthy();
    static void markDatabaseUnhealthy();
    static bool shouldRetryConnection();
    static void waitForRetry();
    static bool testConnection();
};