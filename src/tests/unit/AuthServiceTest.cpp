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
        testPassword_ = "Password123";
        testPlainPassword_ = "Password123"; 
        
        auto mockClientRepo = std::make_unique<NiceMock<MockClientRepository>>();
        mockClientRepo_ = mockClientRepo.get();
        
        authService_ = std::make_unique<AuthService>(std::move(mockClientRepo));
        
        testClient_ = std::make_unique<Client>(testClientId_, "John Doe", testEmail_, "+74955678903");
        
        PasswordHasher hasher;
        std::string passwordHash = hasher.generateSecurePasswordHash(testPlainPassword_);
        testClient_->setPasswordHash(passwordHash);
        testClient_->activate();
        
        testPasswordHash_ = passwordHash;
    }

    void TearDown() override {
        testClient_.reset();
        authService_.reset();
    }

    std::unique_ptr<AuthService> authService_;
    UUID testClientId_;
    std::string testEmail_;
    std::string testPassword_;
    std::string testPlainPassword_; 
    std::string testPasswordHash_;
    std::unique_ptr<Client> testClient_;
    MockClientRepository* mockClientRepo_;
};

TEST_F(AuthServiceTest, RegisterClient_Success) {
    AuthRequestDTO request;
    request.name = "New User";
    request.email = "new@example.com";
    request.phone = "+74955678907";
    request.password = "NewPassword123";  
    
    EXPECT_CALL(*mockClientRepo_, findByEmail("new@example.com"))
        .WillOnce(Return(std::optional<Client>{}));
    
    EXPECT_CALL(*mockClientRepo_, save(Truly([](const Client& client) { 
        return !client.getPasswordHash().empty(); 
    }))).WillOnce(Return(true));

    auto result = authService_->registerClient(request);

    EXPECT_EQ(result.email, "new@example.com");
    EXPECT_EQ(result.status, "ACTIVE");
}

TEST_F(AuthServiceTest, RegisterClient_EmailExists) {
    AuthRequestDTO request;
    request.name = "John Doe";
    request.email = testEmail_;
    request.phone = "+74955678903";
    request.password = "Password123";
    
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
    request.password = testPlainPassword_; 
    
    EXPECT_CALL(*mockClientRepo_, findByEmail(testEmail_))
        .WillOnce(Return(*testClient_));

    auto result = authService_->login(request);

    EXPECT_EQ(result.clientId, testClientId_);
    EXPECT_EQ(result.email, testEmail_);
}

TEST_F(AuthServiceTest, Login_InvalidCredentials) {
    AuthRequestDTO request;
    request.email = testEmail_;
    request.password = "WrongPass123";
    
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
    request.password = "Password123";
    
    EXPECT_CALL(*mockClientRepo_, findByEmail("nonexistent@example.com"))
        .WillOnce(Return(std::optional<Client>{}));

    EXPECT_THROW(
        authService_->login(request),
        InvalidCredentialsException
    );
}

TEST_F(AuthServiceTest, ChangePassword_Success) {
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockClientRepo_, update(Truly([](const Client& client) {
        return !client.getPasswordHash().empty();
    }))).WillOnce(Return(true));

    // Используем исходный пароль для проверки
    bool result = authService_->changePassword(testClientId_, testPlainPassword_, "NewPassword123");
    
    EXPECT_TRUE(result);
}

TEST_F(AuthServiceTest, ChangePassword_WrongOldPassword) {
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));

    // Используем неверный пароль
    EXPECT_THROW(
        authService_->changePassword(testClientId_, "WrongOldPassword", "NewPassword123"),
        InvalidCredentialsException
    );
}

TEST_F(AuthServiceTest, ResetPassword_SetsTemporaryPassword) {
    EXPECT_CALL(*mockClientRepo_, findByEmail(testEmail_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockClientRepo_, update(Truly([](const Client& client) { 
        return !client.getPasswordHash().empty(); 
    }))).WillOnce(Return(true));

    EXPECT_NO_THROW(authService_->resetPassword(testEmail_));
}

TEST_F(AuthServiceTest, ValidateSession_ActiveClient) {
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));

    bool result = authService_->validateSession(testClientId_);
    
    EXPECT_TRUE(result);
}

TEST_F(AuthServiceTest, ValidateSession_InactiveClient) {
    auto inactiveClient = Client(testClientId_, "Inactive John", "inactive@example.com", "+74955678903");
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

TEST_F(AuthServiceTest, ChangePassword_ClientNotFound) {
    UUID nonExistentId = UUID::generate();
    
    EXPECT_CALL(*mockClientRepo_, findById(nonExistentId))
        .WillOnce(Return(std::optional<Client>{}));

    bool result = authService_->changePassword(nonExistentId, "OldPass", "NewPass");
    
    EXPECT_FALSE(result);
}

TEST_F(AuthServiceTest, ResetPassword_ClientNotFound) {
    EXPECT_CALL(*mockClientRepo_, findByEmail("nonexistent@example.com"))
        .WillOnce(Return(std::optional<Client>{}));

    EXPECT_NO_THROW(authService_->resetPassword("nonexistent@example.com"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}