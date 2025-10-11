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
    // Разрешаем пустое имя при регистрации - ИЗМЕНИМ ЭТО
    if (name.empty()) {
        return false; // Теперь имя не может быть пустым
    }
    
    if (name.length() > 100 || name.length() < 2) {
        return false;
    }
    
    // Проверка на допустимые символы (буквы, пробелы, апострофы, дефисы)
    std::regex validChars(R"(^[a-zA-Zа-яА-Я\s\-\']+$)");
    if (!std::regex_match(name, validChars)) {
        return false;
    }
    
    // Проверка на наличие хотя бы одной буквы
    bool hasLetter = std::any_of(name.begin(), name.end(), ::isalpha);
    if (!hasLetter) {
        return false;
    }
    
    return true;
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

bool Client::isValidPhone(const std::string& phone) {
    // Разрешаем пустой телефон при регистрации
    if (phone.empty()) {
        return true;
    }
    
    if (phone.length() < 10 || phone.length() > 20) {
        return false;
    }
    
    std::string cleanPhone;
    for (size_t i = 0; i < phone.length(); ++i) {
        char c = phone[i];
        if (i == 0 && c == '+') {
            cleanPhone += c;
        } else if (std::isdigit(c)) {
            cleanPhone += c;
        }
    }
    
    if (cleanPhone.empty() || 
        (cleanPhone[0] == '+' && cleanPhone.length() < 11) ||
        (cleanPhone[0] != '+' && cleanPhone.length() < 10)) {
        return false;
    }
    
    std::regex phonePattern(R"(^\+?[0-9]{10,15}$)");
    return std::regex_match(cleanPhone, phonePattern);
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