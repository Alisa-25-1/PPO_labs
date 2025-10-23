#ifndef AUTHDTO_HPP
#define AUTHDTO_HPP

#include "../types/uuid.hpp"
#include <string>

struct AuthRequestDTO {
    std::string name;
    std::string email;
    std::string phone;
    std::string password;
    
    // Конструктор по умолчанию
    AuthRequestDTO() = default;
    
    // Конструктор для регистрации
    AuthRequestDTO(const std::string& name, const std::string& email, 
                   const std::string& phone, const std::string& password)
        : name(name), email(email), phone(phone), password(password) {}
    
    // Конструктор для входа
    AuthRequestDTO(const std::string& email, const std::string& password)
        : email(email), password(password) {}
    
    bool validate() const;
};

struct AuthResponseDTO {
    UUID clientId;
    std::string name;
    std::string email;
    std::string token;
    std::string status;
    
    AuthResponseDTO() = default;
    AuthResponseDTO(const UUID& id, const std::string& name, 
                   const std::string& email, const std::string& status)
        : clientId(id), name(name), email(email), status(status) {}
};

#endif // AUTHDTO_HPP