#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/AuthService.hpp"
#include "mocks/MockClientRepository.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::Truly;

class AuthServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        testClientId_ = UUID::fromString("11111111-1111-1111-1111-111111111111");
        testEmail_ = "test@example.com";
        testPassword_ = "password123";
        
        auto mockClientRepo = std::make_unique<NiceMock<MockClientRepository>>();
        mockClientRepo_ = mockClientRepo.get();
        
        authService_ = std::make_unique<AuthService>(std::move(mockClientRepo));
        
        // Создаем тестового клиента
        testClient_ = std::make_unique<Client>(testClientId_, "John Doe", testEmail_, "+1234567890");
        testClient_->changePassword(testPassword_); // Пароль сохраняется напрямую
        testClient_->activate();
    }

    void TearDown() override {
        testClient_.reset();
        authService_.reset();
    }

    std::unique_ptr<AuthService> authService_;
    UUID testClientId_;
    std::string testEmail_;
    std::string testPassword_;
    std::unique_ptr<Client> testClient_;
    MockClientRepository* mockClientRepo_;
};

TEST_F(AuthServiceTest, RegisterClient_Success) {
    AuthRequestDTO request;
    request.name = "New User";
    request.email = "new@example.com";
    request.phone = "+1234567890";
    request.password = "newpassword123";
    
    // Проверяем, что пароль сохраняется напрямую без хэширования
    EXPECT_CALL(*mockClientRepo_, findByEmail("new@example.com"))
        .WillOnce(Return(std::optional<Client>{}));
    EXPECT_CALL(*mockClientRepo_, save(Truly([](const Client& client) {
        return client.getPasswordHash() == "newpassword123"; // Пароль должен быть сохранен как есть
    }))).WillOnce(Return(true));

    auto result = authService_->registerClient(request);

    EXPECT_EQ(result.email, "new@example.com");
    EXPECT_EQ(result.status, "ACTIVE");
}

TEST_F(AuthServiceTest, RegisterClient_EmailExists) {
    AuthRequestDTO request;
    request.name = "John Doe";
    request.email = testEmail_;
    request.phone = "+1234567890";
    request.password = "password123";
    
    EXPECT_CALL(*mockClientRepo_, findByEmail(testEmail_))
        .WillOnce(Return(*testClient_));

    EXPECT_THROW(
        authService_->registerClient(request),
        EmailAlreadyExistsException
    );
}

TEST_F(AuthServiceTest, Login_Success) {
    AuthRequestDTO request;
    request.email = testEmail_;
    request.password = testPassword_; // Правильный пароль
    
    EXPECT_CALL(*mockClientRepo_, findByEmail(testEmail_))
        .WillOnce(Return(*testClient_));

    auto result = authService_->login(request);

    EXPECT_EQ(result.clientId, testClientId_);
    EXPECT_EQ(result.email, testEmail_);
}

TEST_F(AuthServiceTest, Login_InvalidCredentials) {
    AuthRequestDTO request;
    request.email = testEmail_;
    request.password = "wrongpassword"; // Неправильный пароль
    
    EXPECT_CALL(*mockClientRepo_, findByEmail(testEmail_))
        .WillOnce(Return(*testClient_));

    EXPECT_THROW(
        authService_->login(request),
        InvalidCredentialsException
    );
}

TEST_F(AuthServiceTest, Login_ClientNotFound) {
    AuthRequestDTO request;
    request.email = "nonexistent@example.com";
    request.password = "password";
    
    EXPECT_CALL(*mockClientRepo_, findByEmail("nonexistent@example.com"))
        .WillOnce(Return(std::optional<Client>{}));

    EXPECT_THROW(
        authService_->login(request),
        InvalidCredentialsException
    );
}

TEST_F(AuthServiceTest, ChangePassword_Success) {
    std::string oldPassword = "oldpassword";
    std::string newPassword = "newpassword123";
    
    // Настраиваем клиента с текущим паролем
    testClient_->changePassword(oldPassword);
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockClientRepo_, update(Truly([&](const Client& client) {
        return client.getPasswordHash() == newPassword; // Проверяем, что пароль обновлен напрямую
    }))).WillOnce(Return(true));

    bool result = authService_->changePassword(testClientId_, oldPassword, newPassword);
    
    EXPECT_TRUE(result);
}

TEST_F(AuthServiceTest, ChangePassword_WrongOldPassword) {
    std::string oldPassword = "oldpassword";
    std::string wrongOldPassword = "wrongoldpassword";
    std::string newPassword = "newpassword123";
    
    testClient_->changePassword(oldPassword);
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));

    bool result = authService_->changePassword(testClientId_, wrongOldPassword, newPassword);
    
    EXPECT_FALSE(result);
}

TEST_F(AuthServiceTest, ResetPassword_SetsTemporaryPassword) {
    std::string tempPassword = "temp123";
    
    EXPECT_CALL(*mockClientRepo_, findByEmail(testEmail_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockClientRepo_, update(Truly([&](const Client& client) {
        return client.getPasswordHash() == tempPassword; // Проверяем, что установлен временный пароль
    }))).WillOnce(Return(true));

    // Этот метод не бросает исключений при успехе
    EXPECT_NO_THROW(authService_->resetPassword(testEmail_));
}

TEST_F(AuthServiceTest, ValidateSession_ActiveClient) {
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));

    bool result = authService_->validateSession(testClientId_);
    
    EXPECT_TRUE(result);
}

TEST_F(AuthServiceTest, ValidateSession_InactiveClient) {
    auto inactiveClient = Client(testClientId_, "Inactive User", testEmail_, "+1234567890");
    inactiveClient.deactivate();
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(inactiveClient));

    bool result = authService_->validateSession(testClientId_);
    
    EXPECT_FALSE(result);
}

TEST_F(AuthServiceTest, ValidateSession_ClientNotFound) {
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(std::optional<Client>{}));

    bool result = authService_->validateSession(testClientId_);
    
    EXPECT_FALSE(result);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}