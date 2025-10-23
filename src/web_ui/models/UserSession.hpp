#pragma once

#include "../../types/uuid.hpp"
#include <string>
#include <chrono>

enum class UserRole {
    GUEST,
    CLIENT,
    ADMIN
};

class UserSession {
private:
    UUID userId_;
    std::string userName_;
    std::string userEmail_;
    UserRole role_;
    bool isAuthenticated_;
    std::chrono::system_clock::time_point loginTime_;

public:
    UserSession();
    
    void login(const UUID& userId, const std::string& name, 
               const std::string& email, UserRole role);
    void logout();
    
    bool isAuthenticated() const { return isAuthenticated_; }
    UserRole getRole() const { return role_; }
    UUID getUserId() const { return userId_; }
    std::string getUserName() const { return userName_; }
    std::string getUserEmail() const { return userEmail_; }
    
    bool isClient() const { return role_ == UserRole::CLIENT; }
    bool isAdmin() const { return role_ == UserRole::ADMIN; }
};