#ifndef PASSWORDDTO_HPP
#define PASSWORDDTO_HPP

#include "../types/uuid.hpp"
#include <string>

struct PasswordChangeDTO {
    UUID clientId;
    std::string oldPassword;
    std::string newPassword;
    
    PasswordChangeDTO(const UUID& clientId, const std::string& oldPassword, 
                     const std::string& newPassword)
        : clientId(clientId), oldPassword(oldPassword), newPassword(newPassword) {}
};

struct PasswordResetDTO {
    std::string email;
    std::string token;
    std::string newPassword;
    
    PasswordResetDTO(const std::string& email, const std::string& token,
                    const std::string& newPassword)
        : email(email), token(token), newPassword(newPassword) {}
};

#endif // PASSWORDDTO_HPP