#include "MongoDBRepositoryFactory.hpp"
#include <iostream>

// MongoDB repository implementations will be created here
// For now, we'll create stubs

mongocxx::instance& MongoDBRepositoryFactory::getInstance() {
    static mongocxx::instance instance{};
    return instance;
}

MongoDBRepositoryFactory::MongoDBRepositoryFactory(const std::string& connection_string, 
                                                 const std::string& database_name)
    : database_name_(database_name) {
    
    try {
        // Initialize MongoDB client
        client_ = std::make_shared<mongocxx::client>(mongocxx::uri(connection_string));
        
        // Test connection
        auto admin_db = client_->database("admin");
        auto result = admin_db.run_command(bsoncxx::builder::stream::document{} 
            << "ping" << 1 
            << bsoncxx::builder::stream::finalize);
        
        std::cout << "✅ MongoDB connection established successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB connection failed: " << e.what() << std::endl;
        throw;
    }
}

// Stub implementations for now - we'll create actual MongoDB repositories later
std::shared_ptr<IClientRepository> MongoDBRepositoryFactory::createClientRepository() {
    // TODO: Implement MongoDBClientRepository
    std::cout << "⚠️  MongoDBClientRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<IDanceHallRepository> MongoDBRepositoryFactory::createDanceHallRepository() {
    // TODO: Implement MongoDBDanceHallRepository
    std::cout << "⚠️  MongoDBDanceHallRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<IBookingRepository> MongoDBRepositoryFactory::createBookingRepository() {
    // TODO: Implement MongoDBBookingRepository
    std::cout << "⚠️  MongoDBBookingRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<ILessonRepository> MongoDBRepositoryFactory::createLessonRepository() {
    // TODO: Implement MongoDBLessonRepository
    std::cout << "⚠️  MongoDBLessonRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<ITrainerRepository> MongoDBRepositoryFactory::createTrainerRepository() {
    // TODO: Implement MongoDBTrainerRepository
    std::cout << "⚠️  MongoDBTrainerRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<IEnrollmentRepository> MongoDBRepositoryFactory::createEnrollmentRepository() {
    // TODO: Implement MongoDBEnrollmentRepository
    std::cout << "⚠️  MongoDBEnrollmentRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<ISubscriptionRepository> MongoDBRepositoryFactory::createSubscriptionRepository() {
    // TODO: Implement MongoDBSubscriptionRepository
    std::cout << "⚠️  MongoDBSubscriptionRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<ISubscriptionTypeRepository> MongoDBRepositoryFactory::createSubscriptionTypeRepository() {
    // TODO: Implement MongoDBSubscriptionTypeRepository
    std::cout << "⚠️  MongoDBSubscriptionTypeRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<IReviewRepository> MongoDBRepositoryFactory::createReviewRepository() {
    // TODO: Implement MongoDBReviewRepository
    std::cout << "⚠️  MongoDBReviewRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<IBranchRepository> MongoDBRepositoryFactory::createBranchRepository() {
    // TODO: Implement MongoDBBranchRepository
    std::cout << "⚠️  MongoDBBranchRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<IStudioRepository> MongoDBRepositoryFactory::createStudioRepository() {
    // TODO: Implement MongoDBStudioRepository
    std::cout << "⚠️  MongoDBStudioRepository not implemented yet" << std::endl;
    return nullptr;
}

std::shared_ptr<IAttendanceRepository> MongoDBRepositoryFactory::createAttendanceRepository() {
    // TODO: Implement MongoDBAttendanceRepository
    std::cout << "⚠️  MongoDBAttendanceRepository not implemented yet" << std::endl;
    return nullptr;
}

bool MongoDBRepositoryFactory::testConnection() const {
    try {
        auto admin_db = client_->database("admin");
        auto result = admin_db.run_command(bsoncxx::builder::stream::document{} 
            << "ping" << 1 
            << bsoncxx::builder::stream::finalize);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void MongoDBRepositoryFactory::reconnect() {
    // MongoDB client handles reconnection automatically
    // We can reset the client if needed
}

mongocxx::database MongoDBRepositoryFactory::getDatabase() const {
    return client_->database(database_name_);
}

mongocxx::client& MongoDBRepositoryFactory::getClient() const {
    return *client_;
}