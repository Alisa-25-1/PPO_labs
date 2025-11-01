#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include <string>
#include <chrono>

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
    
    // Геттеры (inline определения)
    UUID getId() const { return id_; }
    std::string getName() const { return name_; }
    std::string getEmail() const { return email_; }
    std::string getPhone() const { return phone_; }
    std::string getPasswordHash() const { return passwordHash_; }
    std::chrono::system_clock::time_point getRegistrationDate() const { return registrationDate_; }
    AccountStatus getStatus() const { return status_; }

    // Установка хеша пароля
    void setPasswordHash(const std::string& passwordHash);

    // Методы для работы со статусом
    bool isActive() const;
    void activate();
    void deactivate();
    void suspend();

    // Валидация
    bool isValid() const;
    
    // Статические методы валидации
    static bool isValidName(const std::string& name);
    static bool isValidPhone(const std::string& phone);
    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);

    // Операторы сравнения
    bool operator==(const Client& other) const;
    bool operator!=(const Client& other) const;
};

#endif // CLIENT_HPP