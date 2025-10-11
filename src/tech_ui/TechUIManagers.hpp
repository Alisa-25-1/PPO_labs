#ifndef TECHUIMANAGERS_HPP
#define TECHUIMANAGERS_HPP

#include <memory>
#include <iostream>
#include <vector>
#include "../services/AuthService.hpp"
#include "../services/BookingService.hpp"
#include "../services/LessonService.hpp"
#include "../services/SubscriptionService.hpp"
#include "../services/ReviewService.hpp"
#include "../services/ScheduleService.hpp"
#include "../services/EnrollmentService.hpp"
#include "../data/ResilientDatabaseConnection.hpp"
#include "../models/DanceHall.hpp"
#include "../models/Trainer.hpp"
#include "../models/Lesson.hpp"
#include "../models/SubscriptionType.hpp"

// Forward declarations
class IClientRepository;
class IDanceHallRepository;
class IBookingRepository;
class ILessonRepository;
class ITrainerRepository;
class IEnrollmentRepository;
class ISubscriptionRepository;
class ISubscriptionTypeRepository;
class IReviewRepository;
class IBranchRepository;
class IStudioRepository;

class TechUIManagers {
private:
    std::shared_ptr<ResilientDatabaseConnection> dbConnection_;
    
    // Репозитории
    std::shared_ptr<IClientRepository> clientRepo_;
    std::shared_ptr<IDanceHallRepository> hallRepo_;
    std::shared_ptr<IBookingRepository> bookingRepo_;
    std::shared_ptr<ILessonRepository> lessonRepo_;
    std::shared_ptr<ITrainerRepository> trainerRepo_;
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo_;
    std::shared_ptr<ISubscriptionRepository> subscriptionRepo_;
    std::shared_ptr<ISubscriptionTypeRepository> subscriptionTypeRepo_;
    std::shared_ptr<IReviewRepository> reviewRepo_;
    std::shared_ptr<IBranchRepository> branchRepo_;
    std::shared_ptr<IStudioRepository> studioRepo_;

    // Сервисы
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<BookingService> bookingService_;
    std::unique_ptr<LessonService> lessonService_;
    std::unique_ptr<SubscriptionService> subscriptionService_;
    std::unique_ptr<ReviewService> reviewService_;
    std::unique_ptr<ScheduleService> scheduleService_;
    std::unique_ptr<EnrollmentService> enrollmentService_;

public:
    TechUIManagers(const std::string& connectionString);
    
    // Геттеры для сервисов
    AuthService* getAuthService() { return authService_.get(); }
    BookingService* getBookingService() { return bookingService_.get(); }
    LessonService* getLessonService() { return lessonService_.get(); }
    SubscriptionService* getSubscriptionService() { return subscriptionService_.get(); }
    ReviewService* getReviewService() { return reviewService_.get(); }
    ScheduleService* getScheduleService() { return scheduleService_.get(); }
    EnrollmentService* getEnrollmentService() { return enrollmentService_.get(); }
    
    // Геттеры для репозиториев
    std::shared_ptr<IClientRepository> getClientRepo() { return clientRepo_; }
    std::shared_ptr<IDanceHallRepository> getHallRepo() { return hallRepo_; }
    std::shared_ptr<ITrainerRepository> getTrainerRepo() { return trainerRepo_; }
    std::shared_ptr<ISubscriptionTypeRepository> getSubscriptionTypeRepo() { return subscriptionTypeRepo_; }
    std::shared_ptr<ILessonRepository> getLessonRepo() { return lessonRepo_; }
    
    // Вспомогательные методы
    std::vector<DanceHall> getAvailableHalls() const;
    std::vector<Trainer> getActiveTrainers() const;
    std::vector<SubscriptionType> getSubscriptionTypes() const;
    std::vector<Lesson> getUpcomingLessons(int days = 7) const;
};

#endif // TECHUIMANAGERS_HPP