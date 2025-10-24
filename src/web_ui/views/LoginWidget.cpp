#include "LoginWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WTimer.h>
#include <iostream>

LoginWidget::LoginWidget(WebApplication* app) 
    : app_(app) {
    
    setStyleClass("auth-container");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("auth-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("auth-header");
    auto headerText = header->addNew<Wt::WText>("<h1 class='auth-title'>Вход в систему</h1>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Форма
    auto form = card->addNew<Wt::WContainerWidget>();
    form->setStyleClass("auth-form");
    
    // Email
    auto emailGroup = form->addNew<Wt::WContainerWidget>();
    emailGroup->setStyleClass("auth-input-group");
    
    auto emailLabel = emailGroup->addNew<Wt::WText>("<label class='form-label'>Email</label>");
    emailLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    emailEdit_ = emailGroup->addNew<Wt::WLineEdit>();
    emailEdit_->setPlaceholderText("Введите ваш email");
    emailEdit_->setStyleClass("auth-input email-input");
    
    // Пароль
    auto passwordGroup = form->addNew<Wt::WContainerWidget>();
    passwordGroup->setStyleClass("auth-input-group");
    
    auto passwordLabel = passwordGroup->addNew<Wt::WText>("<label class='form-label'>Пароль</label>");
    passwordLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    passwordEdit_ = passwordGroup->addNew<Wt::WPasswordEdit>();
    passwordEdit_->setPlaceholderText("Введите ваш пароль");
    passwordEdit_->setStyleClass("auth-input password-input");
    
    // Кнопка входа
    loginButton_ = form->addNew<Wt::WPushButton>("🚀 ВОЙТИ В СИСТЕМУ");
    loginButton_->setStyleClass("auth-button");
    loginButton_->clicked().connect(this, &LoginWidget::handleLogin);
    
    // Ссылка на регистрацию
    auto registerLink = form->addNew<Wt::WContainerWidget>();
    registerLink->setStyleClass("auth-footer");
    auto registerText = registerLink->addNew<Wt::WText>("<p>Нет аккаунта? <a href='#' class='auth-link'>Создать аккаунт</a></p>");
    registerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Обработчик клика по ссылке регистрации
    registerText->clicked().connect([this]() {
        app_->showRegistration();
    });
    
    // Статус
    statusText_ = form->addNew<Wt::WText>();
    statusText_->setStyleClass("auth-error-message");
}

void LoginWidget::handleLogin() {
    std::string email = emailEdit_->text().toUTF8();
    std::string password = passwordEdit_->text().toUTF8();
    
    std::cout << "🔐 Попытка входа: " << email << std::endl;
    
    try {
        if (email.empty() || password.empty()) {
            statusText_->setText("❌ Пожалуйста, заполните все поля");
            statusText_->setStyleClass("auth-error-message");
            return;
        }
        
        // Используем AuthController для аутентификации
        AuthResponseDTO response;
        bool success = app_->getAuthController()->login(email, password, response);
        
        if (success) {
            statusText_->setText("✅ Успешный вход! Перенаправление...");
            statusText_->setStyleClass("auth-success-message");
            
            // Сохраняем сессию пользователя
            app_->loginUser(response);
            
        } else {
            statusText_->setText("❌ Неверный email или пароль");
            statusText_->setStyleClass("auth-error-message");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка входа: " << e.what() << std::endl;
        statusText_->setText("❌ Ошибка входа: " + std::string(e.what()));
        statusText_->setStyleClass("auth-error-message");
    }
}