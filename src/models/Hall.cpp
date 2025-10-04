#include "Hall.hpp"

Hall::Hall() 
    : id_(0), name_(""), description_(""), capacity_(0), 
      floorType_(""), equipment_(""), branchId_(0) {}

Hall::Hall(int id, std::string name, int capacity, int branchId)
    : id_(id), name_(name), description_(""), capacity_(capacity),
      floorType_("wooden"), equipment_("mirrors"), branchId_(branchId) {}

int Hall::getId() const { return id_; }
std::string Hall::getName() const { return name_; }
std::string Hall::getDescription() const { return description_; }
int Hall::getCapacity() const { return capacity_; }
std::string Hall::getFloorType() const { return floorType_; }
std::string Hall::getEquipment() const { return equipment_; }
int Hall::getBranchId() const { return branchId_; }

void Hall::setDescription(const std::string& description) {
    description_ = description;
}

void Hall::setFloorType(const std::string& floorType) {
    floorType_ = floorType;
}

void Hall::setEquipment(const std::string& equipment) {
    equipment_ = equipment;
}

bool Hall::isValid() const {
    return id_ > 0 && !name_.empty() && capacity_ > 0 && branchId_ > 0;
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