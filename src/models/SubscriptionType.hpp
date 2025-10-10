#ifndef SUBSCRIPTIONTYPE_HPP
#define SUBSCRIPTIONTYPE_HPP

#include "../types/uuid.hpp"
#include <string>

class SubscriptionType {
private:
    UUID id_;
    std::string name_;
    std::string description_;
    int validityDays_;
    int visitCount_;
    bool unlimited_;
    double price_;

public:
    SubscriptionType();
    SubscriptionType(const UUID& id, const std::string& name, int validityDays, 
                     int visitCount, bool unlimited, double price);
    
    // Геттеры
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    int getValidityDays() const;
    int getVisitCount() const;
    bool isUnlimited() const; 
    double getPrice() const;
    
    // Сеттеры
    void setDescription(const std::string& description);
    
    // Бизнес-логика
    bool isValidForNewSubscription() const;
    int calculateRemainingVisits() const;
    bool isValid() const;
    
    // Валидация
    static bool isValidName(const std::string& name);
    static bool isValidDescription(const std::string& description);
    static bool isValidValidityDays(int days);
    static bool isValidVisitCount(int count);
    static bool isValidPrice(double price);
    
    // Операторы сравнения
    bool operator==(const SubscriptionType& other) const;
    bool operator!=(const SubscriptionType& other) const;
};

#endif // SUBSCRIPTIONTYPE_HPP