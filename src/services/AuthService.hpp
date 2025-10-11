#ifndef AUTHSERVICE_HPP
#define AUTHSERVICE_HPP

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

public:
    explicit AuthService(std::unique_ptr<IClientRepository> clientRepo);
    
    AuthResponseDTO registerClient(const AuthRequestDTO& request);
    AuthResponseDTO login(const AuthRequestDTO& request);
    bool changePassword(const UUID& clientId, const std::string& oldPassword, const std::string& newPassword);
    void resetPassword(const std::string& email);
    bool validateSession(const UUID& clientId) const;
};

#endif // AUTHSERVICE_HPP