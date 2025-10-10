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
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid dance hall data");
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
    if (!isValidDescription(description)) {
        throw std::invalid_argument("Invalid description");
    }
    description_ = description;
}

void DanceHall::setFloorType(const std::string& floorType) {
    if (!isValidFloorType(floorType)) {
        throw std::invalid_argument("Invalid floor type");
    }
    floorType_ = floorType;
}

void DanceHall::setEquipment(const std::string& equipment) {
    if (!isValidEquipment(equipment)) {
        throw std::invalid_argument("Invalid equipment");
    }
    equipment_ = equipment;
}

bool DanceHall::isValid() const {
    return !id_.isNull() && id_.isValid() && 
           isValidName(name_) && 
           isValidDescription(description_) && 
           isValidCapacity(capacity_) && 
           isValidFloorType(floorType_) && 
           isValidEquipment(equipment_) && 
           !branchId_.isNull() && branchId_.isValid();
}

bool DanceHall::isValidName(const std::string& name) {
    if (name.empty() || name.length() > 50 || name.length() < 2) {
        return false;
    }
    std::regex validChars(R"(^[a-zA-Zа-яА-Я0-9\s\-_]+$)");
    return std::regex_match(name, validChars);
}

bool DanceHall::isValidDescription(const std::string& description) {
    return description.length() <= 1000;
}

bool DanceHall::isValidCapacity(int capacity) {
    return capacity > 0 && capacity <= 500;
}

bool DanceHall::isValidFloorType(const std::string& floorType) {
    if (floorType.empty() || floorType.length() > 50) {
        return false;
    }
    std::vector<std::string> validTypes = {
        "wooden", "marley", "vinyl", "concrete", "linoleum", "cork"
    };
    return std::find(validTypes.begin(), validTypes.end(), floorType) != validTypes.end();
}

bool DanceHall::isValidEquipment(const std::string& equipment) {
    return equipment.length() <= 500;
}