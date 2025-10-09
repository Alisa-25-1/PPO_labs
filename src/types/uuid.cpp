#include "uuid.hpp"
#include <regex>
#include <stdexcept>
#include <sstream>
#include <iomanip>

UUID::UUID() : value_("00000000-0000-0000-0000-000000000000") {}

UUID::UUID(const std::string& uuid) : value_(uuid) {
    if (!isValidUUIDFormat(uuid) && uuid != "00000000-0000-0000-0000-000000000000") {
        throw std::invalid_argument("Invalid UUID format: " + uuid);
    }
}

UUID UUID::generate() {
#ifdef _WIN32
    // Реализация для Windows с использованием UuidCreate
    UUID uuid;
    UuidCreate((UUID*)&uuid);
    
    // Преобразуем UUID в строку
    RPC_CSTR str;
    UuidToStringA((UUID*)&uuid, &str);
    std::string result((char*)str);
    RpcStringFreeA(&str);
    
    return UUID(result);
#else
    // Реализация для Linux с использованием libuuid
    uuid_t native_uuid;
    uuid_generate(native_uuid);
    
    char str[37]; // 36 символов + null terminator
    uuid_unparse(native_uuid, str);
    
    return UUID(std::string(str));
#endif
}

UUID UUID::fromString(const std::string& str) {
    return UUID(str);
}

const std::string& UUID::toString() const {
    return value_;
}

bool UUID::isValid() const {
    return isValidUUIDFormat(value_);
}

bool UUID::isValidUUIDFormat(const std::string& str) {
    static const std::regex uuidRegex(
        "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"
    );
    return std::regex_match(str, uuidRegex);
}

bool UUID::isUUIDv4(const std::string& str) {
    if (!isValidUUIDFormat(str)) {
        return false;
    }
    
    // Проверка версии (4-й бит 4-й группы должен быть '4')
    return str[14] == '4' && (str[19] == '8' || str[19] == '9' || str[19] == 'a' || str[19] == 'b');
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