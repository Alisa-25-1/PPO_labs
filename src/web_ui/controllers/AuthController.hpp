#pragma once

#include "../../services/AuthService.hpp"
#include "../../dtos/AuthDTO.hpp"
#include <memory>
#include <string>

class AuthController {
private:
    std::unique_ptr<AuthService> authService_;

public:
    AuthController();
    
    // Основные методы аутентификации
    bool login(const std::string& email, const std::string& password, 
               AuthResponseDTO& response);
    bool registerClient(const std::string& name, const std::string& email,
                       const std::string& phone, const std::string& password,
                       AuthResponseDTO& response);
    bool changePassword(const UUID& clientId, const std::string& oldPassword,
                       const std::string& newPassword);
    void resetPassword(const std::string& email);
    
    // Валидация
    bool validateEmail(const std::string& email) const;
    bool validatePassword(const std::string& password) const;
    bool validatePhone(const std::string& phone) const;
    
private:
    void initializeServices();
};