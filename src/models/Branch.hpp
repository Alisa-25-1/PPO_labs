#ifndef BRANCH_HPP
#define BRANCH_HPP

#include "../types/uuid.hpp"
#include <string>
#include <chrono>

struct WorkingHours {
    std::chrono::hours openTime;
    std::chrono::hours closeTime;
    
    WorkingHours(std::chrono::hours open, std::chrono::hours close)
        : openTime(open), closeTime(close) {}
    
    bool isValid() const {
        return openTime < closeTime;
    }
};

class Branch {
private:
    UUID id_;
    std::string name_;
    std::string address_;
    std::string phone_;
    WorkingHours workingHours_;
    UUID studioId_;

public:
    Branch();
    Branch(const UUID& id, const std::string& name, const std::string& address, 
           const std::string& phone, const WorkingHours& workingHours, const UUID& studioId);
    
    // Геттеры
    UUID getId() const;
    std::string getName() const;
    std::string getAddress() const;
    std::string getPhone() const;
    WorkingHours getWorkingHours() const;
    UUID getStudioId() const;
    
    // Сеттеры
    void setName(const std::string& name);
    void setAddress(const std::string& address);
    void setPhone(const std::string& phone);
    void setWorkingHours(const WorkingHours& hours);
    
    // Валидация
    bool isValid() const;
    static bool isValidName(const std::string& name);
    static bool isValidAddress(const std::string& address);
    static bool isValidPhone(const std::string& phone);
};

#endif // BRANCH_HPP