#include "ClientDashboard.hpp"
#include "web_ui/WebApplication.hpp"
#include <iostream>

ClientDashboard::ClientDashboard(WebApplication* app) 
    : app_(app) {
    
    setupUI();
}

void ClientDashboard::setupUI() {
    setStyleClass("dashboard-container");
    
    // Приветствие с именем пользователя
    std::string userName = app_->getUserSession().getUserName();
    std::string welcomeText = "<h1>🌟 ДОБРО ПОЖАЛОВАТЬ, " + (userName.empty() ? "ПОЛЬЗОВАТЕЛЬ" : userName) + "! 🌟</h1>";
    auto title = addNew<Wt::WText>(welcomeText);
    title->setStyleClass("dashboard-title");
    
    // Подзаголовок
    auto subtitle = addNew<Wt::WText>("<p>Управляйте своими бронированиями и занятиями</p>");
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

    auto lessonsBtn = menu->addNew<Wt::WPushButton>("🎓 Мои занятия");
    lessonsBtn->setStyleClass("btn-menu");
    lessonsBtn->clicked().connect([this]() {
        app_->showLessonView();
    });

    // Кнопка абонементов
    auto subscriptionBtn = menu->addNew<Wt::WPushButton>("🎫 Абонементы");
    subscriptionBtn->setStyleClass("btn-menu");
    subscriptionBtn->clicked().connect([this]() {
        app_->showSubscriptionView();
    });
    
    std::vector<std::string> buttons = {
        "📅 Расписание",
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
        app_->logoutUser();
    });
    
    std::cout << "✅ Дашборд создан для пользователя: " << userName << std::endl;
}