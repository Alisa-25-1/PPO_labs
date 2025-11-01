#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include "DatabaseHealthService.hpp"

class DatabaseMonitorService {
private:
    std::atomic<bool> running_{false};
    std::thread monitorThread_;
    
public:
    void start();
    void stop();
    void monitorLoop();
};