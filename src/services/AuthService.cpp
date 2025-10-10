#include "AuthService.hpp"
#include <regex>

AuthService::AuthService(std::unique_ptr<IClientRepository> clientRepo)
    : clientRepository_(std::move(clientRepo)) {}

std::string AuthService::hashPassword(const std::string& password) const {
    // В реальной системе здесь должно быть хэширование, например, bcrypt
    // Для примера используем упрощенный вариант (не безопасно!)
    return password; // Заглушка
}

bool AuthService::verifyPassword(const std::string& password, const std::string& hash) const {
    // В реальной системе здесь должно быть проверка хэша
    return password == hash; // Заглушка
}

AuthResponseDTO AuthService::registerClient(const AuthRequestDTO& request) {
    if (!request.validate()) {
        throw ValidationException("Invalid registration data");
    }
    
    // Проверяем, что email еще не зарегистрирован
    auto existingClient = clientRepository_->findByEmail(request.email);
    if (existingClient) {
        throw EmailAlreadyExistsException();
    }
    
    // Создаем нового клиента
    UUID newId = UUID::generate();
    Client client(newId, request.email, request.email, ""); // Имя и телефон пока пустые
    client.changePassword(hashPassword(request.password));
    client.activate();
    
    if (!clientRepository_->save(client)) {
        throw std::runtime_error("Failed to save client");
    }
    
    return AuthResponseDTO(newId, client.getName(), client.getEmail(), "ACTIVE");
}

AuthResponseDTO AuthService::login(const AuthRequestDTO& request) {
    if (!request.validate()) {
        throw ValidationException("Invalid login data");
    }
    
    auto client = clientRepository_->findByEmail(request.email);
    if (!client) {
        throw InvalidCredentialsException();
    }
    
    if (!client->isActive()) {
        throw AccountInactiveException();
    }
    
    if (!verifyPassword(request.password, client->getPasswordHash())) {
        throw InvalidCredentialsException();
    }
    
    return AuthResponseDTO(client->getId(), client->getName(), client->getEmail(), "ACTIVE");
}

bool AuthService::changePassword(const UUID& clientId, const std::string& oldPassword, const std::string& newPassword) {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        return false;
    }
    
    if (!verifyPassword(oldPassword, client->getPasswordHash())) {
        return false;
    }
    
    client->changePassword(hashPassword(newPassword));
    return clientRepository_->update(*client);
}

void AuthService::resetPassword(const std::string& email) {
    auto client = clientRepository_->findByEmail(email);
    if (!client) {
        // В целях безопасности не сообщаем, что email не найден
        return;
    }
    
    // Генерируем временный пароль и отправляем по email
    // В реальной системе здесь должна быть отправка email
    std::string tempPassword = "temp123"; // Временный пароль
    client->changePassword(hashPassword(tempPassword));
    clientRepository_->update(*client);
}

bool AuthService::validateSession(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    return client && client->isActive();
}