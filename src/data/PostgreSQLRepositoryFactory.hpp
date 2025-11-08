#ifndef POSTGRESQL_REPOSITORY_FACTORY_HPP
#define POSTGRESQL_REPOSITORY_FACTORY_HPP

#include <memory>
#include <string>
#include "IRepositoryFactory.hpp"

// Интерфейсы репозиториев
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../repositories/IDanceHallRepository.hpp"
#include "../repositories/IBranchRepository.hpp"
#include "../repositories/IStudioRepository.hpp"
#include "../repositories/ITrainerRepository.hpp"
#include "../repositories/ILessonRepository.hpp"
#include "../repositories/IEnrollmentRepository.hpp"
#include "../repositories/IReviewRepository.hpp"
#include "../repositories/ISubscriptionRepository.hpp"
#include "../repositories/ISubscriptionTypeRepository.hpp"
#include "../repositories/IAttendanceRepository.hpp"

// Forward declaration
class DatabaseConnection;

class PostgreSQLRepositoryFactory : public IRepositoryFactory {
private:
    std::shared_ptr<DatabaseConnection> dbConnection_;

public:
    explicit PostgreSQLRepositoryFactory(const std::string& connectionString);
    ~PostgreSQLRepositoryFactory() override = default;

    // Запрещаем копирование
    PostgreSQLRepositoryFactory(const PostgreSQLRepositoryFactory&) = delete;
    PostgreSQLRepositoryFactory& operator=(const PostgreSQLRepositoryFactory&) = delete;

    // Фабричные методы
    std::shared_ptr<IBookingRepository> createBookingRepository() override;
    std::shared_ptr<IClientRepository> createClientRepository() override;
    std::shared_ptr<IDanceHallRepository> createDanceHallRepository() override;
    std::shared_ptr<IBranchRepository> createBranchRepository() override;
    std::shared_ptr<IStudioRepository> createStudioRepository() override;
    std::shared_ptr<ITrainerRepository> createTrainerRepository() override;
    std::shared_ptr<ILessonRepository> createLessonRepository() override;
    std::shared_ptr<IEnrollmentRepository> createEnrollmentRepository() override;
    std::shared_ptr<IReviewRepository> createReviewRepository() override;
    std::shared_ptr<ISubscriptionRepository> createSubscriptionRepository() override;
    std::shared_ptr<ISubscriptionTypeRepository> createSubscriptionTypeRepository() override;
    std::shared_ptr<IAttendanceRepository> createAttendanceRepository() override;

    // Управление соединением 
    bool testConnection() const override;
    void reconnect() override;
    
    // Получение соединения 
    std::shared_ptr<DatabaseConnection> getConnection() const { return dbConnection_; }
};

#endif // POSTGRESQL_REPOSITORY_FACTORY_HPP