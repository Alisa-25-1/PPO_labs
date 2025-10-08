#include "uuid.hpp"
#include <regex>

UUID::UUID() : value_("00000000-0000-0000-0000-000000000000") {}

UUID::UUID(const std::string& uuid) : value_(uuid) {}

UUID UUID::generate() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    
    // Генерация UUID v4
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4"; // version 4
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen); // variant
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    
    return UUID(ss.str());
}

UUID UUID::fromString(const std::string& str) {
    return UUID(str);
}

const std::string& UUID::toString() const {
    return value_;
}

bool UUID::isValid() const {
    static const std::regex uuidRegex(
        "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"
    );
    return std::regex_match(value_, uuidRegex);
}

bool UUID::operator==(const UUID& other) const {
    return value_ == other.value_;
}

bool UUID::operator!=(const UUID& other) const {
    return value_ != other.value_;
}

bool UUID::operator<(const UUID& other) const {
    return value_ < other.value_;
}

std::size_t UUID::Hash::operator()(const UUID& uuid) const {
    return std::hash<std::string>{}(uuid.toString());
}