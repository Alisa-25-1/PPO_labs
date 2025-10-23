#ifndef POSTGRESQL_REPOSITORY_FACTORY_HPP
#define POSTGRESQL_REPOSITORY_FACTORY_HPP

#include <memory>
#include <string>

// Интерфейсы из booking_core
#include "../booking_core/repositories/IBookingRepository.hpp"
#include "../booking_core/repositories/IClientRepository.hpp"
#include "../booking_core/repositories/IDanceHallRepository.hpp"
#include "../booking_core/repositories/IBranchRepository.hpp"
#include "../booking_core/repositories/IStudioRepository.hpp"
#include "../booking_core/repositories/ITrainerRepository.hpp"
#include "../booking_core/repositories/ILessonRepository.hpp"
#include "../booking_core/repositories/IEnrollmentRepository.hpp"
#include "../booking_core/repositories/IReviewRepository.hpp"
#include "../booking_core/repositories/ISubscriptionRepository.hpp"
#include "../booking_core/repositories/ISubscriptionTypeRepository.hpp"

// Реализации из data_access
class PostgreSQLBookingRepository;
class PostgreSQLClientRepository;
class PostgreSQLBranchRepository;
class PostgreSQLDanceHallRepository;
class PostgreSQLEnrollmentRepository;
class PostgreSQLLessonRepository;
class PostgreSQLReviewRepository;
class PostgreSQLStudioRepository;
class PostgreSQLSubscriptionRepository;
class PostgreSQLSubscriptionTypeRepository;
class PostgreSQLTrainerRepository;

class DatabaseConnection;

class PostgreSQLRepositoryFactory {
private:
    std::shared_ptr<DatabaseConnection> dbConnection_;

public:
    explicit PostgreSQLRepositoryFactory(const std::string& connectionString);
    ~PostgreSQLRepositoryFactory() = default;

    // Фабричные методы для создания репозиториев
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

    // Управление соединением
    bool testConnection() const;
    void reconnect();
};

#endif // POSTGRESQL_REPOSITORY_FACTORY_HPP