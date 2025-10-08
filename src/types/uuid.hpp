#ifndef UUID_HPP
#define UUID_HPP

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

class UUID {
private:
    std::string value_;

public:
    UUID();
    UUID(const std::string& uuid);
    
    static UUID generate();
    static UUID fromString(const std::string& str);
    
    const std::string& toString() const;
    bool isValid() const;
    bool isNull() const { return value_ == "00000000-0000-0000-0000-000000000000"; }
    
    // Операторы сравнения
    bool operator==(const UUID& other) const;
    bool operator!=(const UUID& other) const;
    bool operator<(const UUID& other) const;
    
    // Для использования в unordered контейнерах
    struct Hash {
        std::size_t operator()(const UUID& uuid) const;
    };
};

#endif // UUID_HPP