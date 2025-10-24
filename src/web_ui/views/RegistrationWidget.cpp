#include "RegistrationWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WBreak.h>
#include <Wt/WTimer.h>
#include <regex>
#include <iostream>
#include <cctype>

RegistrationWidget::RegistrationWidget(WebApplication* app) 
    : app_(app) {
    setupUI();
}

void RegistrationWidget::setupUI() {
    setStyleClass("registration-container");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("registration-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("registration-header");
    auto headerText = header->addNew<Wt::WText>("<h1>📝 Регистрация</h1>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    header->addNew<Wt::WText>("<p>Создайте новую учетную запись</p>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Форма
    auto form = card->addNew<Wt::WContainerWidget>();
    form->setStyleClass("registration-form");
    
    // ФИО
    auto nameGroup = form->addNew<Wt::WContainerWidget>();
    nameGroup->setStyleClass("form-group");
    
    auto nameLabel = nameGroup->addNew<Wt::WText>("<label class='form-label'>👤 ФИО</label>");
    nameLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    nameEdit_ = nameGroup->addNew<Wt::WLineEdit>();
    nameEdit_->setPlaceholderText("Иванов Иван Иванович");
    nameEdit_->setStyleClass("form-input");
    
    // Email
    auto emailGroup = form->addNew<Wt::WContainerWidget>();
    emailGroup->setStyleClass("form-group");
    
    auto emailLabel = emailGroup->addNew<Wt::WText>("<label class='form-label'>📧 Email</label>");
    emailLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    emailEdit_ = emailGroup->addNew<Wt::WLineEdit>();
    emailEdit_->setPlaceholderText("example@mail.ru");
    emailEdit_->setStyleClass("form-input email-input");
    
    // Телефон
    auto phoneGroup = form->addNew<Wt::WContainerWidget>();
    phoneGroup->setStyleClass("form-group");
    
    auto phoneLabel = phoneGroup->addNew<Wt::WText>("<label class='form-label'>📞 Телефон</label>");
    phoneLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    phoneEdit_ = phoneGroup->addNew<Wt::WLineEdit>();
    phoneEdit_->setPlaceholderText("+7 (999) 999-99-99");
    phoneEdit_->setStyleClass("form-input");
    
    // Пароль
    auto passwordGroup = form->addNew<Wt::WContainerWidget>();
    passwordGroup->setStyleClass("form-group");
    
    auto passwordLabel = passwordGroup->addNew<Wt::WText>("<label class='form-label'>🔒 Пароль</label>");
    passwordLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    passwordEdit_ = passwordGroup->addNew<Wt::WPasswordEdit>();
    passwordEdit_->setPlaceholderText("Минимум 8 символов");
    passwordEdit_->setStyleClass("form-input password-input");
    
    // Подтверждение пароля
    auto confirmPasswordGroup = form->addNew<Wt::WContainerWidget>();
    confirmPasswordGroup->setStyleClass("form-group");
    
    auto confirmPasswordLabel = confirmPasswordGroup->addNew<Wt::WText>("<label class='form-label'>✅ Подтвердите пароль</label>");
    confirmPasswordLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    confirmPasswordEdit_ = confirmPasswordGroup->addNew<Wt::WPasswordEdit>();
    confirmPasswordEdit_->setPlaceholderText("Повторите пароль");
    confirmPasswordEdit_->setStyleClass("form-input password-input");
    
    // Кнопки
    auto buttonGroup = form->addNew<Wt::WContainerWidget>();
    buttonGroup->setStyleClass("registration-button-group");
    
    registerButton_ = buttonGroup->addNew<Wt::WPushButton>("🚀 СОЗДАТЬ АККАУНТ");
    registerButton_->setStyleClass("btn-register");
    registerButton_->clicked().connect(this, &RegistrationWidget::handleRegister);
    
    backButton_ = buttonGroup->addNew<Wt::WPushButton>("← Назад ко входу");
    backButton_->setStyleClass("btn-back");
    backButton_->clicked().connect(this, &RegistrationWidget::handleBack);
    
    // Статус
    statusText_ = form->addNew<Wt::WText>();
    statusText_->setStyleClass("registration-status");
}

void RegistrationWidget::handleRegister() {
    if (!validateForm()) {
        return;
    }
    
    std::string name = nameEdit_->text().toUTF8();
    std::string email = emailEdit_->text().toUTF8();
    std::string phone = phoneEdit_->text().toUTF8();
    std::string password = passwordEdit_->text().toUTF8();
    
    try {
        AuthResponseDTO response;
        bool success = app_->getAuthController()->registerClient(name, email, phone, password, response);
        
        if (success) {
            updateStatus("✅ Регистрация успешна! Вы будете перенаправлены.", false);
            
            // Очищаем поля
            nameEdit_->setText("");
            emailEdit_->setText("");
            phoneEdit_->setText("");
            passwordEdit_->setText("");
            confirmPasswordEdit_->setText("");
            
            // Автоматический вход после регистрации
            app_->loginUser(response);
            
        } else {
            updateStatus("❌ Ошибка регистрации. Возможно, email уже используется.", true);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка регистрации: " << e.what() << std::endl;
        updateStatus("❌ Ошибка регистрации: " + std::string(e.what()), true);
    }
}

void RegistrationWidget::handleBack() {
    app_->showLogin();
}

bool RegistrationWidget::validateForm() {
    std::string name = nameEdit_->text().toUTF8();
    std::string email = emailEdit_->text().toUTF8();
    std::string phone = phoneEdit_->text().toUTF8();
    std::string password = passwordEdit_->text().toUTF8();
    std::string confirmPassword = confirmPasswordEdit_->text().toUTF8();
    
    // Проверка обязательных полей
    if (name.empty() || email.empty() || phone.empty() || password.empty() || confirmPassword.empty()) {
        updateStatus("❌ Пожалуйста, заполните все поля", true);
        return false;
    }
    
    // Упрощенная проверка имени (разрешаем латиницу и кириллицу)
    if (!Client::isValidName(name)) {
        updateStatus("❌ ФИО может содержать буквы, проблелы и быть от 2 до 100 символов", true);
        return false;
    }
    
    // Проверка email
    std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    if (!std::regex_match(email, emailPattern)) {
        updateStatus("❌ Введите корректный email адрес", true);
        return false;
    }

    // Используем единую проверку телефона из Client
    if (!Client::isValidPhone(phone)) {
        updateStatus("❌ Введите корректный российский номер телефона (рекомендуемый формат +7 XXX XXX-XX-XX)", true);
        return false;
    }
    
    // Проверка пароля
    if (password.length() < 8) {
        updateStatus("❌ Пароль должен содержать не менее 8 символов", true);
        return false;
    }
    
    // Проверка совпадения паролей
    if (password != confirmPassword) {
        updateStatus("❌ Пароли не совпадают", true);
        return false;
    }
    
    return true;
}

void RegistrationWidget::updateStatus(const std::string& message, bool isError) {
    statusText_->setText(message);
    if (isError) {
        statusText_->removeStyleClass("status-success");
        statusText_->addStyleClass("status-error");
    } else {
        statusText_->removeStyleClass("status-error");
        statusText_->addStyleClass("status-success");
    }
}