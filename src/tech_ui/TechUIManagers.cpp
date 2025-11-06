#include "TechUIManagers.hpp"
#include "../services/BranchService.hpp"

TechUIManagers::TechUIManagers(const Config& config) {
    try {
        // Создаем фабрику репозиториев на основе конфигурации
        repositoryFactory_ = RepositoryFactoryCreator::createFactory(config);
        
        // Создаем все репозитории через фабрику
        clientRepo_ = repositoryFactory_->createClientRepository();
        hallRepo_ = repositoryFactory_->createDanceHallRepository();
        bookingRepo_ = repositoryFactory_->createBookingRepository();
        lessonRepo_ = repositoryFactory_->createLessonRepository();
        trainerRepo_ = repositoryFactory_->createTrainerRepository();
        enrollmentRepo_ = repositoryFactory_->createEnrollmentRepository();
        subscriptionRepo_ = repositoryFactory_->createSubscriptionRepository();
        subscriptionTypeRepo_ = repositoryFactory_->createSubscriptionTypeRepository();
        reviewRepo_ = repositoryFactory_->createReviewRepository();
        branchRepo_ = repositoryFactory_->createBranchRepository();
        studioRepo_ = repositoryFactory_->createStudioRepository();
        attendanceRepo_ = repositoryFactory_->createAttendanceRepository(); 

        // Тестируем соединение
        if (!repositoryFactory_->testConnection()) {
            throw std::runtime_error("Database connection test failed");
        }
        
        auto attendanceService = std::make_shared<AttendanceService>(
            attendanceRepo_,
            bookingRepo_,
            enrollmentRepo_,
            lessonRepo_
        );

        auto branchService = std::make_shared<BranchService>(branchRepo_, hallRepo_);
        
        authService_ = std::make_unique<AuthService>(clientRepo_);
        
        bookingService_ = std::make_unique<BookingService>(
            bookingRepo_,
            clientRepo_, 
            hallRepo_,
            branchRepo_,
            branchService,
            lessonRepo_,
            attendanceService
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
            attendanceService  
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
            enrollmentRepo_,
            attendanceService
        );
        
        statisticsManager_ = std::make_unique<StatisticsManager>(statisticsService_.get());
        
        std::cout << "✅ Все менеджеры TechUI инициализированы с БД: " 
                  << config.getDatabaseType() << std::endl;
        
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