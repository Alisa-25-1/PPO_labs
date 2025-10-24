#include "AuthController.hpp"
#include "../../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include <regex>
#include <iostream>

AuthController::AuthController() {
    initializeServices();
}

void AuthController::initializeServices() {
    try {
        auto connectionString = "postgresql://dance_user:dance_password@localhost/dance_studio";
        auto dbConnection = std::make_shared<DatabaseConnection>(connectionString);
        
        auto clientRepo = std::make_unique<PostgreSQLClientRepository>(dbConnection);
        authService_ = std::make_unique<AuthService>(std::move(clientRepo));
        
        std::cout << "✅ AuthController инициализирован" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка инициализации AuthController: " << e.what() << std::endl;
        throw;
    }
}

bool AuthController::login(const std::string& email, const std::string& password, 
                          AuthResponseDTO& response) {
    try {
        if (!validateEmail(email) || password.empty()) {
            return false;
        }
        
        AuthRequestDTO request(email, password);
        response = authService_->login(request);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка входа: " << e.what() << std::endl;
        return false;
    }
}

bool AuthController::registerClient(const std::string& name, const std::string& email,
                                   const std::string& phone, const std::string& password,
                                   AuthResponseDTO& response) {
    try {
        // Используем упрощенную валидацию
        if (!validateEmail(email) || !validatePassword(password) || 
            !Client::isValidName(name) || 
            !Client::isValidPhone(phone)) {
            return false;
        }
        
        AuthRequestDTO request(name, email, phone, password);
        response = authService_->registerClient(request);
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка регистрации: " << e.what() << std::endl;
        return false;
    }
}

bool AuthController::validateEmail(const std::string& email) const {
    std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

bool AuthController::validatePassword(const std::string& password) const {
    return password.length() >= 8;
}