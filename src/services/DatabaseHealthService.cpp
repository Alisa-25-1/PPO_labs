#include "DatabaseHealthService.hpp"
#include <iostream>
#include <pqxx/pqxx>

std::atomic<bool> DatabaseHealthService::isDatabaseHealthy_{true};
std::chrono::system_clock::time_point DatabaseHealthService::lastUnhealthyTime_;
std::mutex DatabaseHealthService::checkMutex_;
std::atomic<bool> DatabaseHealthService::monitoringActive_{false};
std::thread DatabaseHealthService::monitoringThread_;
std::condition_variable DatabaseHealthService::monitoringCondition_;
std::mutex DatabaseHealthService::monitoringMutex_;

const std::chrono::seconds DatabaseHealthService::RETRY_INTERVAL_{30};
const std::chrono::seconds DatabaseHealthService::MONITORING_INTERVAL_{10};

void DatabaseHealthService::startMonitoring() {
    if (monitoringActive_.exchange(true)) {
        return; // Уже запущено
    }
    
    monitoringThread_ = std::thread(&DatabaseHealthService::monitoringLoop);
    std::cout << "🔍 Database health monitoring started" << std::endl;
}

void DatabaseHealthService::stopMonitoring() {
    monitoringActive_ = false;
    monitoringCondition_.notify_all();
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    std::cout << "🔍 Database health monitoring stopped" << std::endl;
}

void DatabaseHealthService::monitoringLoop() {
    while (monitoringActive_) {
        {
            std::unique_lock<std::mutex> lock(monitoringMutex_);
            monitoringCondition_.wait_for(lock, MONITORING_INTERVAL_, []() {
                return !monitoringActive_;
            });
        }
        
        if (!monitoringActive_) {
            break;
        }
        
        // Если база помечена как нездоровая, проверяем ее состояние
        if (!isDatabaseHealthy_) {
            if (testConnection()) {
                std::cout << "✅ Database is back online! Marking as healthy." << std::endl;
                markDatabaseHealthy();
            } else {
                auto now = std::chrono::system_clock::now();
                auto timeSinceUnhealthy = now - lastUnhealthyTime_;
                auto secondsSinceUnhealthy = std::chrono::duration_cast<std::chrono::seconds>(timeSinceUnhealthy).count();
                
                std::cout << "🔍 Database still unavailable (" << secondsSinceUnhealthy << "s since failure)" << std::endl;
            }
        }
    }
}

bool DatabaseHealthService::testConnection() {
    try {
        auto connectionString = "postgresql://dance_user:dance_password@localhost/dance_studio";
        pqxx::connection testConn(connectionString);
        
        if (testConn.is_open()) {
            pqxx::work testWork(testConn);
            testWork.exec("SELECT 1");
            testWork.commit();
            return true;
        }
    } catch (const std::exception& e) {
        // Соединение все еще недоступно
    }
    return false;
}

bool DatabaseHealthService::isDatabaseHealthy() {
    std::lock_guard<std::mutex> lock(checkMutex_);
    
    // Если база нездорова и прошло больше RETRY_INTERVAL_, разрешаем ретрай
    if (!isDatabaseHealthy_) {
        auto now = std::chrono::system_clock::now();
        if (now - lastUnhealthyTime_ > RETRY_INTERVAL_) {
            // Проверяем, доступна ли база сейчас
            if (testConnection()) {
                markDatabaseHealthy();
            } else {
                // Обновляем время, чтобы не проверять слишком часто
                lastUnhealthyTime_ = now - RETRY_INTERVAL_ + std::chrono::seconds(5);
            }
        }
    }
    
    return isDatabaseHealthy_;
}

void DatabaseHealthService::markDatabaseHealthy() {
    std::lock_guard<std::mutex> lock(checkMutex_);
    if (!isDatabaseHealthy_) {
        isDatabaseHealthy_ = true;
        std::cout << "✅ Database marked as healthy" << std::endl;
    }
}

void DatabaseHealthService::markDatabaseUnhealthy() {
    std::lock_guard<std::mutex> lock(checkMutex_);
    if (isDatabaseHealthy_) {
        isDatabaseHealthy_ = false;
        lastUnhealthyTime_ = std::chrono::system_clock::now();
        std::cout << "🚨 Database marked as unhealthy" << std::endl;
    }
}

bool DatabaseHealthService::shouldRetryConnection() {
    return isDatabaseHealthy();
}

void DatabaseHealthService::waitForRetry() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
}