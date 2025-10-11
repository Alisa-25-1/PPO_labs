#include "AuthDTO.hpp"
#include <regex>

bool AuthRequestDTO::validate() const {
    // Базовая валидация email
    std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    
    // Для входа проверяем только email и пароль
    // Для регистрации будут проверяться все поля в соответствующих методах
    return !email.empty() && 
           !password.empty() && 
           password.length() >= 8 &&
           std::regex_match(email, emailPattern);
}