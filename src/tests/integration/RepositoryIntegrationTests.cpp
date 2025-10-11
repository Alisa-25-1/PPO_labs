#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include <vector>

// Модели
#include "../../models/Client.hpp"
#include "../../models/DanceHall.hpp"
#include "../../models/Booking.hpp"
#include "../../models/TimeSlot.hpp"
#include "../../models/Subscription.hpp"
#include "../../models/SubscriptionType.hpp"
#include "../../models/Studio.hpp"
#include "../../models/Branch.hpp"
#include "../../models/Trainer.hpp"
#include "../../models/Lesson.hpp"
#include "../../models/Enrollment.hpp"
#include "../../models/Review.hpp"

// Репозитории
#include "../../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../../repositories/impl/PostgreSQLDanceHallRepository.hpp"
#include "../../repositories/impl/PostgreSQLBookingRepository.hpp"
#include "../../repositories/impl/PostgreSQLSubscriptionRepository.hpp"
#include "../../repositories/impl/PostgreSQLSubscriptionTypeRepository.hpp"
#include "../../repositories/impl/PostgreSQLStudioRepository.hpp"
#include "../../repositories/impl/PostgreSQLBranchRepository.hpp"
#include "../../repositories/impl/PostgreSQLTrainerRepository.hpp"
#include "../../repositories/impl/PostgreSQLLessonRepository.hpp"
#include "../../repositories/impl/PostgreSQLEnrollmentRepository.hpp"
#include "../../repositories/impl/PostgreSQLReviewRepository.hpp"

// Данные
#include "../../data/DatabaseConnection.hpp"

class RepositoryIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    void clearTestData();
    void setupTestData();

    std::shared_ptr<DatabaseConnection> dbConnection;
    
    // Все репозитории
    std::unique_ptr<IClientRepository> clientRepository;
    std::unique_ptr<IDanceHallRepository> hallRepository;
    std::unique_ptr<IBookingRepository> bookingRepository;
    std::unique_ptr<ISubscriptionRepository> subscriptionRepository;
    std::unique_ptr<ISubscriptionTypeRepository> subscriptionTypeRepository;
    std::unique_ptr<IStudioRepository> studioRepository;
    std::unique_ptr<IBranchRepository> branchRepository;
    std::unique_ptr<ITrainerRepository> trainerRepository;
    std::unique_ptr<ILessonRepository> lessonRepository;
    std::unique_ptr<IEnrollmentRepository> enrollmentRepository;
    std::unique_ptr<IReviewRepository> reviewRepository;
};

void RepositoryIntegrationTest::SetUp() {
    // Используем тестовую базу данных
    std::string connectionString = "postgresql://dance_user:dance_password@localhost/test_dance_studio";
    dbConnection = std::make_shared<DatabaseConnection>(connectionString);
    
    // Инициализация всех репозиториев
    clientRepository = std::make_unique<PostgreSQLClientRepository>(dbConnection);
    hallRepository = std::make_unique<PostgreSQLDanceHallRepository>(dbConnection);
    bookingRepository = std::make_unique<PostgreSQLBookingRepository>(dbConnection);
    subscriptionRepository = std::make_unique<PostgreSQLSubscriptionRepository>(dbConnection);
    subscriptionTypeRepository = std::make_unique<PostgreSQLSubscriptionTypeRepository>(dbConnection);
    studioRepository = std::make_unique<PostgreSQLStudioRepository>(dbConnection);
    branchRepository = std::make_unique<PostgreSQLBranchRepository>(dbConnection);
    trainerRepository = std::make_unique<PostgreSQLTrainerRepository>(dbConnection);
    lessonRepository = std::make_unique<PostgreSQLLessonRepository>(dbConnection);
    enrollmentRepository = std::make_unique<PostgreSQLEnrollmentRepository>(dbConnection);
    reviewRepository = std::make_unique<PostgreSQLReviewRepository>(dbConnection);
    
    // Очищаем тестовые данные перед каждым тестом
    clearTestData();
    
    // Создаем тестовые данные
    setupTestData();
}

void RepositoryIntegrationTest::TearDown() {
    clearTestData();
}

void RepositoryIntegrationTest::clearTestData() {
    try {
        auto work = dbConnection->beginTransaction();
        work.exec("DELETE FROM reviews");
        work.exec("DELETE FROM enrollments");
        work.exec("DELETE FROM lessons");
        work.exec("DELETE FROM trainer_specializations");
        work.exec("DELETE FROM trainers");
        work.exec("DELETE FROM bookings");
        work.exec("DELETE FROM subscriptions");
        work.exec("DELETE FROM subscription_types");
        work.exec("DELETE FROM clients");
        work.exec("DELETE FROM dance_halls");
        work.exec("DELETE FROM branches");
        work.exec("DELETE FROM studios");
        dbConnection->commitTransaction(work);
    } catch (...) {
        // Игнорируем ошибки очистки
    }
}

