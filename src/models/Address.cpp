#include "Address.hpp"
#include <regex>
#include <stdexcept>
#include <map>

// Карта смещений для основных городов России (в минутах от UTC)
static const std::map<std::string, std::chrono::minutes> CITY_TIMEZONES = {
    {"Москва", std::chrono::hours(3)},
    {"Санкт-Петербург", std::chrono::hours(3)},
    {"Новосибирск", std::chrono::hours(7)},
    {"Екатеринбург", std::chrono::hours(5)},
    {"Казань", std::chrono::hours(3)},
    {"Нижний Новгород", std::chrono::hours(3)},
    {"Краснодар", std::chrono::hours(3)},
    {"Сочи", std::chrono::hours(3)},
    {"Владивосток", std::chrono::hours(10)},
    {"default", std::chrono::hours(3)} // UTC+3 по умолчанию для России
};

BranchAddress::BranchAddress() 
    : id_(UUID()), country_("Россия"), city_(""), street_(""), building_(""),
      apartment_(""), postalCode_(""), timezoneOffset_(std::chrono::hours(3)) {}

BranchAddress::BranchAddress(const UUID& id, const std::string& country, const std::string& city,
                 const std::string& street, const std::string& building,
                 const std::chrono::minutes& timezoneOffset)
    : id_(id), country_(country), city_(city), street_(street), building_(building),
      apartment_(""), postalCode_(""), timezoneOffset_(timezoneOffset) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid address data");
    }
}

UUID BranchAddress::getId() const { return id_; }
std::string BranchAddress::getCountry() const { return country_; }
std::string BranchAddress::getCity() const { return city_; }
std::string BranchAddress::getStreet() const { return street_; }
std::string BranchAddress::getBuilding() const { return building_; }
std::string BranchAddress::getApartment() const { return apartment_; }
std::string BranchAddress::getPostalCode() const { return postalCode_; }
std::chrono::minutes BranchAddress::getTimezoneOffset() const { return timezoneOffset_; }

std::string BranchAddress::getFullAddress() const {
    std::string result = country_ + ", " + city_ + ", " + street_ + ", " + building_;
    if (!apartment_.empty()) {
        result += ", кв. " + apartment_;
    }
    if (!postalCode_.empty()) {
        result += ", " + postalCode_;
    }
    return result;
}

void BranchAddress::setApartment(const std::string& apartment) {
    apartment_ = apartment;
}

void BranchAddress::setPostalCode(const std::string& postalCode) {
    if (!isValidPostalCode(postalCode)) {
        throw std::invalid_argument("Invalid postal code");
    }
    postalCode_ = postalCode;
}

bool BranchAddress::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           isValidCountry(country_) &&
           isValidCity(city_) &&
           isValidStreet(street_) &&
           isValidBuilding(building_) &&
           isValidTimezoneOffset(timezoneOffset_);
}

bool BranchAddress::isValidCountry(const std::string& country) {
    return !country.empty() && country.length() <= 50;
}

bool BranchAddress::isValidCity(const std::string& city) {
    return !city.empty() && city.length() <= 50;
}

bool BranchAddress::isValidStreet(const std::string& street) {
    return !street.empty() && street.length() <= 100;
}

bool BranchAddress::isValidBuilding(const std::string& building) {
    return !building.empty() && building.length() <= 10;
}

bool BranchAddress::isValidTimezoneOffset(const std::chrono::minutes& offset) {
    // Допустимые смещения от UTC-12 до UTC+14
    return offset >= std::chrono::hours(-12) && offset <= std::chrono::hours(14);
}

bool BranchAddress::isValidPostalCode(const std::string& postalCode) {
    std::regex postalCodePattern(R"(^\d{6}$)");
    return postalCode.empty() || std::regex_match(postalCode, postalCodePattern);
}

std::chrono::minutes BranchAddress::getTimezoneOffsetForCity(const std::string& city) {
    auto it = CITY_TIMEZONES.find(city);
    if (it != CITY_TIMEZONES.end()) {
        return it->second;
    }
    return CITY_TIMEZONES.at("default");
}