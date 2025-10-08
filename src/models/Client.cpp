#include "Client.hpp"
#include <chrono>

Client::Client() 
    : id_(UUID()), name_(""), email_(""), passwordHash_(""), phone_(""), 
      registrationDate_(std::chrono::system_clock::now()), 
      status_(AccountStatus::INACTIVE) {}

Client::Client(const UUID& id, const std::string& name, const std::string& email, const std::string& phone)
    : id_(id), name_(name), email_(email), passwordHash_(""), phone_(phone),
      registrationDate_(std::chrono::system_clock::now()), 
      status_(AccountStatus::ACTIVE) {}

UUID Client::getId() const { return id_; }
std::string Client::getName() const { return name_; }
std::string Client::getEmail() const { return email_; }
std::string Client::getPhone() const { return phone_; }
std::chrono::system_clock::time_point Client::getRegistrationDate() const { return registrationDate_; }
AccountStatus Client::getStatus() const { return status_; }

bool Client::validatePassword(const std::string& password) const {
    return !password.empty();
}

void Client::changePassword(const std::string& newPassword) {
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
           !name_.empty() && !email_.empty() && 
           !phone_.empty();
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