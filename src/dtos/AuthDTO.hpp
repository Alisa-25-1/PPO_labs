#ifndef AUTHDTO_HPP
#define AUTHDTO_HPP

#include "../types/uuid.hpp"
#include <string>

struct AuthRequestDTO {
    std::string email;
    std::string password;
    
    bool validate() const {
        return !email.empty() && !password.empty() && password.length() >= 8;
    }
};

struct AuthResponseDTO {
    UUID clientId;
    std::string name;
    std::string email;
    std::string token; // В реальной системе - JWT токен
    std::string status;
    
    AuthResponseDTO(const UUID& id, const std::string& name, 
                   const std::string& email, const std::string& status)
        : clientId(id), name(name), email(email), status(status) {}
};

#endif // AUTHDTO_HPP