#ifndef MONGODB_REPOSITORY_FACTORY_HPP
#define MONGODB_REPOSITORY_FACTORY_HPP

#include <memory>
#include <string>
#include "IRepositoryFactory.hpp"

// MongoDB includes
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

class MongoDBRepositoryFactory : public IRepositoryFactory {
private:
    std::shared_ptr<mongocxx::client> client_;
    std::string database_name_;
    
    // Ensure MongoDB instance is initialized
    static mongocxx::instance& getInstance();

public:
    explicit MongoDBRepositoryFactory(const std::string& connection_string, 
                                    const std::string& database_name);
    ~MongoDBRepositoryFactory() = default;

    // Фабричные методы
    std::shared_ptr<IClientRepository> createClientRepository() override;
    std::shared_ptr<IDanceHallRepository> createDanceHallRepository() override;
    std::shared_ptr<IBookingRepository> createBookingRepository() override;
    std::shared_ptr<ILessonRepository> createLessonRepository() override;
    std::shared_ptr<ITrainerRepository> createTrainerRepository() override;
    std::shared_ptr<IEnrollmentRepository> createEnrollmentRepository() override;
    std::shared_ptr<ISubscriptionRepository> createSubscriptionRepository() override;
    std::shared_ptr<ISubscriptionTypeRepository> createSubscriptionTypeRepository() override;
    std::shared_ptr<IReviewRepository> createReviewRepository() override;
    std::shared_ptr<IBranchRepository> createBranchRepository() override;
    std::shared_ptr<IStudioRepository> createStudioRepository() override;
    std::shared_ptr<IAttendanceRepository> createAttendanceRepository() override;

    // Управление соединением
    bool testConnection() const override;
    void reconnect() override;
    
    // Получение MongoDB-specific объектов
    mongocxx::database getDatabase() const;
    mongocxx::client& getClient() const;
};

#endif // MONGODB_REPOSITORY_FACTORY_HPP