#include "ClientDashboard.hpp"
#include "web_ui/WebApplication.hpp"
#include <iostream>

ClientDashboard::ClientDashboard(WebApplication* app) 
    : app_(app) {
    
    setupUI();
}

void ClientDashboard::setupUI() {
    setStyleClass("dashboard-container");
    
    // Яркий заголовок
    auto title = addNew<Wt::WText>("<h1>🌟 ЛИЧНЫЙ КАБИНЕТ 🌟</h1>");
    title->setStyleClass("dashboard-title");
    
    // Подзаголовок
    auto subtitle = addNew<Wt::WText>("<p>Добро пожаловать в систему!</p>");
    subtitle->setStyleClass("dashboard-subtitle");
    
    // Простое меню
    auto menu = addNew<Wt::WContainerWidget>();
    menu->setStyleClass("dashboard-menu");
    
    // Кнопка бронирований
    auto bookingBtn = menu->addNew<Wt::WPushButton>("🏟️ Бронирование залов");
    bookingBtn->setStyleClass("btn-menu");
    bookingBtn->clicked().connect([this]() {
        app_->showBookingView();
    });
    
    std::vector<std::string> buttons = {
        "📅 Расписание",
        "🎓 Мои занятия", 
        "🎫 Абонементы",
        "⭐ Отзывы"
    };
    
    for (const auto& text : buttons) {
        auto btn = menu->addNew<Wt::WPushButton>(text);
        btn->setStyleClass("btn-menu");
        
        btn->clicked().connect([text]() {
            std::cout << "🎯 Нажата кнопка: " << text << std::endl;
        });
    }
    
    // Кнопка выхода
    auto logoutBtn = addNew<Wt::WPushButton>("🚪 Выйти");
    logoutBtn->setStyleClass("btn-logout");
    logoutBtn->clicked().connect([this]() {
        app_->showLogin();
    });
    
    std::cout << "✅ Дашборд создан" << std::endl;
}