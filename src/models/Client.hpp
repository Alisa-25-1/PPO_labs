#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../types/uuid.hpp"
#include <string>
#include <chrono>

enum class AccountStatus {
    ACTIVE,
    INACTIVE,
    SUSPENDED
};

class Client {
private:
    UUID id_;
    std::string name_;
    std::string email_;
    std::string passwordHash_;
    std::string phone_;
    std::chrono::system_clock::time_point registrationDate_;
    AccountStatus status_;

public:
    Client();
    Client(const UUID& id, const std::string& name, const std::string& email, const std::string& phone);
    
    // Геттеры
    UUID getId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPhone() const;
    std::chrono::system_clock::time_point getRegistrationDate() const;
    AccountStatus getStatus() const;
    
    // Бизнес-логика
    bool validatePassword(const std::string& password) const;
    void changePassword(const std::string& newPassword);
    bool isActive() const;
    void activate();
    void deactivate();
    void suspend();
    
    // Валидация
    bool isValid() const;
    static bool isValidName(const std::string& name);
    static bool isValidEmail(const std::string& email);
    static bool isValidPhone(const std::string& phone);
    static bool isValidPassword(const std::string& password);

    // Операторы сравнения
    bool operator==(const Client& other) const;
    bool operator!=(const Client& other) const;
};

#endif // CLIENT_HPP