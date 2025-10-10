#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/SubscriptionService.hpp"
#include "mocks/MockSubscriptionRepository.hpp"
#include "mocks/MockSubscriptionTypeRepository.hpp"
#include "mocks/MockClientRepository.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class SubscriptionServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        testClientId_ = UUID::fromString("11111111-1111-1111-1111-111111111111");
        testSubscriptionTypeId_ = UUID::fromString("22222222-2222-2222-2222-222222222222");
        testUnlimitedSubscriptionTypeId_ = UUID::fromString("44444444-4444-4444-4444-444444444444");
        testSubscriptionId_ = UUID::fromString("33333333-3333-3333-3333-333333333333");
        testUnlimitedSubscriptionId_ = UUID::fromString("55555555-5555-5555-5555-555555555555");
        nonExistentId_ = UUID::fromString("99999999-9999-9999-9999-999999999999");

        auto mockSubscriptionRepo = std::make_unique<NiceMock<MockSubscriptionRepository>>();
        auto mockSubscriptionTypeRepo = std::make_unique<NiceMock<MockSubscriptionTypeRepository>>();
        auto mockClientRepo = std::make_unique<NiceMock<MockClientRepository>>();

        mockSubscriptionRepo_ = mockSubscriptionRepo.get();
        mockSubscriptionTypeRepo_ = mockSubscriptionTypeRepo.get();
        mockClientRepo_ = mockClientRepo.get();

        subscriptionService_ = std::make_unique<SubscriptionService>(
            std::move(mockSubscriptionRepo),
            std::move(mockSubscriptionTypeRepo),
            std::move(mockClientRepo)
        );

        testClient_ = std::make_unique<Client>(testClientId_, "John Doe", "john@example.com", "+1234567890");
        testClient_->activate();
        
        // Обычная подписка с лимитом
        testSubscriptionType_ = std::make_unique<SubscriptionType>(
            testSubscriptionTypeId_, "Monthly", 30, 8, false, 100.0
        );
        
        // Безлимитная подписка
        testUnlimitedSubscriptionType_ = std::make_unique<SubscriptionType>(
            testUnlimitedSubscriptionTypeId_, "Unlimited Monthly", 30, 0, true, 200.0
        );
        
        auto startTime = std::chrono::system_clock::now();
        auto endTime = startTime + std::chrono::hours(24 * 30);
        
        // Обычная подписка
        testSubscription_ = std::make_unique<Subscription>(
            testSubscriptionId_, testClientId_, testSubscriptionTypeId_, 
            startTime, endTime, 8
        );
        
        // Безлимитная подписка
        testUnlimitedSubscription_ = std::make_unique<Subscription>(
            testUnlimitedSubscriptionId_, testClientId_, testUnlimitedSubscriptionTypeId_, 
            startTime, endTime, -1  // -1 для безлимита
        );
    }

    void TearDown() override {
        testUnlimitedSubscription_.reset();
        testSubscription_.reset();
        testUnlimitedSubscriptionType_.reset();
        testSubscriptionType_.reset();
        testClient_.reset();
        subscriptionService_.reset();
    }

    std::unique_ptr<SubscriptionService> subscriptionService_;
    
    UUID testClientId_;
    UUID testSubscriptionTypeId_;
    UUID testUnlimitedSubscriptionTypeId_;
    UUID testSubscriptionId_;
    UUID testUnlimitedSubscriptionId_;
    UUID nonExistentId_;
    
    std::unique_ptr<Client> testClient_;
    std::unique_ptr<SubscriptionType> testSubscriptionType_;
    std::unique_ptr<SubscriptionType> testUnlimitedSubscriptionType_;
    std::unique_ptr<Subscription> testSubscription_;
    std::unique_ptr<Subscription> testUnlimitedSubscription_;

    MockSubscriptionRepository* mockSubscriptionRepo_;
    MockSubscriptionTypeRepository* mockSubscriptionTypeRepo_;
    MockClientRepository* mockClientRepo_;
};

TEST_F(SubscriptionServiceTest, PurchaseSubscription_Success) {
    SubscriptionRequestDTO request{testClientId_, testSubscriptionTypeId_};
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    // Исправлено: ожидаем два вызова findById для subscriptionType
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testSubscriptionTypeId_))
        .Times(2)
        .WillRepeatedly(Return(*testSubscriptionType_));
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(std::vector<Subscription>{}));
    EXPECT_CALL(*mockSubscriptionRepo_, save(_))
        .WillOnce(Return(true));

    auto result = subscriptionService_->purchaseSubscription(request);

    EXPECT_EQ(result.clientId, testClientId_);
    EXPECT_EQ(result.subscriptionTypeId, testSubscriptionTypeId_);
    EXPECT_EQ(result.status, "ACTIVE");
}

