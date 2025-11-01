#ifndef PASSWORDHASHER_HPP
#define PASSWORDHASHER_HPP

#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>

class PasswordHasher {
public:
    static std::string generateSalt();
    static std::string hashPassword(const std::string& password, const std::string& salt);
    static bool verifyPassword(const std::string& password, const std::string& hash, const std::string& salt);
    static std::string generateSecurePasswordHash(const std::string& password);
    static bool verifySecurePassword(const std::string& password, const std::string& hashedPassword);

private:
    static const int SALT_LENGTH = 32;
    static const int HASH_ITERATIONS = 100000;
    static const int HASH_LENGTH = 64;
};

#endif // PASSWORDHASHER_HPP