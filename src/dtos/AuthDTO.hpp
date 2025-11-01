#ifndef AUTHDTO_HPP
#define AUTHDTO_HPP

#include "../types/uuid.hpp"
#include <string>

// Pure Data Transfer Objects - только данные, без логики
struct AuthRequestDTO {
    std::string name;
    std::string email;
    std::string phone;
    std::string password;
    
    AuthRequestDTO() = default;
    AuthRequestDTO(const std::string& name, const std::string& email, 
                   const std::string& phone, const std::string& password)
        : name(name), email(email), phone(phone), password(password) {}
    
    AuthRequestDTO(const std::string& email, const std::string& password)
        : email(email), password(password) {}
};

struct AuthResponseDTO {
    UUID clientId;
    std::string name;
    std::string email;
    std::string status;
    
    AuthResponseDTO() = default;
    AuthResponseDTO(const UUID& clientId, const std::string& name, 
                   const std::string& email, const std::string& status)
        : clientId(clientId), name(name), email(email), status(status) {}
};

#endif // AUTHDTO_HPP