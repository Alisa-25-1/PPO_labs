#include "WebApplication.hpp"
#include "views/LoginWidget.hpp"
#include "views/ClientDashboard.hpp"
#include "views/RegistrationWidget.hpp"
#include "views/BookingView.hpp"
#include "views/SubscriptionView.hpp"
#include "views/LessonView.hpp"

// Контроллеры
#include "controllers/AuthController.hpp"
#include "controllers/BookingController.hpp"
#include "controllers/SubscriptionController.hpp"

// Репозитории
#include "repositories/impl/PostgreSQLBookingRepository.hpp"
#include "repositories/impl/PostgreSQLClientRepository.hpp"
#include "repositories/impl/PostgreSQLDanceHallRepository.hpp"
#include "repositories/impl/PostgreSQLBranchRepository.hpp"
#include "repositories/impl/PostgreSQLSubscriptionRepository.hpp"
#include "repositories/impl/PostgreSQLSubscriptionTypeRepository.hpp"
#include "repositories/impl/PostgreSQLLessonRepository.hpp"
#include "repositories/impl/PostgreSQLTrainerRepository.hpp"
#include "repositories/impl/PostgreSQLEnrollmentRepository.hpp"
#include "repositories/impl/PostgreSQLAttendanceRepository.hpp"

// Сервисы
#include "services/BookingService.hpp"
#include "services/SubscriptionService.hpp"
#include "services/LessonService.hpp"
#include "services/BranchService.hpp"
#include "services/EnrollmentService.hpp"
#include "services/AttendanceService.hpp"

// Данные
#include "data/ResilientDatabaseConnection.hpp"  
#include "services/DatabaseHealthService.hpp"

#include <Wt/WPushButton.h>
#include <iostream>

WebApplication::WebApplication(const Wt::WEnvironment& env)
    : WApplication(env),
      mainStack_(nullptr),
      loginView_(nullptr),
      dashboardView_(nullptr),
      registrationView_(nullptr),
      bookingView_(nullptr),
      subscriptionView_(nullptr),
      lessonView_(nullptr) {
    
    setTitle("Dance Studio");
    
    initializeControllers();
    
    auto container = root()->addNew<Wt::WContainerWidget>();
    container->setStyleClass("app-container");
    
    mainStack_ = container->addNew<Wt::WStackedWidget>();
    mainStack_->setStyleClass("main-stack");
    
    loginView_ = mainStack_->addNew<LoginWidget>(this);
    registrationView_ = mainStack_->addNew<RegistrationWidget>(this);
    
    dashboardView_ = nullptr;
    bookingView_ = nullptr;
    subscriptionView_ = nullptr;
    lessonView_ = nullptr;
    
    setupStyles();
    
    showLogin();
    
    std::cout << "✅ WebApplication создан" << std::endl;
}

void WebApplication::handleDatabaseError(const std::string& context) {
    std::cerr << "❌ Database error in " << context << std::endl;
    
    // Показываем пользователю сообщение об ошибке
    auto errorContainer = root()->addNew<Wt::WContainerWidget>();
    errorContainer->setStyleClass("error-container");
    
    errorContainer->addNew<Wt::WText>(
        "<div style='text-align: center; padding: 2rem;'>"
        "<h2>😔 Временные неполадки</h2>"
        "<p>Система временно недоступна. Пожалуйста, попробуйте позже.</p>"
        "<p><small>Мы уже работаем над решением проблемы</small></p>"
        "</div>"
    )->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Скрываем основной интерфейс
    if (mainStack_) {
        mainStack_->hide();
    }
}

void WebApplication::initializeControllers() {
    try {
        std::cout << "🔧 Инициализация контроллеров..." << std::endl;
        
        // Проверяем состояние БД перед инициализацией
        if (!DatabaseHealthService::isDatabaseHealthy()) {
            throw std::runtime_error("Database is currently unavailable");
        }
        
        auto connectionString = "postgresql://dance_user:dance_password@localhost/dance_studio";
        auto dbConnection = std::make_shared<ResilientDatabaseConnection>(connectionString); 
        std::cout << "✅ Устойчивое подключение к БД создано" << std::endl;
        
        // Инициализируем AuthController
        authController_ = std::make_unique<AuthController>();
        std::cout << "✅ AuthController создан" << std::endl;
        
        // Создаем репозитории для Services
        auto lessonRepo = std::make_shared<PostgreSQLLessonRepository>(dbConnection);
        auto enrollmentRepo = std::make_shared<PostgreSQLEnrollmentRepository>(dbConnection);
        auto trainerRepo = std::make_shared<PostgreSQLTrainerRepository>(dbConnection);
        auto subscriptionRepo = std::make_shared<PostgreSQLSubscriptionRepository>(dbConnection);
        auto subscriptionTypeRepo = std::make_shared<PostgreSQLSubscriptionTypeRepository>(dbConnection);
        auto bookingRepo = std::make_shared<PostgreSQLBookingRepository>(dbConnection);
        auto clientRepo = std::make_shared<PostgreSQLClientRepository>(dbConnection);
        auto hallRepo = std::make_shared<PostgreSQLDanceHallRepository>(dbConnection);
        auto branchRepo = std::make_shared<PostgreSQLBranchRepository>(dbConnection);
        auto attendanceRepo = std::make_shared<PostgreSQLAttendanceRepository>(dbConnection);

        auto attendanceService = std::make_shared<AttendanceService>(attendanceRepo, bookingRepo, enrollmentRepo, lessonRepo);
        auto branchService = std::make_shared<BranchService>(branchRepo, hallRepo);
        auto lessonService = std::make_shared<LessonService>(lessonRepo, enrollmentRepo, trainerRepo, hallRepo);
        auto enrollmentService = std::make_shared<EnrollmentService>(enrollmentRepo, clientRepo, lessonRepo, attendanceService);

        lessonController_ = std::make_unique<LessonController>(lessonService, enrollmentService, branchService);
        std::cout << "✅ LessonController создан" << std::endl;

        auto subscriptionService = std::make_shared<SubscriptionService>(
            subscriptionRepo, subscriptionTypeRepo, clientRepo);

        subscriptionController_ = std::make_unique<SubscriptionController>(subscriptionService);
        std::cout << "✅ SubscriptionController создан" << std::endl;
        
        auto bookingService = std::make_shared<BookingService>(
            bookingRepo, 
            clientRepo, 
            hallRepo, 
            branchRepo,
            branchService,
            lessonRepo,
            attendanceService
        );
        std::cout << "✅ BookingService создан" << std::endl;
        
        bookingController_ = std::make_unique<BookingController>(bookingService);
        std::cout << "✅ BookingController создан" << std::endl;
        
        std::cout << "✅ Все контроллеры инициализированы" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка инициализации контроллеров: " << e.what() << std::endl;
        handleDatabaseError("initializeControllers");
        throw;
    }
}

