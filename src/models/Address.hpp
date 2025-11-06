#pragma once

#include "../types/uuid.hpp"
#include <string>
#include <chrono>

class BranchAddress {
private:
    UUID id_;
    std::string country_;
    std::string city_;
    std::string street_;
    std::string building_;
    std::string apartment_;
    std::string postalCode_;
    std::chrono::minutes timezoneOffset_; // Смещение от UTC в минутах

public:
    BranchAddress();
    BranchAddress(const UUID& id, const std::string& country, const std::string& city, 
            const std::string& street, const std::string& building,
            const std::chrono::minutes& timezoneOffset);
    
    // Геттеры
    UUID getId() const;
    std::string getCountry() const;
    std::string getCity() const;
    std::string getStreet() const;
    std::string getBuilding() const;
    std::string getApartment() const;
    std::string getPostalCode() const;
    std::chrono::minutes getTimezoneOffset() const;
    std::string getFullAddress() const;
    
    // Сеттеры
    void setApartment(const std::string& apartment);
    void setPostalCode(const std::string& postalCode);
    
    // Валидация
    bool isValid() const;
    static bool isValidCountry(const std::string& country);
    static bool isValidCity(const std::string& city);
    static bool isValidStreet(const std::string& street);
    static bool isValidBuilding(const std::string& building);
    static bool isValidTimezoneOffset(const std::chrono::minutes& offset);
    bool isValidPostalCode(const std::string& postalCode);
    
    // Вспомогательные методы
    static std::chrono::minutes getTimezoneOffsetForCity(const std::string& city);
};