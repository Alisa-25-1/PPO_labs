#include "Branch.hpp"
#include <regex>
#include <stdexcept>

Branch::Branch() 
    : id_(UUID()), name_(""), phone_(""), 
      workingHours_(std::chrono::hours(9), std::chrono::hours(18)),
      studioId_(UUID()), address_() {}

Branch::Branch(const UUID& id, const std::string& name, const std::string& phone,
               const WorkingHours& workingHours, const UUID& studioId, const Address& address)
    : id_(id), name_(name), phone_(phone), workingHours_(workingHours),
      studioId_(studioId), address_(address) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid branch data");
    }
}

UUID Branch::getId() const { return id_; }
std::string Branch::getName() const { return name_; }
std::string Branch::getPhone() const { return phone_; }
WorkingHours Branch::getWorkingHours() const { return workingHours_; }
UUID Branch::getStudioId() const { return studioId_; }
Address Branch::getAddress() const { return address_; }
std::chrono::minutes Branch::getTimezoneOffset() const { 
    return address_.getTimezoneOffset(); 
}

void Branch::setName(const std::string& name) {
    if (!isValidName(name)) {
        throw std::invalid_argument("Invalid name");
    }
    name_ = name;
}

void Branch::setPhone(const std::string& phone) {
    if (!isValidPhone(phone)) {
        throw std::invalid_argument("Invalid phone");
    }
    phone_ = phone;
}

void Branch::setWorkingHours(const WorkingHours& hours) {
    if (!hours.isValid()) {
        throw std::invalid_argument("Invalid working hours");
    }
    workingHours_ = hours;
}

void Branch::setAddress(const Address& address) {
    address_ = address;
}

bool Branch::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           isValidName(name_) &&
           isValidPhone(phone_) &&
           workingHours_.isValid() &&
           !studioId_.isNull() && studioId_.isValid() &&
           address_.isValid();  // Валидируем объект Address
}

bool Branch::isValidName(const std::string& name) {
    return !name.empty() && name.length() <= 100 && name.length() >= 2;
}

bool Branch::isValidPhone(const std::string& phone) {
    std::regex phonePattern(R"(^\+?[0-9\s\-\(\)]{10,20}$)");
    return !phone.empty() && std::regex_match(phone, phonePattern);
}