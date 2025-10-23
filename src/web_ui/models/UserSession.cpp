#include "UserSession.hpp"

UserSession::UserSession() 
    : userId_(UUID()), role_(UserRole::GUEST), isAuthenticated_(false) {
}

void UserSession::login(const UUID& userId, const std::string& name, 
                       const std::string& email, UserRole role) {
    userId_ = userId;
    userName_ = name;
    userEmail_ = email;
    role_ = role;
    isAuthenticated_ = true;
    loginTime_ = std::chrono::system_clock::now();
}

void UserSession::logout() {
    userId_ = UUID();
    userName_.clear();
    userEmail_.clear();
    role_ = UserRole::GUEST;
    isAuthenticated_ = false;
}