#include "DanceHall.hpp"
#include <regex>
#include <stdexcept>
#include <algorithm>

DanceHall::DanceHall() 
    : id_(UUID()), name_(""), description_(""), capacity_(0), 
      floorType_(""), equipment_(""), branchId_(UUID()) {}

DanceHall::DanceHall(const UUID& id, const std::string& name, int capacity, const UUID& branchId)
    : id_(id), name_(name), description_(""), capacity_(capacity),
      floorType_("wooden"), equipment_("mirrors"), branchId_(branchId) {
    
    // Убираем строгую валидацию в конструкторе - будем валидировать только при сохранении
    if (name.empty() || capacity <= 0 || branchId.isNull()) {
        throw std::invalid_argument("Invalid dance hall data: name, capacity and branchId are required");
    }
}

UUID DanceHall::getId() const { return id_; }
std::string DanceHall::getName() const { return name_; }
std::string DanceHall::getDescription() const { return description_; }
int DanceHall::getCapacity() const { return capacity_; }
std::string DanceHall::getFloorType() const { return floorType_; }
std::string DanceHall::getEquipment() const { return equipment_; }
UUID DanceHall::getBranchId() const { return branchId_; }

void DanceHall::setDescription(const std::string& description) {
    description_ = description;
}

void DanceHall::setFloorType(const std::string& floorType) {
    // Убираем строгую валидацию - принимаем любой непустой floorType
    if (floorType.empty()) {
        throw std::invalid_argument("Floor type cannot be empty");
    }
    floorType_ = floorType;
}

void DanceHall::setEquipment(const std::string& equipment) {
    equipment_ = equipment;
}

bool DanceHall::isValid() const {
    // Ослабленная валидация - проверяем только обязательные поля
    return !id_.isNull() && 
           !name_.empty() && 
           capacity_ > 0 && 
           !floorType_.empty() && 
           !branchId_.isNull();
}

bool DanceHall::isValidName(const std::string& name) {
    // Более мягкая валидация имени
    return !name.empty() && name.length() <= 100;
}

bool DanceHall::isValidDescription(const std::string& description) {
    return description.length() <= 1000;
}

bool DanceHall::isValidCapacity(int capacity) {
    return capacity > 0 && capacity <= 500;
}

bool DanceHall::isValidFloorType(const std::string& floorType) {
    // Принимаем любой непустой тип покрытия
    return !floorType.empty() && floorType.length() <= 50;
}

bool DanceHall::isValidEquipment(const std::string& equipment) {
    return equipment.length() <= 500;
}