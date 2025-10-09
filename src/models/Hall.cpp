#include "Hall.hpp"
#include <regex>
#include <stdexcept>
#include <algorithm>

Hall::Hall() 
    : id_(UUID()), name_(""), description_(""), capacity_(0), 
      floorType_(""), equipment_(""), branchId_(UUID()) {}

Hall::Hall(const UUID& id, const std::string& name, int capacity, const UUID& branchId)
    : id_(id), name_(name), description_(""), capacity_(capacity),
      floorType_("wooden"), equipment_("mirrors"), branchId_(branchId) {
    
    // Валидация при создании
    if (!isValid()) {
        throw std::invalid_argument("Invalid hall data provided");
    }
}

UUID Hall::getId() const { return id_; }
std::string Hall::getName() const { return name_; }
std::string Hall::getDescription() const { return description_; }
int Hall::getCapacity() const { return capacity_; }
std::string Hall::getFloorType() const { return floorType_; }
std::string Hall::getEquipment() const { return equipment_; }
UUID Hall::getBranchId() const { return branchId_; }

void Hall::setDescription(const std::string& description) {
    if (!isValidDescription(description)) {
        throw std::invalid_argument("Invalid description format");
    }
    description_ = description;
}

void Hall::setFloorType(const std::string& floorType) {
    if (!isValidFloorType(floorType)) {
        throw std::invalid_argument("Invalid floor type format");
    }
    floorType_ = floorType;
}

void Hall::setEquipment(const std::string& equipment) {
    if (!isValidEquipment(equipment)) {
        throw std::invalid_argument("Invalid equipment format");
    }
    equipment_ = equipment;
}

bool Hall::isValid() const {
    return !id_.isNull() && id_.isValid() && 
           isValidName(name_) && 
           isValidDescription(description_) && 
           isValidCapacity(capacity_) && 
           isValidFloorType(floorType_) && 
           isValidEquipment(equipment_) && 
           !branchId_.isNull() && branchId_.isValid();
}

bool Hall::isValidName(const std::string& name) {
    // Проверка: не пустое, разумная длина, допустимые символы
    if (name.empty() || name.length() > 50 || name.length() < 2) {
        return false;
    }
    
    // Проверка на допустимые символы
    std::regex validChars(R"(^[a-zA-Zа-яА-Я0-9\s\-_]+$)");
    return std::regex_match(name, validChars);
}

bool Hall::isValidDescription(const std::string& description) {
    // Описание может быть пустым, но если указано - проверить длину
    if (description.length() > 1000) {
        return false;
    }
    
    // Проверка на допустимые символы
    std::regex validChars(R"(^[a-zA-Zа-яА-Я0-9\s\.,!?\-_()]*$)");
    return std::regex_match(description, validChars);
}

bool Hall::isValidCapacity(int capacity) {
    return capacity > 0 && capacity <= 500; // Разумные пределы для танцевального зала
}

bool Hall::isValidFloorType(const std::string& floorType) {
    // Проверка: не пустое, разумная длина
    if (floorType.empty() || floorType.length() > 50) {
        return false;
    }
    
    // Допустимые типы покрытия
    static const std::vector<std::string> validTypes = {
        "wooden", "marley", "vinyl", "concrete", "linoleum", "cork"
    };
    
    return std::find(validTypes.begin(), validTypes.end(), floorType) != validTypes.end();
}

bool Hall::isValidEquipment(const std::string& equipment) {
    // Оборудование может быть пустым, но если указано - проверить длину
    if (equipment.length() > 500) {
        return false;
    }
    
    // Проверка на допустимые символы
    std::regex validChars(R"(^[a-zA-Zа-яА-Я0-9\s\.,\-_]*$)");
    return std::regex_match(equipment, validChars);
}

// Операторы сравнения
bool Hall::operator==(const Hall& other) const {
    return id_ == other.id_ && 
           name_ == other.name_ && 
           capacity_ == other.capacity_ && 
           branchId_ == other.branchId_;
}

bool Hall::operator!=(const Hall& other) const {
    return !(*this == other);
}