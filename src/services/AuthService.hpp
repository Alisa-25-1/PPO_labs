#ifndef AUTHSERVICE_HPP
#define AUTHSERVICE_HPP

#include "../repositories/IClientRepository.hpp"
#include "../dtos/AuthDTO.hpp"
#include "../types/uuid.hpp"
#include "../services/exceptions/AuthException.hpp"
#include "../core/PasswordHasher.hpp"
#include <memory>
#include <string>

class AuthService {
private:
    std::shared_ptr<IClientRepository> clientRepository_;
    std::unique_ptr<PasswordHasher> passwordHasher_;

public:
    explicit AuthService(std::shared_ptr<IClientRepository> clientRepo);
    
    // Основные методы аутентификации
    AuthResponseDTO registerClient(const AuthRequestDTO& request);
    AuthResponseDTO login(const AuthRequestDTO& request);
    bool changePassword(const UUID& clientId, const std::string& oldPassword, const std::string& newPassword);
    void resetPassword(const std::string& email);
    bool validateSession(const UUID& clientId) const;
    
private:
    std::string generateSecureTemporaryPassword() const;
    void validateRegistrationData(const AuthRequestDTO& request) const;
    bool isEmailAvailable(const std::string& email) const;
};

#endif // AUTHSERVICE_HPP