#pragma once

#include "../../services/AuthService.hpp"
#include "../../dtos/AuthDTO.hpp"
#include "../../models/Client.hpp"
#include <memory>

class AuthController {
private:
    std::unique_ptr<AuthService> authService_;

public:
    AuthController();
    
    bool login(const std::string& email, const std::string& password, AuthResponseDTO& response);
    bool registerClient(const std::string& name, const std::string& email,
                       const std::string& phone, const std::string& password,
                       AuthResponseDTO& response);
    
private:
    void initializeServices();
    bool validateEmail(const std::string& email) const;
    bool validatePassword(const std::string& password) const;
};