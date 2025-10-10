#pragma once
#include "../repositories/IClientRepository.hpp"
#include "../dtos/AuthDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/ValidationException.hpp"
#include "exceptions/AuthException.hpp"
#include <memory>
#include <string>

class AuthService {
private:
    std::unique_ptr<IClientRepository> clientRepository_;
    
    std::string hashPassword(const std::string& password) const;
    bool verifyPassword(const std::string& password, const std::string& hash) const;

public:
    explicit AuthService(std::unique_ptr<IClientRepository> clientRepo);
    
    AuthResponseDTO registerClient(const AuthRequestDTO& request);
    AuthResponseDTO login(const AuthRequestDTO& request);
    bool changePassword(const UUID& clientId, const std::string& oldPassword, const std::string& newPassword);
    void resetPassword(const std::string& email);
    bool validateSession(const UUID& clientId) const;
};