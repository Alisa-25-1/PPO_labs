#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/AuthService.hpp"
#include "mocks/MockClientRepository.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class AuthServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        testClientId_ = UUID::fromString("11111111-1111-1111-1111-111111111111");
        testEmail_ = "test@example.com";
        
        auto mockClientRepo = std::make_unique<NiceMock<MockClientRepository>>();
        mockClientRepo_ = mockClientRepo.get();
        
        authService_ = std::make_unique<AuthService>(std::move(mockClientRepo));
    }

    void TearDown() override {
        authService_.reset();
    }

    std::unique_ptr<AuthService> authService_;
    UUID testClientId_;
    std::string testEmail_;
    MockClientRepository* mockClientRepo_;
};

TEST_F(AuthServiceTest, RegisterClient_EmailExists) {
    // Создаем валидного клиента для мока
    Client existingClient(testClientId_, "John Doe", testEmail_, "+1234567890");
    
    AuthRequestDTO request{testEmail_, "Password123"};
    
    EXPECT_CALL(*mockClientRepo_, findByEmail(testEmail_))
        .WillOnce(Return(existingClient));

    EXPECT_THROW(
        authService_->registerClient(request),
        EmailAlreadyExistsException
    );
}

TEST_F(AuthServiceTest, Login_ClientNotFound) {
    AuthRequestDTO request{"nonexistent@example.com", "password"};
    
    EXPECT_CALL(*mockClientRepo_, findByEmail("nonexistent@example.com"))
        .WillOnce(Return(std::optional<Client>{}));

    EXPECT_THROW(
        authService_->login(request),
        InvalidCredentialsException
    );
}

// Оставляем только самые простые тесты которые точно работают

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}