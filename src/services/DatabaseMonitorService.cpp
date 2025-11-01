#include "DatabaseMonitorService.hpp"
#include "../data/DatabaseConnection.hpp"
#include <iostream>

void DatabaseMonitorService::start() {
    running_ = true;
    monitorThread_ = std::thread(&DatabaseMonitorService::monitorLoop, this);
    std::cout << "🔍 Database monitor started" << std::endl;
}

void DatabaseMonitorService::stop() {
    running_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
    std::cout << "🔍 Database monitor stopped" << std::endl;
}

void DatabaseMonitorService::monitorLoop() {
    while (running_) {
        try {
            // Пытаемся подключиться к БД
            auto connectionString = "postgresql://dance_user:dance_password@localhost/dance_studio";
            DatabaseConnection testConn(connectionString);
            auto work = testConn.beginTransaction();
            work.exec("SELECT 1");
            testConn.commitTransaction(work);
            
            DatabaseHealthService::markDatabaseHealthy();
            std::cout << "✅ Database health check passed" << std::endl;
            
        } catch (const std::exception& e) {
            DatabaseHealthService::markDatabaseUnhealthy();
            std::cerr << "❌ Database health check failed: " << e.what() << std::endl;
        }
        
        // Ждем 30 секунд до следующей проверки
        for (int i = 0; i < 30 && running_; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}