void RepositoryIntegrationTest::setupTestData() {
    auto work = dbConnection->beginTransaction();

    // Создаем студию
    work.exec("INSERT INTO studios (id, name, contact_email) VALUES "
            "('11111111-1111-1111-1111-111111111111', 'Test Studio', 'studio@test.com')");
    
    // Создаем филиал - ИСПРАВЛЕНО: используем целые числа для времени
    work.exec("INSERT INTO branches (id, name, address, phone, open_time, close_time, studio_id) VALUES "
            "('22222222-2222-2222-2222-222222222222', 'Test Branch', 'Test Address', '+1234567890', "
            "9, 22, '11111111-1111-1111-1111-111111111111')");
    
    // Создаем залы
    work.exec("INSERT INTO dance_halls (id, name, description, capacity, floor_type, equipment, branch_id) VALUES "
            "('33333333-3333-3333-3333-333333333333', 'Test Hall 1', 'Test hall for integration tests', "
            "50, 'wooden', 'mirrors, sound system', '22222222-2222-2222-2222-222222222222')");
    
    work.exec("INSERT INTO dance_halls (id, name, description, capacity, floor_type, equipment, branch_id) VALUES "
            "('44444444-4444-4444-4444-444444444444', 'Test Hall 2', 'Another test hall', "
            "30, 'marley', 'mirrors, barre', '22222222-2222-2222-2222-222222222222')");
    
    // Создаем тип абонемента
    work.exec("INSERT INTO subscription_types (id, name, description, validity_days, visit_count, unlimited, price) VALUES "
            "('55555555-5555-5555-5555-555555555555', 'Monthly Test', 'Test subscription type', "
            "30, 8, false, 100.00)");
    
    // Создаем тренера
    work.exec("INSERT INTO trainers (id, name, biography, qualification_level, is_active) VALUES "
            "('66666666-6666-6666-6666-666666666666', 'Test Trainer', 'Test biography', 'senior', true)");
    
    // Добавляем специализации тренера
    work.exec("INSERT INTO trainer_specializations (trainer_id, specialization) VALUES "
            "('66666666-6666-6666-6666-666666666666', 'Ballet')");
    work.exec("INSERT INTO trainer_specializations (trainer_id, specialization) VALUES "
            "('66666666-6666-6666-6666-666666666666', 'Contemporary')");
    
    // Создаем занятие
    work.exec("INSERT INTO lessons (id, type, name, description, start_time, duration_minutes, "
            "difficulty, max_participants, current_participants, price, status, trainer_id, hall_id) VALUES "
            "('77777777-7777-7777-7777-777777777777', 'OPEN_CLASS', 'Test Lesson', 'Test lesson description', "
            "CURRENT_TIMESTAMP + INTERVAL '1 hour', 60, 'BEGINNER', 10, 0, 25.00, 'SCHEDULED', "
            "'66666666-6666-6666-6666-666666666666', '33333333-3333-3333-3333-333333333333')");
    
    dbConnection->commitTransaction(work);
}

// === СУЩЕСТВУЮЩИЕ ТЕСТЫ ===

TEST_F(RepositoryIntegrationTest, ClientRepository_SaveAndFind) {
    // Arrange
    UUID clientId = UUID::generate();
    Client client(clientId, "Integration Test User", "integration@example.com", "+12345678901");
    
    // Act
    bool saveResult = clientRepository->save(client);
    auto foundClient = clientRepository->findById(clientId);
    auto foundByEmail = clientRepository->findByEmail("integration@example.com");
    
    // Assert
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundClient.has_value());
    EXPECT_EQ(foundClient->getId(), clientId);
    EXPECT_EQ(foundClient->getName(), "Integration Test User");
    EXPECT_EQ(foundClient->getEmail(), "integration@example.com");
    
    ASSERT_TRUE(foundByEmail.has_value());
    EXPECT_EQ(foundByEmail->getId(), clientId);
}

TEST_F(RepositoryIntegrationTest, BookingRepository_FullCycle) {
    // Arrange
    UUID clientId = UUID::generate();
    UUID bookingId = UUID::generate();
    UUID hallId = UUID::fromString("33333333-3333-3333-3333-333333333333");
    
    // Создаем клиента
    Client client(clientId, "Booking Test User", "booking@example.com", "+12345678901");
    clientRepository->save(client);
    
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(1);
    TimeSlot timeSlot(startTime, 120);
    Booking booking(bookingId, clientId, hallId, timeSlot, "Integration Test Booking");
    
    // Act & Assert - Сохраняем
    bool saveResult = bookingRepository->save(booking);
    EXPECT_TRUE(saveResult);
    
    // Проверяем сохранение
    auto foundBooking = bookingRepository->findById(bookingId);
    ASSERT_TRUE(foundBooking.has_value());
    EXPECT_EQ(foundBooking->getPurpose(), "Integration Test Booking");
    EXPECT_EQ(foundBooking->getStatus(), BookingStatus::PENDING);
    
    // Act & Assert - Обновляем
    foundBooking->confirm();
    bool updateResult = bookingRepository->update(*foundBooking);
    EXPECT_TRUE(updateResult);
    
    auto updatedBooking = bookingRepository->findById(bookingId);
    ASSERT_TRUE(updatedBooking.has_value());
    EXPECT_EQ(updatedBooking->getStatus(), BookingStatus::CONFIRMED);
    
    // Act & Assert - Удаляем
    bool removeResult = bookingRepository->remove(bookingId);
    EXPECT_TRUE(removeResult);
    
    auto deletedBooking = bookingRepository->findById(bookingId);
    EXPECT_FALSE(deletedBooking.has_value());
}

