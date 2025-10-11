#include "TechUIManagers.hpp"
#include "../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../repositories/impl/PostgreSQLDanceHallRepository.hpp"
#include "../repositories/impl/PostgreSQLBookingRepository.hpp"
#include "../repositories/impl/PostgreSQLLessonRepository.hpp"
#include "../repositories/impl/PostgreSQLTrainerRepository.hpp"
#include "../repositories/impl/PostgreSQLEnrollmentRepository.hpp"
#include "../repositories/impl/PostgreSQLSubscriptionRepository.hpp"
#include "../repositories/impl/PostgreSQLSubscriptionTypeRepository.hpp"
#include "../repositories/impl/PostgreSQLReviewRepository.hpp"
#include "../repositories/impl/PostgreSQLBranchRepository.hpp"
#include "../repositories/impl/PostgreSQLStudioRepository.hpp"

TechUIManagers::TechUIManagers(const std::string& connectionString) {
    try {
        dbConnection_ = std::make_shared<DatabaseConnection>(connectionString);
        
        // Инициализация репозиториев
        clientRepo_ = std::make_shared<PostgreSQLClientRepository>(dbConnection_);
        hallRepo_ = std::make_shared<PostgreSQLDanceHallRepository>(dbConnection_);
        bookingRepo_ = std::make_shared<PostgreSQLBookingRepository>(dbConnection_);
        lessonRepo_ = std::make_shared<PostgreSQLLessonRepository>(dbConnection_);
        trainerRepo_ = std::make_shared<PostgreSQLTrainerRepository>(dbConnection_);
        enrollmentRepo_ = std::make_shared<PostgreSQLEnrollmentRepository>(dbConnection_);
        subscriptionRepo_ = std::make_shared<PostgreSQLSubscriptionRepository>(dbConnection_);
        subscriptionTypeRepo_ = std::make_shared<PostgreSQLSubscriptionTypeRepository>(dbConnection_);
        reviewRepo_ = std::make_shared<PostgreSQLReviewRepository>(dbConnection_);
        branchRepo_ = std::make_shared<PostgreSQLBranchRepository>(dbConnection_);
        studioRepo_ = std::make_shared<PostgreSQLStudioRepository>(dbConnection_);
        
        // Инициализация сервисов
        authService_ = std::make_unique<AuthService>(
            std::make_unique<PostgreSQLClientRepository>(dbConnection_)
        );
        
        bookingService_ = std::make_unique<BookingService>(
            std::make_unique<PostgreSQLBookingRepository>(dbConnection_),
            std::make_unique<PostgreSQLClientRepository>(dbConnection_),
            std::make_unique<PostgreSQLDanceHallRepository>(dbConnection_)
        );
        
        lessonService_ = std::make_unique<LessonService>(
            std::make_unique<PostgreSQLLessonRepository>(dbConnection_),
            std::make_unique<PostgreSQLEnrollmentRepository>(dbConnection_),
            std::make_unique<PostgreSQLTrainerRepository>(dbConnection_),
            std::make_unique<PostgreSQLDanceHallRepository>(dbConnection_)
        );
        
        subscriptionService_ = std::make_unique<SubscriptionService>(
            std::make_unique<PostgreSQLSubscriptionRepository>(dbConnection_),
            std::make_unique<PostgreSQLSubscriptionTypeRepository>(dbConnection_),
            std::make_unique<PostgreSQLClientRepository>(dbConnection_)
        );
        
        reviewService_ = std::make_unique<ReviewService>(
            std::make_unique<PostgreSQLReviewRepository>(dbConnection_),
            std::make_unique<PostgreSQLClientRepository>(dbConnection_),
            std::make_unique<PostgreSQLLessonRepository>(dbConnection_),
            std::make_unique<PostgreSQLEnrollmentRepository>(dbConnection_)
        );
        
        scheduleService_ = std::make_unique<ScheduleService>(
            std::make_unique<PostgreSQLLessonRepository>(dbConnection_),
            std::make_unique<PostgreSQLBookingRepository>(dbConnection_),
            std::make_unique<PostgreSQLDanceHallRepository>(dbConnection_)
        );

        enrollmentService_ = std::make_unique<EnrollmentService>(
            std::make_unique<PostgreSQLEnrollmentRepository>(dbConnection_),
            std::make_unique<PostgreSQLClientRepository>(dbConnection_),
            std::make_unique<PostgreSQLLessonRepository>(dbConnection_)
        );
        
        std::cout << "✅ Все менеджеры TechUI инициализированы" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка инициализации TechUIManagers: " << e.what() << std::endl;
        throw;
    }
}

std::vector<DanceHall> TechUIManagers::getAvailableHalls() const {
    try {
        return hallRepo_->findAll();
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения списка залов: " << e.what() << std::endl;
        return {};
    }
}

std::vector<Trainer> TechUIManagers::getActiveTrainers() const {
    try {
        return trainerRepo_->findActiveTrainers();
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения списка преподавателей: " << e.what() << std::endl;
        return {};
    }
}

std::vector<SubscriptionType> TechUIManagers::getSubscriptionTypes() const {
    try {
        return subscriptionTypeRepo_->findAllActive();
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения типов абонементов: " << e.what() << std::endl;
        return {};
    }
}

std::vector<Lesson> TechUIManagers::getUpcomingLessons(int days) const {
    try {
        return lessonRepo_->findUpcomingLessons(days);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения предстоящих занятий: " << e.what() << std::endl;
        return {};
    }
}