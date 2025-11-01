#ifndef AUTHRESULTDTO_HPP
#define AUTHRESULTDTO_HPP

#include "AuthDTO.hpp"
#include <string>

struct AuthResult {
    bool success;
    AuthResponseDTO response;
    std::string errorMessage;
    
    AuthResult(bool success, const AuthResponseDTO& response, const std::string& errorMessage)
        : success(success), response(response), errorMessage(errorMessage) {}
};

#endif // AUTHRESULTDTO_HPP