TEST_F(RepositoryIntegrationTest, SubscriptionRepository_CreateAndFind) {
    // Arrange
    UUID clientId = UUID::generate();
    UUID subscriptionId = UUID::generate();
    UUID subscriptionTypeId = UUID::fromString("55555555-5555-5555-5555-555555555555");
    
    // Создаем клиента
    Client client(clientId, "Subscription Test User", "subscription@example.com", "+12345678901");
    clientRepository->save(client);
    
    auto startDate = std::chrono::system_clock::now();
    auto endDate = startDate + std::chrono::hours(24 * 30); // 30 дней
    Subscription subscription(subscriptionId, clientId, subscriptionTypeId, startDate, endDate, 8);
    
    // Act
    bool saveResult = subscriptionRepository->save(subscription);
    auto foundSubscription = subscriptionRepository->findById(subscriptionId);
    auto clientSubscriptions = subscriptionRepository->findByClientId(clientId);
    
    // Assert
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundSubscription.has_value());
    EXPECT_EQ(foundSubscription->getClientId(), clientId);
    EXPECT_EQ(foundSubscription->getRemainingVisits(), 8);
    EXPECT_EQ(foundSubscription->getStatus(), SubscriptionStatus::ACTIVE);
    
    EXPECT_EQ(clientSubscriptions.size(), 1);
    EXPECT_EQ(clientSubscriptions[0].getId(), subscriptionId);
}

TEST_F(RepositoryIntegrationTest, DanceHallRepository_BasicOperations) {
    // Act
    auto hall1 = hallRepository->findById(UUID::fromString("33333333-3333-3333-3333-333333333333"));
    auto hall2 = hallRepository->findById(UUID::fromString("44444444-4444-4444-4444-444444444444"));
    auto allHalls = hallRepository->findAll();
    
    // Assert
    ASSERT_TRUE(hall1.has_value());
    EXPECT_EQ(hall1->getName(), "Test Hall 1");
    EXPECT_EQ(hall1->getCapacity(), 50);
    
    ASSERT_TRUE(hall2.has_value());
    EXPECT_EQ(hall2->getName(), "Test Hall 2");
    EXPECT_EQ(hall2->getCapacity(), 30);
    
    EXPECT_GE(allHalls.size(), 2);
}

TEST_F(RepositoryIntegrationTest, SubscriptionTypeRepository_CreateAndFind) {
    // Arrange
    UUID subscriptionTypeId = UUID::generate();
    SubscriptionType subscriptionType(subscriptionTypeId, "Test Subscription", 30, 12, false, 150.0);
    subscriptionType.setDescription("Test subscription type for integration tests");
    
    // Act
    bool saveResult = subscriptionTypeRepository->save(subscriptionType);
    auto foundType = subscriptionTypeRepository->findById(subscriptionTypeId);
    auto allActiveTypes = subscriptionTypeRepository->findAllActive();
    auto allTypes = subscriptionTypeRepository->findAll();
    
    // Assert
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundType.has_value());
    EXPECT_EQ(foundType->getName(), "Test Subscription");
    EXPECT_EQ(foundType->getValidityDays(), 30);
    EXPECT_EQ(foundType->getVisitCount(), 12);
    EXPECT_EQ(foundType->getPrice(), 150.0);

    // Проверяем, что наш тип есть в списках
    bool foundInActive = std::any_of(allActiveTypes.begin(), allActiveTypes.end(),
        [subscriptionTypeId](const SubscriptionType& type) {
            return type.getId() == subscriptionTypeId;
        });
    EXPECT_TRUE(foundInActive);
    
    bool foundInAll = std::any_of(allTypes.begin(), allTypes.end(),
        [subscriptionTypeId](const SubscriptionType& type) {
            return type.getId() == subscriptionTypeId;
        });
    EXPECT_TRUE(foundInAll);
    
    // Act & Assert - Обновляем
    subscriptionType.setDescription("Updated description");
    bool updateResult = subscriptionTypeRepository->update(subscriptionType);
    EXPECT_TRUE(updateResult);
    
    auto updatedType = subscriptionTypeRepository->findById(subscriptionTypeId);
    ASSERT_TRUE(updatedType.has_value());
    EXPECT_EQ(updatedType->getDescription(), "Updated description");
    
    // Act & Assert - Удаляем
    bool removeResult = subscriptionTypeRepository->remove(subscriptionTypeId);
    EXPECT_TRUE(removeResult);
    
    auto deletedType = subscriptionTypeRepository->findById(subscriptionTypeId);
    EXPECT_FALSE(deletedType.has_value());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "Running integration tests. Ensure test database is running." << std::endl;
    std::cout << "Connection string: postgresql://dance_user:dance_password@localhost/test_dance_studio" << std::endl;
    
    return RUN_ALL_TESTS();
}