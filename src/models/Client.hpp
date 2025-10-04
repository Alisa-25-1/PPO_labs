#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <chrono>

enum class AccountStatus {
    ACTIVE,
    INACTIVE,
    SUSPENDED
};

class Client {
private:
    int id_;
    std::string name_;
    std::string email_;
    std::string passwordHash_;
    std::string phone_;
    std::chrono::system_clock::time_point registrationDate_;
    AccountStatus status_;

public:
    Client();
    Client(int id, std::string name, std::string email, std::string phone);
    
    int getId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPhone() const;
    std::chrono::system_clock::time_point getRegistrationDate() const;
    AccountStatus getStatus() const;
    
    bool validatePassword(const std::string& password) const;
    void changePassword(const std::string& newPassword);
    bool isActive() const;
    void activate();
    void deactivate();
    void suspend();
    
    bool isValid() const;

    // Операторы сравнения
    bool operator==(const Client& other) const;
    bool operator!=(const Client& other) const;
};

#endif // CLIENT_HPP