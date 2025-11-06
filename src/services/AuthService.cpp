#include "AuthService.hpp"
#include "exceptions/AuthException.hpp"
#include "../models/Client.hpp"
#include <random>
#include <iostream>

AuthService::AuthService(std::shared_ptr<IClientRepository> clientRepo)
    : clientRepository_(std::move(clientRepo)), 
      passwordHasher_(std::make_unique<PasswordHasher>()) {}

AuthResponseDTO AuthService::registerClient(const AuthRequestDTO& request) {
    std::cout << "🔧 AuthService::registerClient - Начало регистрации: " << request.email << std::endl;
    
    // Валидация данных через методы Client
    validateRegistrationData(request);
    
    // Проверяем, что email еще не зарегистрирован
    if (!isEmailAvailable(request.email)) {
        std::cout << "❌ AuthService::registerClient - Email уже занят: " << request.email << std::endl;
        throw EmailAlreadyExistsException();
    }
    
    // Создаем нового клиента (валидация происходит в конструкторе Client)
    UUID newId = UUID::generate();
    
    // Хешируем праоль классическим способом
    std::string passwordHash = passwordHasher_->generateSecurePasswordHash(request.password);
    
    Client client(newId, request.name, request.email, request.phone);
    client.setPasswordHash(passwordHash); // Сохраняем хеш
    
    std::cout << "✅ AuthService::registerClient - Клиент создан, сохраняем в БД..." << std::endl;
    
    if (!clientRepository_->save(client)) {
        throw std::runtime_error("Failed to save client to database");
    }
    
    std::cout << "✅ AuthService::registerClient - Регистрация успешна: " << request.email << std::endl;
    return AuthResponseDTO(newId, client.getName(), client.getEmail(), "ACTIVE");
}

AuthResponseDTO AuthService::login(const AuthRequestDTO& request) {
    std::cout << "🔐 AuthService::login - Попытка входа: " << request.email << std::endl;
    
    if (request.email.empty() || request.password.empty()) {
        throw std::invalid_argument("Email и пароль обязательны");
    }
    
    auto client = clientRepository_->findByEmail(request.email);
    if (!client) {
        std::cout << "❌ AuthService::login - Клиент не найден: " << request.email << std::endl;
        throw InvalidCredentialsException();
    }
    
    if (!client->isActive()) {
        std::cout << "❌ AuthService::login - Аккаунт неактивен: " << request.email << std::endl;
        throw AccountInactiveException();
    }
    
    // Проверяем пароль с испольхованием хеширования
    if (!passwordHasher_->verifySecurePassword(request.password, client->getPasswordHash())) {
        std::cout << "❌ AuthService::login - Неверный пароль для: " << request.email << std::endl;
        throw InvalidCredentialsException();
    }
    
    std::cout << "✅ AuthService::login - Успешный вход: " << request.email << std::endl;
    return AuthResponseDTO(client->getId(), client->getName(), client->getEmail(), "ACTIVE");
}

bool AuthService::changePassword(const UUID& clientId, const std::string& oldPassword, const std::string& newPassword) {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        return false; // Клиент не найден
    }
    
    // Проверяем старый пароль с хешированием
    if (!passwordHasher_->verifySecurePassword(oldPassword, client->getPasswordHash())) {
        throw InvalidCredentialsException(); // Бросаем исключение вместо возврата false
    }
    
    // Проверяем новый пароль
    if (!Client::isValidPassword(newPassword)) {
        throw std::invalid_argument("Новый пароль не соответствует требованиям безопасности");
    }
    
    // Хешируем новый пароль
    std::string newPasswordHash = passwordHasher_->generateSecurePasswordHash(newPassword);
    Client updatedClient = *client;
    updatedClient.setPasswordHash(newPasswordHash);
    
    return clientRepository_->update(updatedClient);
}

void AuthService::resetPassword(const std::string& email) {
    auto client = clientRepository_->findByEmail(email);
    if (!client) {
        return; // Security: не раскрываем информацию о существовании email
    }
    
    std::string tempPassword = generateSecureTemporaryPassword();
    // Хешируем временный пароль
    std::string tempPasswordHash = passwordHasher_->generateSecurePasswordHash(tempPassword);
    
    Client updatedClient = *client;
    updatedClient.setPasswordHash(tempPasswordHash);
    
    if (clientRepository_->update(updatedClient)) {
        // TODO: Реализовать отправку email через отдельный сервис
        std::cout << "✅ AuthService::resetPassword - Пароль сброшен для: " << email << std::endl;
        std::cout << "✅ AuthService::resetPassword - новый пароль: " << tempPassword << std::endl;
    }
}

bool AuthService::validateSession(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    return client && client->isActive();
}

void AuthService::validateRegistrationData(const AuthRequestDTO& request) const {
    std::cout << "🔍 AuthService::validateRegistrationData - Валидация данных регистрации" << std::endl;
    
    if (request.name.empty() || request.email.empty() || request.password.empty()) {
        throw std::invalid_argument("Все поля обязательны для заполнения");
    }
    
    // Используем статические методы валидации из Client
    if (!Client::isValidName(request.name)) {
        std::cout << "❌ AuthService::validateRegistrationData - Невалидное имя: " << request.name << std::endl;
        throw std::invalid_argument("Неверный формат имени");
    }
    
    if (!Client::isValidEmail(request.email)) {
        std::cout << "❌ AuthService::validateRegistrationData - Невалидный email: " << request.email << std::endl;
        throw std::invalid_argument("Неверный формат email");
    }
    
    if (!request.phone.empty() && !Client::isValidPhone(request.phone)) {
        std::cout << "❌ AuthService::validateRegistrationData - Невалидный телефон: " << request.phone << std::endl;
        throw std::invalid_argument("Неверный формат телефона");
    }
    
    if (!Client::isValidPassword(request.password)) {
        std::cout << "❌ AuthService::validateRegistrationData - Невалидный пароль (длина: " << request.password.length() << ")" << std::endl;
        throw std::invalid_argument("Пароль не соответствует требованиям безопасности");
    }
    
    std::cout << "✅ AuthService::validateRegistrationData - Все данные валидны" << std::endl;
}

bool AuthService::isEmailAvailable(const std::string& email) const {
    auto client = clientRepository_->findByEmail(email);
    return !client.has_value();
}

std::string AuthService::generateSecureTemporaryPassword() const {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, chars.size() - 1);
    
    std::string password;
    for (int i = 0; i < 12; ++i) {
        password += chars[distribution(generator)];
    }
    
    return password;
}