#ifndef MONGODB_GLOBAL_INSTANCE_HPP
#define MONGODB_GLOBAL_INSTANCE_HPP

#include <mongocxx/instance.hpp>
#include <memory>
#include <mutex>
#include <iostream>

class MongoDBGlobalInstance {
private:
    static std::unique_ptr<mongocxx::instance> instance_;
    static std::mutex mutex_;
    static bool initialized_;

public:
    static void initialize();
    static bool isInitialized();
    
};

#endif // MONGODB_GLOBAL_INSTANCE_HPP