void WebApplication::setupStyles() {
    useStyleSheet("styles/main.css");
}

void WebApplication::loginUser(const AuthResponseDTO& authResponse) {
    try {
        std::cout << "🔐 Вход пользователя: " << authResponse.name << " (" << authResponse.email << ")" << std::endl;
    
        userSession_.login(authResponse.clientId, authResponse.name, 
                          authResponse.email, UserRole::CLIENT);

        if (!dashboardView_) {
            dashboardView_ = mainStack_->addNew<ClientDashboard>(this);
        }
        
        showDashboard();
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка входа пользователя: " << e.what() << std::endl;
        showLogin();
    }
}

void WebApplication::logoutUser() {
    userSession_.logout();
    
    dashboardView_ = nullptr;
    bookingView_ = nullptr;
    subscriptionView_ = nullptr;
    lessonView_ = nullptr;
    
    showLogin();
}

void WebApplication::showLogin() {
    std::cout << "🔄 Показываем логин" << std::endl;
    mainStack_->setCurrentWidget(loginView_);
}

void WebApplication::showDashboard() {
    if (!dashboardView_) {
        dashboardView_ = mainStack_->addNew<ClientDashboard>(this);
    }
    std::cout << "🔄 Показываем дашборд для: " << userSession_.getUserName() << std::endl;
    mainStack_->setCurrentWidget(dashboardView_);
}

void WebApplication::showRegistration() {
    std::cout << "🔄 Показываем регистрацию" << std::endl;
    mainStack_->setCurrentWidget(registrationView_);
}

void WebApplication::showBookingView() {
    try {
        std::cout << "🔄 Запрос на показ бронирований..." << std::endl;
        
        if (!bookingView_) {
            std::cout << "🔧 Создание нового BookingView..." << std::endl;
            bookingView_ = mainStack_->addNew<BookingView>(this);
            std::cout << "✅ BookingView создан" << std::endl;
        }
        
        std::cout << "🔄 Показываем бронирования для пользователя: " << userSession_.getUserName() << std::endl;
        mainStack_->setCurrentWidget(bookingView_);
        std::cout << "✅ Бронирования показаны" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ КРИТИЧЕСКАЯ ОШИБКА при создании BookingView: " << e.what() << std::endl;
        handleDatabaseError("showBookingView");
    }
}

void WebApplication::showSubscriptionView() {
    try {
        std::cout << "🔄 Запрос на показ абонементов..." << std::endl;
        
        if (!subscriptionView_) {
            std::cout << "🔧 Создание нового SubscriptionView..." << std::endl;
            subscriptionView_ = mainStack_->addNew<SubscriptionView>(this);
            std::cout << "✅ SubscriptionView создан" << std::endl;
        }
        
        std::cout << "🔄 Показываем абонемненты для пользователя: " << userSession_.getUserName() << std::endl;
        mainStack_->setCurrentWidget(subscriptionView_);
        std::cout << "✅ Абонементы показаны" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ КРИТИЧЕСКАЯ ОШИБКА при создании BookingView: " << e.what() << std::endl;
        handleDatabaseError("showSubscriptionView");
    }
}

void WebApplication::showLessonView() {
    try {
        std::cout << "🔄 Запрос на показ занятий..." << std::endl;
        
        if (!lessonView_) {
            std::cout << "🔧 Создание нового LessonView..." << std::endl;
            lessonView_ = mainStack_->addNew<LessonView>(this);
            std::cout << "✅ LessonView создан" << std::endl;
        }
        
        std::cout << "🔄 Показываем занятия для пользователя: " << userSession_.getUserName() << std::endl;
        mainStack_->setCurrentWidget(lessonView_);
        std::cout << "✅ Занятия показаны" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при создании LessonView: " << e.what() << std::endl;
        handleDatabaseError("showLessonView");
    }
}