TEST_F(SubscriptionServiceTest, PurchaseUnlimitedSubscription_Success) {
    SubscriptionRequestDTO request{testClientId_, testUnlimitedSubscriptionTypeId_};
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    // Исправлено: ожидаем два вызова findById для unlimited subscriptionType
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testUnlimitedSubscriptionTypeId_))
        .Times(2)
        .WillRepeatedly(Return(*testUnlimitedSubscriptionType_));
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(std::vector<Subscription>{}));
    EXPECT_CALL(*mockSubscriptionRepo_, save(_))
        .WillOnce(Return(true));

    auto result = subscriptionService_->purchaseSubscription(request);

    EXPECT_EQ(result.clientId, testClientId_);
    EXPECT_EQ(result.subscriptionTypeId, testUnlimitedSubscriptionTypeId_);
    EXPECT_EQ(result.status, "ACTIVE");
}

TEST_F(SubscriptionServiceTest, PurchaseSubscription_ClientNotFound) {
    SubscriptionRequestDTO request{nonExistentId_, testSubscriptionTypeId_};
    
    EXPECT_CALL(*mockClientRepo_, findById(nonExistentId_))
        .WillOnce(Return(std::optional<Client>{}));

    EXPECT_THROW(
        subscriptionService_->purchaseSubscription(request),
        ValidationException
    );
}

TEST_F(SubscriptionServiceTest, PurchaseSubscription_SubscriptionTypeNotFound) {
    SubscriptionRequestDTO request{testClientId_, nonExistentId_};
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(nonExistentId_))
        .WillOnce(Return(std::optional<SubscriptionType>{}));

    EXPECT_THROW(
        subscriptionService_->purchaseSubscription(request),
        ValidationException
    );
}

TEST_F(SubscriptionServiceTest, PurchaseSubscription_AlreadyHasActiveSubscription) {
    SubscriptionRequestDTO request{testClientId_, testSubscriptionTypeId_};
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testSubscriptionTypeId_))
        .WillOnce(Return(*testSubscriptionType_));
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(std::vector<Subscription>{*testSubscription_}));

    EXPECT_THROW(
        subscriptionService_->purchaseSubscription(request),
        BusinessRuleException
    );
}

TEST_F(SubscriptionServiceTest, CancelSubscription_Success) {
    EXPECT_CALL(*mockSubscriptionRepo_, findById(testSubscriptionId_))
        .WillOnce(Return(*testSubscription_));
    EXPECT_CALL(*mockSubscriptionRepo_, update(_))
        .WillOnce(Return(true));

    auto result = subscriptionService_->cancelSubscription(testSubscriptionId_);

    EXPECT_EQ(result.status, "CANCELLED");
}

TEST_F(SubscriptionServiceTest, GetAvailableSubscriptionTypes_Success) {
    std::vector<SubscriptionType> subscriptionTypes = {*testSubscriptionType_, *testUnlimitedSubscriptionType_};
    
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findAllActive())
        .WillOnce(Return(subscriptionTypes));

    auto result = subscriptionService_->getAvailableSubscriptionTypes();

    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].name, "Monthly");
    EXPECT_EQ(result[1].name, "Unlimited Monthly");
}

TEST_F(SubscriptionServiceTest, CanUseSubscription_WithActiveSubscription) {
    std::vector<Subscription> subscriptions = {*testSubscription_};
    
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(subscriptions));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testSubscriptionTypeId_))
        .WillOnce(Return(*testSubscriptionType_));

    bool result = subscriptionService_->canUseSubscription(testClientId_);
    
    EXPECT_TRUE(result);
}

TEST_F(SubscriptionServiceTest, CanUseSubscription_WithUnlimitedSubscription) {
    std::vector<Subscription> subscriptions = {*testUnlimitedSubscription_};
    
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(subscriptions));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testUnlimitedSubscriptionTypeId_))
        .WillOnce(Return(*testUnlimitedSubscriptionType_));

    bool result = subscriptionService_->canUseSubscription(testClientId_);
    
    EXPECT_TRUE(result);
}

TEST_F(SubscriptionServiceTest, CanUseSubscription_NoActiveSubscription) {
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(std::vector<Subscription>{}));

    bool result = subscriptionService_->canUseSubscription(testClientId_);
    
    EXPECT_FALSE(result);
}

