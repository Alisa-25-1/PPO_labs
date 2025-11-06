#pragma once
#include <memory>
#include <vector>
#include "../services/AuthService.hpp"
#include "../services/BookingService.hpp"
#include "../services/LessonService.hpp"
#include "../services/EnrollmentService.hpp"
#include "../services/SubscriptionService.hpp"
#include "../services/ReviewService.hpp"
#include "../services/ScheduleService.hpp"
#include "../services/StatisticsService.hpp"
#include "../services/AttendanceService.hpp"
#include "StatisticsManager.hpp" 
#include "../data/IRepositoryFactory.hpp"
#include "../data/RepositoryFactoryCreator.hpp"
#include "../core/Config.hpp"

class TechUIManagers {
private:
    // Фабрика репозиториев
    std::shared_ptr<IRepositoryFactory> repositoryFactory_;
    
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
    std::shared_ptr<IAttendanceRepository> attendanceRepo_;

    // Сервисы
    std::unique_ptr<AuthService> authService_;
    std::unique_ptr<BookingService> bookingService_;
    std::unique_ptr<LessonService> lessonService_;
    std::unique_ptr<EnrollmentService> enrollmentService_;
    std::unique_ptr<SubscriptionService> subscriptionService_;
    std::unique_ptr<ReviewService> reviewService_;
    std::unique_ptr<ScheduleService> scheduleService_;
    std::unique_ptr<StatisticsService> statisticsService_;
    std::unique_ptr<StatisticsService> attendanceService_;

    // Менеджеры UI
    std::unique_ptr<StatisticsManager> statisticsManager_;

public:
    explicit TechUIManagers(const Config& config);
    
    // Геттеры для сервисов
    AuthService* getAuthService() const { return authService_.get(); }
    BookingService* getBookingService() const { return bookingService_.get(); }
    LessonService* getLessonService() const { return lessonService_.get(); }
    EnrollmentService* getEnrollmentService() const { return enrollmentService_.get(); }
    SubscriptionService* getSubscriptionService() const { return subscriptionService_.get(); }
    ReviewService* getReviewService() const { return reviewService_.get(); }
    ScheduleService* getScheduleService() const { return scheduleService_.get(); }
    StatisticsService* getStatisticsService() const { return statisticsService_.get(); }
    
    // Геттеры для репозиториев
    std::shared_ptr<IClientRepository> getClientRepo() const { return clientRepo_; }
    std::shared_ptr<IDanceHallRepository> getHallRepo() const { return hallRepo_; }
    std::shared_ptr<ILessonRepository> getLessonRepo() const { return lessonRepo_; }
    std::shared_ptr<ITrainerRepository> getTrainerRepo() const { return trainerRepo_; }
    std::shared_ptr<IEnrollmentRepository> getEnrollmentRepo() const { return enrollmentRepo_; }
    
    // Геттер для StatisticsManager
    StatisticsManager* getStatisticsManager() const { return statisticsManager_.get(); } 
    
    // Методы для получения данных
    std::vector<DanceHall> getAvailableHalls() const;
    std::vector<Trainer> getActiveTrainers() const;
    std::vector<SubscriptionType> getSubscriptionTypes() const;
    std::vector<Lesson> getUpcomingLessons(int days = 7) const;
    std::vector<Client> getAllClients() const;
    std::vector<BookingResponseDTO> getAllBookings() const;
};