#include "Trainer.hpp"
#include <regex>
#include <stdexcept>
#include <algorithm>

Trainer::Trainer() 
    : id_(UUID()), name_(""), biography_(""), qualificationLevel_(""), isActive_(true) {}

Trainer::Trainer(const UUID& id, const std::string& name, const std::vector<std::string>& specializations)
    : id_(id), name_(name), specializations_(specializations), 
      biography_(""), qualificationLevel_(""), isActive_(true) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid trainer data");
    }
}

UUID Trainer::getId() const { return id_; }
std::string Trainer::getName() const { return name_; }
std::vector<std::string> Trainer::getSpecializations() const { return specializations_; }
std::string Trainer::getBiography() const { return biography_; }
std::string Trainer::getQualificationLevel() const { return qualificationLevel_; }
bool Trainer::isActive() const { return isActive_; }

void Trainer::setBiography(const std::string& biography) {
    if (!isValidBiography(biography)) {
        throw std::invalid_argument("Invalid biography");
    }
    biography_ = biography;
}

void Trainer::setQualificationLevel(const std::string& level) {
    if (!isValidQualificationLevel(level)) {
        throw std::invalid_argument("Invalid qualification level");
    }
    qualificationLevel_ = level;
}

void Trainer::setActive(bool active) {
    isActive_ = active;
}

void Trainer::addSpecialization(const std::string& specialization) {
    if (isValidSpecialization(specialization) && !hasSpecialization(specialization)) {
        specializations_.push_back(specialization);
    }
}

void Trainer::removeSpecialization(const std::string& specialization) {
    specializations_.erase(
        std::remove(specializations_.begin(), specializations_.end(), specialization),
        specializations_.end()
    );
}

bool Trainer::hasSpecialization(const std::string& specialization) const {
    return std::find(specializations_.begin(), specializations_.end(), specialization) 
           != specializations_.end();
}

bool Trainer::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           isValidName(name_) &&
           std::all_of(specializations_.begin(), specializations_.end(), isValidSpecialization) &&
           isValidBiography(biography_) &&
           isValidQualificationLevel(qualificationLevel_);
}

bool Trainer::isValidName(const std::string& name) {
    if (name.empty() || name.length() > 100 || name.length() < 2) {
        return false;
    }
    std::regex validChars(R"(^[a-zA-Zа-яА-Я\s\-\']+$)");
    return std::regex_match(name, validChars);
}

bool Trainer::isValidSpecialization(const std::string& specialization) {
    return !specialization.empty() && specialization.length() <= 50;
}

bool Trainer::isValidBiography(const std::string& biography) {
    return biography.length() <= 2000;
}

bool Trainer::isValidQualificationLevel(const std::string& level) {
    std::vector<std::string> validLevels = {
        "junior", "middle", "senior", "master"
    };
    return std::find(validLevels.begin(), validLevels.end(), level) != validLevels.end();
}