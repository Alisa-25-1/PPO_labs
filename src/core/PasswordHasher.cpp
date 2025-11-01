#include "PasswordHasher.hpp"
#include <stdexcept>

std::string PasswordHasher::generateSalt() {
    unsigned char salt[SALT_LENGTH];
    if (RAND_bytes(salt, sizeof(salt)) != 1) {
        throw std::runtime_error("Failed to generate salt");
    }
    
    std::stringstream ss;
    for (int i = 0; i < SALT_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)salt[i];
    }
    return ss.str();
}

std::string PasswordHasher::hashPassword(const std::string& password, const std::string& salt) {
    unsigned char hash[HASH_LENGTH];
    
    // Конвертируем salt из hex строки в байты
    unsigned char salt_bytes[SALT_LENGTH];
    for (size_t i = 0; i < SALT_LENGTH; i++) {
        salt_bytes[i] = std::stoi(salt.substr(i * 2, 2), nullptr, 16);
    }
    
    if (PKCS5_PBKDF2_HMAC(
        password.c_str(), password.length(),
        salt_bytes, SALT_LENGTH,
        HASH_ITERATIONS,
        EVP_sha512(),
        HASH_LENGTH, hash) != 1) {
        throw std::runtime_error("Failed to hash password");
    }
    
    std::stringstream ss;
    for (int i = 0; i < HASH_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool PasswordHasher::verifyPassword(const std::string& password, const std::string& hash, const std::string& salt) {
    std::string newHash = hashPassword(password, salt);
    return newHash == hash;
}

std::string PasswordHasher::generateSecurePasswordHash(const std::string& password) {
    std::string salt = generateSalt();
    std::string hash = hashPassword(password, salt);
    return salt + ":" + hash; // Формат: salt:hash
}

bool PasswordHasher::verifySecurePassword(const std::string& password, const std::string& hashedPassword) {
    size_t separator = hashedPassword.find(':');
    if (separator == std::string::npos) {
        return false;
    }
    
    std::string salt = hashedPassword.substr(0, separator);
    std::string hash = hashedPassword.substr(separator + 1);
    
    return verifyPassword(password, hash, salt);
}