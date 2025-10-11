#include "AuthService.hpp"
#include <regex>

AuthService::AuthService(std::unique_ptr<IClientRepository> clientRepo)
    : clientRepository_(std::move(clientRepo)) {}

AuthResponseDTO AuthService::registerClient(const AuthRequestDTO& request) {
    // Дополнительная валидация для регистрации
    if (request.name.empty()) {
        throw ValidationException("Имя не может быть пустым");
    }
    if (request.phone.empty()) {
        throw ValidationException("Телефон не может быть пустым");
    }
    
    if (!request.validate()) {
        throw ValidationException("Неверные данные для регистрации");
    }
    
    // Проверяем, что email еще не зарегистрирован
    auto existingClient = clientRepository_->findByEmail(request.email);
    if (existingClient) {
        throw EmailAlreadyExistsException();
    }
    
    // Создаем нового клиента
    UUID newId = UUID::generate();
    
    Client client(newId, request.name, request.email, request.phone);
    // Сохраняем пароль напрямую без хэширования
    client.changePassword(request.password);
    client.activate();
    
    if (!clientRepository_->save(client)) {
        throw std::runtime_error("Не удалось сохранить клиента");
    }
    
    return AuthResponseDTO(newId, client.getName(), client.getEmail(), "ACTIVE");
}

AuthResponseDTO AuthService::login(const AuthRequestDTO& request) {
    if (!request.validate()) {
        throw ValidationException("Неверный email или пароль");
    }
    
    auto client = clientRepository_->findByEmail(request.email);
    if (!client) {
        throw InvalidCredentialsException();
    }
    
    if (!client->isActive()) {
        throw AccountInactiveException();
    }
    
    // Прямое сравнение паролей без хэширования
    if (request.password != client->getPasswordHash()) {
        throw InvalidCredentialsException();
    }
    
    return AuthResponseDTO(client->getId(), client->getName(), client->getEmail(), "ACTIVE");
}

bool AuthService::changePassword(const UUID& clientId, const std::string& oldPassword, const std::string& newPassword) {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        return false;
    }
    
    // Прямое сравнение старого пароля
    if (oldPassword != client->getPasswordHash()) {
        return false;
    }
    
    // Сохраняем новый пароль напрямую
    client->changePassword(newPassword);
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
    std::string tempPassword = "TempPass123"; // Валидный временный пароль
    client->changePassword(tempPassword);
    clientRepository_->update(*client);
}

bool AuthService::validateSession(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    return client && client->isActive();
}