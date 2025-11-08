#include "MongoDBGlobalInstance.hpp"

// Определяем статические члены в .cpp файле
std::unique_ptr<mongocxx::instance> MongoDBGlobalInstance::instance_ = nullptr;
std::mutex MongoDBGlobalInstance::mutex_;
bool MongoDBGlobalInstance::initialized_ = false;

void MongoDBGlobalInstance::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!initialized_) {
        instance_ = std::make_unique<mongocxx::instance>();
        initialized_ = true;
        std::cout << "✅ Global MongoDB instance initialized" << std::endl;
    }
}

bool MongoDBGlobalInstance::isInitialized() {
    return initialized_;
}