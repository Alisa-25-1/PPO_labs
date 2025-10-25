#include "TechUIManagers.hpp"
#include "../data/PostgreSQLRepositoryFactory.hpp"
#include "../services/BranchService.hpp"
#include "../repositories/impl/PostgreSQLEnrollmentRepository.hpp"

TechUIManagers::TechUIManagers(const std::string& connectionString) {
    try {
        // Используем фабрику для создания всех репозиториев
        auto factory = std::make_shared<PostgreSQLRepositoryFactory>(connectionString);
        
        // Создаем все репозитории через фабрику
        clientRepo_ = factory->createClientRepository();
        hallRepo_ = factory->createDanceHallRepository();
        bookingRepo_ = factory->createBookingRepository();
        lessonRepo_ = factory->createLessonRepository();
        trainerRepo_ = factory->createTrainerRepository();
        enrollmentRepo_ = factory->createEnrollmentRepository();
        subscriptionRepo_ = factory->createSubscriptionRepository();
        subscriptionTypeRepo_ = factory->createSubscriptionTypeRepository();
        reviewRepo_ = factory->createReviewRepository();
        branchRepo_ = factory->createBranchRepository();
        studioRepo_ = factory->createStudioRepository();
        attendanceRepo_ = factory->createAttendanceRepository(); 
        
        auto branchService = std::make_shared<BranchService>(branchRepo_, hallRepo_);
        
        authService_ = std::make_unique<AuthService>(clientRepo_);
        
        bookingService_ = std::make_unique<BookingService>(
            bookingRepo_,
            clientRepo_, 
            hallRepo_,
            branchRepo_,
            branchService,
            attendanceRepo_  
        );
        
        lessonService_ = std::make_unique<LessonService>(
            lessonRepo_,
            enrollmentRepo_,
            trainerRepo_,
            hallRepo_
        );
    
        enrollmentService_ = std::make_unique<EnrollmentService>(
            enrollmentRepo_,
            clientRepo_,
            lessonRepo_,
            attendanceRepo_  
        );
        
        subscriptionService_ = std::make_unique<SubscriptionService>(
            subscriptionRepo_,
            subscriptionTypeRepo_,
            clientRepo_
        );
        
        reviewService_ = std::make_unique<ReviewService>(
            reviewRepo_,
            clientRepo_,
            lessonRepo_,
            enrollmentRepo_
        );
        
        scheduleService_ = std::make_unique<ScheduleService>(
            lessonRepo_,
            bookingRepo_,
            hallRepo_
        );

        statisticsService_ = std::make_unique<StatisticsService>(
            attendanceRepo_,  
            clientRepo_,
            lessonRepo_,
            bookingRepo_,
            enrollmentRepo_
        );
        
        statisticsManager_ = std::make_unique<StatisticsManager>(statisticsService_.get());
        
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

std::vector<Client> TechUIManagers::getAllClients() const {
    try {
        return clientRepo_->findAll();
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения списка клиентов: " << e.what() << std::endl;
        return {};
    }
}

std::vector<BookingResponseDTO> TechUIManagers::getAllBookings() const {
    try {
        auto bookings = bookingRepo_->findAll();
        std::vector<BookingResponseDTO> result;
        
        for (const auto& booking : bookings) {
            result.push_back(BookingResponseDTO(booking));
        }
        
        return result;
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения списка бронирований: " << e.what() << std::endl;
        return {};
    }
}