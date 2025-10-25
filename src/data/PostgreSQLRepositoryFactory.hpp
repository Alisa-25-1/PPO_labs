#ifndef POSTGRESQL_REPOSITORY_FACTORY_HPP
#define POSTGRESQL_REPOSITORY_FACTORY_HPP

#include <memory>
#include <string>

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
#include "../repositories/IAttendanceRepository.hpp" // ДОБАВЛЕНО

// Forward declaration
class DatabaseConnection;

class PostgreSQLRepositoryFactory {
private:
    std::shared_ptr<DatabaseConnection> dbConnection_;

public:
    explicit PostgreSQLRepositoryFactory(const std::string& connectionString);
    ~PostgreSQLRepositoryFactory() = default;

    // Запрещаем копирование
    PostgreSQLRepositoryFactory(const PostgreSQLRepositoryFactory&) = delete;
    PostgreSQLRepositoryFactory& operator=(const PostgreSQLRepositoryFactory&) = delete;

    // Фабричные методы
    std::shared_ptr<IBookingRepository> createBookingRepository();
    std::shared_ptr<IClientRepository> createClientRepository();
    std::shared_ptr<IDanceHallRepository> createDanceHallRepository();
    std::shared_ptr<IBranchRepository> createBranchRepository();
    std::shared_ptr<IStudioRepository> createStudioRepository();
    std::shared_ptr<ITrainerRepository> createTrainerRepository();
    std::shared_ptr<ILessonRepository> createLessonRepository();
    std::shared_ptr<IEnrollmentRepository> createEnrollmentRepository();
    std::shared_ptr<IReviewRepository> createReviewRepository();
    std::shared_ptr<ISubscriptionRepository> createSubscriptionRepository();
    std::shared_ptr<ISubscriptionTypeRepository> createSubscriptionTypeRepository();
    std::shared_ptr<IAttendanceRepository> createAttendanceRepository(); // ДОБАВЛЕНО

    // Управление соединением
    bool testConnection() const;
    void reconnect();
    
    // Получение соединения (для advanced использования)
    std::shared_ptr<DatabaseConnection> getConnection() const { return dbConnection_; }
};

#endif // POSTGRESQL_REPOSITORY_FACTORY_HPP