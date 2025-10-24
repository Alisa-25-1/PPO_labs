#include "Client.hpp"
#include <chrono>
#include <regex>
#include <stdexcept>
#include <algorithm>

Client::Client() 
    : id_(UUID()), name_(""), email_(""), passwordHash_(""), phone_(""), 
      registrationDate_(std::chrono::system_clock::now()), 
      status_(AccountStatus::INACTIVE) {}

Client::Client(const UUID& id, const std::string& name, const std::string& email, const std::string& phone)
    : id_(id), name_(name), email_(email), passwordHash_(""), phone_(phone),
      registrationDate_(std::chrono::system_clock::now()), 
      status_(AccountStatus::ACTIVE) {
    
    // Валидация при создании
    if (!isValid()) {
        throw std::invalid_argument("Invalid client data provided");
    }
}

UUID Client::getId() const { return id_; }
std::string Client::getName() const { return name_; }
std::string Client::getEmail() const { return email_; }
std::string Client::getPhone() const { return phone_; }
std::string Client::getPasswordHash() const { return passwordHash_; }
std::chrono::system_clock::time_point Client::getRegistrationDate() const { return registrationDate_; }
AccountStatus Client::getStatus() const { return status_; }

bool Client::validatePassword(const std::string& password) const {
    // Прямое сравнение паролей без хэширования
    return !password.empty() && passwordHash_ == password;
}

void Client::changePassword(const std::string& newPassword) {
    if (!isValidPassword(newPassword)) {
        throw std::invalid_argument("Invalid password format");
    }
    // Сохраняем пароль напрямую без хэширования
    passwordHash_ = newPassword;
}

bool Client::isActive() const {
    return status_ == AccountStatus::ACTIVE;
}

void Client::activate() {
    status_ = AccountStatus::ACTIVE;
}

void Client::deactivate() {
    status_ = AccountStatus::INACTIVE;
}

void Client::suspend() {
    status_ = AccountStatus::SUSPENDED;
}

bool Client::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           isValidName(name_) && 
           isValidEmail(email_) && 
           isValidPhone(phone_) &&
           registrationDate_ <= std::chrono::system_clock::now();
}

bool Client::isValidName(const std::string& name) {
    // Минимальная проверка: не пустое и разумная длина
    if (name.empty() || name.length() < 2 || name.length() > 100) {
        return false;
    }

    // Проверка на наличие только пробелов
    bool hasNonSpace = false;
    for (char c : name) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            hasNonSpace = true;
            break;
        }
    }
    if (!hasNonSpace) {
        return false;
    }

    const std::string forbiddenChars = "<>&\"';=()[]{}|\\`~!@#$%^*+,?/:\n\r\t";
    
    for (char c : name) {
        // Запрещаем управляющие символы (ASCII < 32, кроме пробела, табуляции, перевода строки)
        if (static_cast<unsigned char>(c) < 32 && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            return false;
        }
        
        // Запрещаем опасные символы из нашего списка
        if (forbiddenChars.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

bool Client::isValidPhone(const std::string& phone) {
    // Разрешаем пустой телефон при регистрации
    if (phone.empty()) {
        return true;
    }
    
    // Упрощенная проверка для российских номеров
    std::string cleanPhone;
    for (size_t i = 0; i < phone.length(); ++i) {
        char c = phone[i];
        if (i == 0 && c == '+') {
            cleanPhone += c;
        } else if (std::isdigit(c)) {
            cleanPhone += c;
        }
    }
    
    // Проверка форматов российских номеров
    if (cleanPhone.empty()) {
        return false;
    }
    
    // +7XXXXXXXXXX (12 символов)
    if (cleanPhone.substr(0, 2) == "+7") {
        return cleanPhone.length() == 12;
    }
    // 8XXXXXXXXXX (11 символов)
    else if (cleanPhone[0] == '8') {
        return cleanPhone.length() == 11;
    }
    // 7XXXXXXXXXX (11 символов - без +)
    else if (cleanPhone[0] == '7') {
        return cleanPhone.length() == 11;
    }
    
    return false;
}

bool Client::isValidEmail(const std::string& email) {
    // Проверка базовой структуры email
    if (email.empty() || email.length() > 254) { // RFC 5321
        return false;
    }
    
    // Регулярное выражение для проверки email
    std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    
    if (!std::regex_match(email, emailPattern)) {
        return false;
    }
    
    // Проверка на наличие домена
    size_t atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0 || atPos == email.length() - 1) {
        return false;
    }
    
    // Проверка доменной части
    std::string domain = email.substr(atPos + 1);
    if (domain.find('.') == std::string::npos) {
        return false;
    }
    
    return true;
}

bool Client::isValidPassword(const std::string& password) {
    // Минимальные требования к паролю
    if (password.length() < 8) {
        return false;
    }
    
    bool hasUpper = false, hasLower = false, hasDigit = false;
    
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
    }
    
    return hasUpper && hasLower && hasDigit;
}

// Операторы сравнения
bool Client::operator==(const Client& other) const {
    return id_ == other.id_ && 
           name_ == other.name_ && 
           email_ == other.email_ && 
           phone_ == other.phone_;
}

bool Client::operator!=(const Client& other) const {
    return !(*this == other);
}