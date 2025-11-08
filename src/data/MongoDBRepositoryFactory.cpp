#include "MongoDBRepositoryFactory.hpp"
#include "../repositories/impl/MongoDBClientRepository.hpp"
#include "../repositories/impl/MongoDBDanceHallRepository.hpp"
#include "../repositories/impl/MongoDBBookingRepository.hpp"
#include "../repositories/impl/MongoDBBranchRepository.hpp"
#include "../repositories/impl/MongoDBStudioRepository.hpp"
#include "../repositories/impl/MongoDBTrainerRepository.hpp"
#include "../repositories/impl/MongoDBSubscriptionTypeRepository.hpp"
#include "../repositories/impl/MongoDBSubscriptionRepository.hpp"
#include "../repositories/impl/MongoDBLessonRepository.hpp"
#include "../repositories/impl/MongoDBEnrollmentRepository.hpp"
#include "../repositories/impl/MongoDBReviewRepository.hpp"
#include "../repositories/impl/MongoDBAttendanceRepository.hpp"
#include <iostream>

// Инициализация статических членов
std::unique_ptr<mongocxx::instance> MongoDBRepositoryFactory::instance_ = nullptr;
std::mutex MongoDBRepositoryFactory::instance_mutex_;
int MongoDBRepositoryFactory::instance_count_ = 0;

void MongoDBRepositoryFactory::initializeInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    
    if (instance_count_ == 0) {
        // Первое использование - создаем instance
        instance_ = std::make_unique<mongocxx::instance>();
        std::cout << "✅ MongoDB instance created" << std::endl;
    }
    
    instance_count_++;
    std::cout << "🔧 MongoDB instance count: " << instance_count_ << std::endl;
}

void MongoDBRepositoryFactory::cleanupInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    
    instance_count_--;
    std::cout << "🔧 MongoDB instance count: " << instance_count_ << std::endl;
    
    if (instance_count_ == 0) {
        // Последнее использование - уничтожаем instance
        instance_.reset();
        std::cout << "✅ MongoDB instance destroyed" << std::endl;
    }
}

MongoDBRepositoryFactory::MongoDBRepositoryFactory(const std::string& connection_string, 
                                                 const std::string& database_name)
    : database_name_(database_name) {
    
    try {
        // Инициализируем instance (с подсчетом ссылок)
        initializeInstance();
        
        // Initialize MongoDB client
        client_ = std::make_shared<mongocxx::client>(mongocxx::uri(connection_string));
        
        // Test connection
        auto admin_db = client_->database("admin");
        auto result = admin_db.run_command(bsoncxx::builder::stream::document{} 
            << "ping" << 1 
            << bsoncxx::builder::stream::finalize);
        
        std::cout << "✅ MongoDB connection established successfully to database: " 
                  << database_name_ << std::endl;
        
    } catch (const std::exception& e) {
        // В случае ошибки очищаем instance
        cleanupInstance();
        std::cerr << "❌ MongoDB connection failed: " << e.what() << std::endl;
        throw;
    }
}

MongoDBRepositoryFactory::~MongoDBRepositoryFactory() {
    // Явно уничтожаем клиент перед instance
    client_.reset();
    // Очищаем instance (с подсчетом ссылок)
    cleanupInstance();
}

std::shared_ptr<IClientRepository> MongoDBRepositoryFactory::createClientRepository() {
    return std::make_shared<MongoDBClientRepository>(shared_from_this());
}

std::shared_ptr<IDanceHallRepository> MongoDBRepositoryFactory::createDanceHallRepository() {
    return std::make_shared<MongoDBDanceHallRepository>(shared_from_this());
}

std::shared_ptr<IBookingRepository> MongoDBRepositoryFactory::createBookingRepository() {
    return std::make_shared<MongoDBBookingRepository>(shared_from_this());
}

std::shared_ptr<IBranchRepository> MongoDBRepositoryFactory::createBranchRepository() {
    return std::make_shared<MongoDBBranchRepository>(shared_from_this());
}

std::shared_ptr<ILessonRepository> MongoDBRepositoryFactory::createLessonRepository() {
    return std::make_shared<MongoDBLessonRepository>(shared_from_this());
}

std::shared_ptr<ITrainerRepository> MongoDBRepositoryFactory::createTrainerRepository() {
    return std::make_shared<MongoDBTrainerRepository>(shared_from_this());
}

std::shared_ptr<IEnrollmentRepository> MongoDBRepositoryFactory::createEnrollmentRepository() {
    return std::make_shared<MongoDBEnrollmentRepository>(shared_from_this());
}

std::shared_ptr<ISubscriptionRepository> MongoDBRepositoryFactory::createSubscriptionRepository() {
    return std::make_shared<MongoDBSubscriptionRepository>(shared_from_this());
}

std::shared_ptr<ISubscriptionTypeRepository> MongoDBRepositoryFactory::createSubscriptionTypeRepository() {
    return std::make_shared<MongoDBSubscriptionTypeRepository>(shared_from_this());
}

std::shared_ptr<IReviewRepository> MongoDBRepositoryFactory::createReviewRepository() {
    return std::make_shared<MongoDBReviewRepository>(shared_from_this());
}

std::shared_ptr<IStudioRepository> MongoDBRepositoryFactory::createStudioRepository() {
    return std::make_shared<MongoDBStudioRepository>(shared_from_this());
}

std::shared_ptr<IAttendanceRepository> MongoDBRepositoryFactory::createAttendanceRepository() {
    return std::make_shared<MongoDBAttendanceRepository>(shared_from_this());
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
    if (client_) {
        // Client автоматически переподключается при необходимости
    }
}

mongocxx::database MongoDBRepositoryFactory::getDatabase() const {
    return client_->database(database_name_);
}

mongocxx::client& MongoDBRepositoryFactory::getClient() const {
    return *client_;
}