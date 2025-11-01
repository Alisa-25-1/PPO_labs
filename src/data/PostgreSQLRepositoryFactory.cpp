#include "PostgreSQLRepositoryFactory.hpp"
#include "../data/DatabaseConnection.hpp"
#include "../repositories/impl/PostgreSQLBookingRepository.hpp"
#include "../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../repositories/impl/PostgreSQLDanceHallRepository.hpp"
#include "../repositories/impl/PostgreSQLBranchRepository.hpp"
#include "../repositories/impl/PostgreSQLStudioRepository.hpp"
#include "../repositories/impl/PostgreSQLTrainerRepository.hpp"
#include "../repositories/impl/PostgreSQLLessonRepository.hpp"
#include "../repositories/impl/PostgreSQLEnrollmentRepository.hpp"
#include "../repositories/impl/PostgreSQLReviewRepository.hpp"
#include "../repositories/impl/PostgreSQLSubscriptionRepository.hpp"
#include "../repositories/impl/PostgreSQLSubscriptionTypeRepository.hpp"
#include "../repositories/impl/PostgreSQLAttendanceRepository.hpp"

PostgreSQLRepositoryFactory::PostgreSQLRepositoryFactory(const std::string& connectionString) {
    dbConnection_ = std::make_shared<DatabaseConnection>(connectionString);
}

std::shared_ptr<IBookingRepository> PostgreSQLRepositoryFactory::createBookingRepository() {
    return std::make_shared<PostgreSQLBookingRepository>(dbConnection_);
}

std::shared_ptr<IClientRepository> PostgreSQLRepositoryFactory::createClientRepository() {
    return std::make_shared<PostgreSQLClientRepository>(dbConnection_);
}

std::shared_ptr<IDanceHallRepository> PostgreSQLRepositoryFactory::createDanceHallRepository() {
    return std::make_shared<PostgreSQLDanceHallRepository>(dbConnection_);
}

std::shared_ptr<IBranchRepository> PostgreSQLRepositoryFactory::createBranchRepository() {
    return std::make_shared<PostgreSQLBranchRepository>(dbConnection_);
}

std::shared_ptr<IStudioRepository> PostgreSQLRepositoryFactory::createStudioRepository() {
    return std::make_shared<PostgreSQLStudioRepository>(dbConnection_);
}

std::shared_ptr<ITrainerRepository> PostgreSQLRepositoryFactory::createTrainerRepository() {
    return std::make_shared<PostgreSQLTrainerRepository>(dbConnection_);
}

std::shared_ptr<ILessonRepository> PostgreSQLRepositoryFactory::createLessonRepository() {
    return std::make_shared<PostgreSQLLessonRepository>(dbConnection_);
}

std::shared_ptr<IEnrollmentRepository> PostgreSQLRepositoryFactory::createEnrollmentRepository() {
    return std::make_shared<PostgreSQLEnrollmentRepository>(dbConnection_);
}

std::shared_ptr<IReviewRepository> PostgreSQLRepositoryFactory::createReviewRepository() {
    return std::make_shared<PostgreSQLReviewRepository>(dbConnection_);
}

std::shared_ptr<ISubscriptionRepository> PostgreSQLRepositoryFactory::createSubscriptionRepository() {
    return std::make_shared<PostgreSQLSubscriptionRepository>(dbConnection_);
}

std::shared_ptr<ISubscriptionTypeRepository> PostgreSQLRepositoryFactory::createSubscriptionTypeRepository() {
    return std::make_shared<PostgreSQLSubscriptionTypeRepository>(dbConnection_);
}

std::shared_ptr<IAttendanceRepository> PostgreSQLRepositoryFactory::createAttendanceRepository() {
    return std::make_shared<PostgreSQLAttendanceRepository>(dbConnection_);
}

bool PostgreSQLRepositoryFactory::testConnection() const {
    try {
        pqxx::connection& conn = dbConnection_->getConnection();
        return conn.is_open();
    } catch (...) {
        return false;
    }
}

void PostgreSQLRepositoryFactory::reconnect() {
    if (dbConnection_) {
    }
}