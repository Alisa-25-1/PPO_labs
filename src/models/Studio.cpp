#include "Studio.hpp"
#include <regex>
#include <stdexcept>

Studio::Studio() 
    : id_(UUID()), name_(""), description_(""), contactEmail_("") {}

Studio::Studio(const UUID& id, const std::string& name, const std::string& contactEmail)
    : id_(id), name_(name), description_(""), contactEmail_(contactEmail) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid studio data");
    }
}

UUID Studio::getId() const { return id_; }
std::string Studio::getName() const { return name_; }
std::string Studio::getDescription() const { return description_; }
std::string Studio::getContactEmail() const { return contactEmail_; }
std::vector<UUID> Studio::getBranchIds() const { return branchIds_; }

void Studio::setDescription(const std::string& description) {
    if (!isValidDescription(description)) {
        throw std::invalid_argument("Invalid description");
    }
    description_ = description;
}

void Studio::setContactEmail(const std::string& email) {
    if (!isValidEmail(email)) {
        throw std::invalid_argument("Invalid email");
    }
    contactEmail_ = email;
}

void Studio::addBranch(const UUID& branchId) {
    if (!branchId.isNull() && !hasBranch(branchId)) {
        branchIds_.push_back(branchId);
    }
}

void Studio::removeBranch(const UUID& branchId) {
    branchIds_.erase(
        std::remove(branchIds_.begin(), branchIds_.end(), branchId),
        branchIds_.end()
    );
}

bool Studio::hasBranch(const UUID& branchId) const {
    return std::find(branchIds_.begin(), branchIds_.end(), branchId) != branchIds_.end();
}

bool Studio::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           isValidName(name_) && 
           isValidEmail(contactEmail_) &&
           isValidDescription(description_);
}

bool Studio::isValidName(const std::string& name) {
    return !name.empty() && name.length() <= 100 && name.length() >= 2;
}

bool Studio::isValidEmail(const std::string& email) {
    std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return !email.empty() && std::regex_match(email, emailPattern);
}

bool Studio::isValidDescription(const std::string& description) {
    return description.length() <= 1000;
}