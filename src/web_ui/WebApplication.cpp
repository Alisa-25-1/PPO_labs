#include "WebApplication.hpp"
#include "views/LoginWidget.hpp"
#include "views/ClientDashboard.hpp"
#include "views/RegistrationWidget.hpp"
#include "views/BookingView.hpp"
#include <Wt/WPushButton.h>
#include <iostream>

WebApplication::WebApplication(const Wt::WEnvironment& env)
    : WApplication(env),
      mainStack_(nullptr),
      loginView_(nullptr),
      dashboardView_(nullptr),
      registrationView_(nullptr),
      bookingView_(nullptr) {
    
    setTitle("Dance Studio");
    
    // Инициализируем контроллеры
    initializeControllers();
    
    // Создаем главный контейнер
    auto container = root()->addNew<Wt::WContainerWidget>();
    container->setStyleClass("app-container");
    
    // Создаем стек для контента
    mainStack_ = container->addNew<Wt::WStackedWidget>();
    mainStack_->setStyleClass("main-stack");
    
    // Создаем представления
    loginView_ = mainStack_->addNew<LoginWidget>(this);
    dashboardView_ = mainStack_->addNew<ClientDashboard>(this);
    registrationView_ = mainStack_->addNew<RegistrationWidget>(this);
    bookingView_ = mainStack_->addNew<BookingView>(this);
    
    setupStyles();
    showLogin();
    
    std::cout << "✅ WebApplication создан" << std::endl;
}

void WebApplication::initializeControllers() {
    // Инициализируем AuthController
    authController_ = std::make_unique<AuthController>();
    
    // Инициализируем BookingController
    // Пока создаем заглушку - в реальности нужно передать BookingService
    auto bookingService = std::make_shared<BookingService>(nullptr, nullptr, nullptr, nullptr);
    bookingController_ = std::make_unique<BookingController>(bookingService);
}

void WebApplication::setupStyles() {
    useStyleSheet("styles.css");
}

void WebApplication::showLogin() {
    std::cout << "🔄 Показываем логин" << std::endl;
    mainStack_->setCurrentWidget(loginView_);
}

void WebApplication::showDashboard() {
    std::cout << "🔄 Показываем дашборд" << std::endl;
    mainStack_->setCurrentWidget(dashboardView_);
}

void WebApplication::showRegistration() {
    std::cout << "🔄 Показываем регистрацию" << std::endl;
    mainStack_->setCurrentWidget(registrationView_);
}

void WebApplication::showBookingView() {
    std::cout << "🔄 Показываем бронирования" << std::endl;
    mainStack_->setCurrentWidget(bookingView_);
}