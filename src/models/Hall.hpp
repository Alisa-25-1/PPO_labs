#ifndef HALL_HPP
#define HALL_HPP

#include "../types/uuid.hpp"
#include <string>

class Hall {
private:
    UUID id_;
    std::string name_;
    std::string description_;
    int capacity_;
    std::string floorType_;
    std::string equipment_;
    UUID branchId_;

public:
    Hall();
    Hall(const UUID& id, const std::string& name, int capacity, const UUID& branchId);
    
    // Геттеры
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    int getCapacity() const;
    std::string getFloorType() const;
    std::string getEquipment() const;
    UUID getBranchId() const;
    
    // Сеттеры с валидацией
    void setDescription(const std::string& description);
    void setFloorType(const std::string& floorType);
    void setEquipment(const std::string& equipment);
    
    // Валидация
    bool isValid() const;
    static bool isValidName(const std::string& name);
    static bool isValidDescription(const std::string& description);
    static bool isValidCapacity(int capacity);
    static bool isValidFloorType(const std::string& floorType);
    static bool isValidEquipment(const std::string& equipment);

    // Операторы сравнения
    bool operator==(const Hall& other) const;
    bool operator!=(const Hall& other) const;
};

#endif // HALL_HPP