#include "AuthController.hpp"
#include "../../services/exceptions/AuthException.hpp"
#include "../../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include <iostream>

AuthController::AuthController() {
    initializeServices();
}

void AuthController::initializeServices() {
    try {
        auto connectionString = "postgresql://dance_user:dance_password@localhost/dance_studio";
        auto dbConnection = std::make_shared<DatabaseConnection>(connectionString);
        
        auto clientRepo = std::make_shared<PostgreSQLClientRepository>(dbConnection);
        authService_ = std::make_unique<AuthService>(clientRepo);
        
        std::cout << "✅ AuthController инициализирован (без ValidationService)" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка инициализации AuthController: " << e.what() << std::endl;
        throw;
    }
}

bool AuthController::login(const std::string& email, const std::string& password, AuthResponseDTO& response) {
    try {
        AuthRequestDTO request(email, password);
        response = authService_->login(request);
        std::cout << "✅ Успешный вход для: " << email << std::endl;
        return true;
    } catch (const InvalidCredentialsException& e) {
        std::cerr << "❌ Неверные учетные данные: " << email << std::endl;
        return false;
    } catch (const AccountInactiveException& e) {
        std::cerr << "❌ Аккаунт неактивен: " << email << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка входа: " << e.what() << std::endl;
        return false;
    }
}

bool AuthController::registerClient(const std::string& name, const std::string& email,
                                   const std::string& phone, const std::string& password,
                                   AuthResponseDTO& response) {
    try {
        std::cout << "🔧 AuthController::registerClient - Начало регистрации: " << email << std::endl;
        
        AuthRequestDTO request(name, email, phone, password);
        response = authService_->registerClient(request);
        
        std::cout << "✅ Успешная регистрация: " << email << " (" << name << ")" << std::endl;
        return true;
    } catch (const EmailAlreadyExistsException& e) {
        std::cerr << "❌ Email уже зарегистрирован: " << email << std::endl;
        return false;
    } catch (const std::invalid_argument& e) {
        std::cerr << "❌ Ошибка валидации при регистрации: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "❌ Неизвестная ошибка регистрации: " << e.what() << std::endl;
        return false;
    }
}

bool AuthController::changePassword(const std::string& clientId, const std::string& oldPassword, 
                                  const std::string& newPassword) {
    try {
        UUID clientUUID = UUID::fromString(clientId);
        return authService_->changePassword(clientUUID, oldPassword, newPassword);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка смены пароля: " << e.what() << std::endl;
        return false;
    }
}

void AuthController::resetPassword(const std::string& email) {
    try {
        authService_->resetPassword(email);
        std::cout << "✅ Запрос на сброс пароля отправлен: " << email << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка сброса пароля: " << e.what() << std::endl;
    }
}