TEST_F(SubscriptionServiceTest, CanUseSubscription_ExpiredSubscription) {
    // Создаем истекшую подписку
    auto startTime = std::chrono::system_clock::now() - std::chrono::hours(24 * 60); // 60 дней назад
    auto endTime = startTime + std::chrono::hours(24 * 30); // истекла 30 дней назад
    Subscription expiredSubscription(
        testSubscriptionId_, testClientId_, testSubscriptionTypeId_, 
        startTime, endTime, 0
    );
    
    std::vector<Subscription> subscriptions = {expiredSubscription};
    
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(subscriptions));

    bool result = subscriptionService_->canUseSubscription(testClientId_);
    
    EXPECT_FALSE(result);
}

TEST_F(SubscriptionServiceTest, GetRemainingVisits_WithActiveSubscription) {
    std::vector<Subscription> subscriptions = {*testSubscription_};
    
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(subscriptions));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testSubscriptionTypeId_))
        .WillOnce(Return(*testSubscriptionType_));

    int result = subscriptionService_->getRemainingVisits(testClientId_);
    
    EXPECT_EQ(result, 8);
}

TEST_F(SubscriptionServiceTest, GetRemainingVisits_WithUnlimitedSubscription) {
    std::vector<Subscription> subscriptions = {*testUnlimitedSubscription_};
    
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(subscriptions));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testUnlimitedSubscriptionTypeId_))
        .WillOnce(Return(*testUnlimitedSubscriptionType_));

    int result = subscriptionService_->getRemainingVisits(testClientId_);
    
    EXPECT_EQ(result, -1); // -1 означает безлимит
}

TEST_F(SubscriptionServiceTest, GetRemainingVisits_MultipleSubscriptions) {
    // Клиент имеет обе подписки - обычную и безлимитную
    std::vector<Subscription> subscriptions = {*testSubscription_, *testUnlimitedSubscription_};
    
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(subscriptions));
    // Ожидаем вызовы для обоих типов подписок
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testSubscriptionTypeId_))
        .WillOnce(Return(*testSubscriptionType_));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testUnlimitedSubscriptionTypeId_))
        .WillOnce(Return(*testUnlimitedSubscriptionType_));

    int result = subscriptionService_->getRemainingVisits(testClientId_);
    
    // При наличии безлимитной подписки результат должен быть -1
    EXPECT_EQ(result, -1);
}

TEST_F(SubscriptionServiceTest, GetRemainingVisits_NoSubscriptions) {
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(std::vector<Subscription>{}));

    int result = subscriptionService_->getRemainingVisits(testClientId_);
    
    EXPECT_EQ(result, 0);
}

TEST_F(SubscriptionServiceTest, GetClientSubscriptions_Success) {
    std::vector<Subscription> subscriptions = {*testSubscription_, *testUnlimitedSubscription_};
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockSubscriptionRepo_, findByClientId(testClientId_))
        .WillOnce(Return(subscriptions));

    auto result = subscriptionService_->getClientSubscriptions(testClientId_);

    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].clientId, testClientId_);
    EXPECT_EQ(result[1].clientId, testClientId_);
}

TEST_F(SubscriptionServiceTest, RenewSubscription_Success) {
    // Создаем истекшую подписку для обновления
    auto startTime = std::chrono::system_clock::now() - std::chrono::hours(24 * 60);
    auto endTime = startTime + std::chrono::hours(24 * 30);
    Subscription expiredSubscription(
        testSubscriptionId_, testClientId_, testSubscriptionTypeId_, 
        startTime, endTime, 0
    );
    
    EXPECT_CALL(*mockSubscriptionRepo_, findById(testSubscriptionId_))
        .WillOnce(Return(expiredSubscription));
    EXPECT_CALL(*mockSubscriptionTypeRepo_, findById(testSubscriptionTypeId_))
        .WillOnce(Return(*testSubscriptionType_));
    EXPECT_CALL(*mockSubscriptionRepo_, update(_))
        .WillOnce(Return(true));

    auto result = subscriptionService_->renewSubscription(testSubscriptionId_);

    EXPECT_EQ(result.clientId, testClientId_);
    EXPECT_EQ(result.subscriptionTypeId, testSubscriptionTypeId_);
}

TEST_F(SubscriptionServiceTest, RenewSubscription_NotExpired) {
    // Попытка обновить активную подписку должна вызвать исключение
    EXPECT_CALL(*mockSubscriptionRepo_, findById(testSubscriptionId_))
        .WillOnce(Return(*testSubscription_));

    EXPECT_THROW(
        subscriptionService_->renewSubscription(testSubscriptionId_),
        BusinessRuleException
    );
}

TEST_F(SubscriptionServiceTest, RenewSubscription_NotFound) {
    EXPECT_CALL(*mockSubscriptionRepo_, findById(nonExistentId_))
        .WillOnce(Return(std::optional<Subscription>{}));

    EXPECT_THROW(
        subscriptionService_->renewSubscription(nonExistentId_),
        std::runtime_error
    );
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}