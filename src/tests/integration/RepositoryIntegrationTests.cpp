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

#include "../../data/ResilientDatabaseConnection.hpp"

class RepositoryIntegrationTest : public ::testing::Test {
protected:
    static std::shared_ptr<ResilientDatabaseConnection> dbConnection;
    

    static void SetUpTestSuite() {
        std::cout << "🔧 Setting up resilient database connection for test suite..." << std::endl;
        
        std::string connectionString = "postgresql://dance_user:dance_password@localhost/test_dance_studio";
        dbConnection = std::make_shared<ResilientDatabaseConnection>(connectionString);
        
        dbConnection->setRetryPolicy(5, std::chrono::milliseconds(2000));
    
        // Проверяем подключение
        if (!dbConnection->isConnected()) {
            throw std::runtime_error("Failed to connect to test database");
        }
        
        std::cout << "✅ Resilient database connection ready for testing" << std::endl;
}
    
    static void TearDownTestSuite() {
        std::cout << "🧹 Cleaning up test suite..." << std::endl;
        dbConnection.reset();
        std::cout << "✅ Test suite cleanup complete" << std::endl;
    }

    void SetUp() override {
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
        
        // Очищаем и готовим данные для КАЖДОГО теста
        clearTestData();
        setupTestData();
        
        std::cout << "✅ Test data prepared for: " << ::testing::UnitTest::GetInstance()->current_test_info()->name() << std::endl;
    }

    void TearDown() override {
        clearTestData();
    }

    void clearTestData() {
    try {
        auto work = dbConnection->beginTransaction(); 
        work.exec("TRUNCATE TABLE reviews, enrollments, lessons, trainer_specializations, "
                 "trainers, bookings, subscriptions, subscription_types, "
                 "clients, dance_halls, branches, studios CASCADE");
        dbConnection->commitTransaction(work); 
    } catch (const std::exception& e) {
        std::cerr << "⚠️ Warning: Failed to clear test data: " << e.what() << std::endl;
    }
}
    void setupTestData() {
        try {
            auto work = dbConnection->beginTransaction();

            // Создаем тестовые данные
            work.exec("INSERT INTO studios (id, name, contact_email) VALUES "
                    "('11111111-1111-1111-1111-111111111111', 'Test Studio', 'studio@test.com')");
            
            work.exec("INSERT INTO branches (id, name, address, phone, open_time, close_time, studio_id) VALUES "
                    "('22222222-2222-2222-2222-222222222222', 'Test Branch', 'Test Address', '+1234567890', 9, 22, '11111111-1111-1111-1111-111111111111')");
            
            work.exec("INSERT INTO dance_halls (id, name, description, capacity, floor_type, equipment, branch_id) VALUES "
                    "('33333333-3333-3333-3333-333333333333', 'Test Hall 1', 'Large hall', 50, 'wooden', 'mirrors', '22222222-2222-2222-2222-222222222222'),"
                    "('44444444-4444-4444-4444-444444444444', 'Test Hall 2', 'Small hall', 30, 'marley', 'sound system', '22222222-2222-2222-2222-222222222222')");
            
            work.exec("INSERT INTO subscription_types (id, name, description, validity_days, visit_count, unlimited, price) VALUES "
                    "('55555555-5555-5555-5555-555555555555', 'Monthly', 'Monthly subscription', 30, 8, false, 100.0),"
                    "('66666666-6666-6666-6666-666666666666', 'Unlimited', 'Unlimited monthly', 30, 0, true, 200.0)");
            
            work.exec("INSERT INTO trainers (id, name, biography, qualification_level, is_active) VALUES "
                    "('77777777-7777-7777-7777-777777777777', 'Test Trainer', 'Experienced trainer', 'senior', true)");
            
            work.exec("INSERT INTO trainer_specializations (trainer_id, specialization) VALUES "
                    "('77777777-7777-7777-7777-777777777777', 'Ballet'),"
                    "('77777777-7777-7777-7777-777777777777', 'Contemporary')");
            
            dbConnection->commitTransaction(work);
        } catch (const std::exception& e) {
            std::cerr << "❌ Failed to setup test data: " << e.what() << std::endl;
            throw;
        }
    }


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

// Статическая инициализация
std::shared_ptr<ResilientDatabaseConnection> RepositoryIntegrationTest::dbConnection = nullptr;

TEST_F(RepositoryIntegrationTest, ClientRepository_SaveAndFind) {
    // Arrange
    UUID clientId = UUID::generate();
    Client client(clientId, "Integration Test User", "integration@example.com", "+12345678901");
    
    // Устанавливаем валидный пароль
    client.changePassword("ValidPass123");
    client.activate();
    
    // Act
    bool saveResult = clientRepository->save(client);
    
    // Проверяем, что сохранение прошло успешно
    EXPECT_TRUE(saveResult);
    
    // Ищем клиента по ID
    auto foundClient = clientRepository->findById(clientId);
    
    // Проверяем, что клиент найден
    ASSERT_TRUE(foundClient.has_value());
    EXPECT_EQ(foundClient->getId(), clientId);
    EXPECT_EQ(foundClient->getName(), "Integration Test User");
    EXPECT_EQ(foundClient->getEmail(), "integration@example.com");
    EXPECT_EQ(foundClient->getPhone(), "+12345678901");
    EXPECT_TRUE(foundClient->isActive());
    
    // Проверяем, что пароль сохранился корректно
    EXPECT_TRUE(foundClient->validatePassword("ValidPass123"));
    
    // Ищем клиента по email
    auto foundByEmail = clientRepository->findByEmail("integration@example.com");
    
    // Проверяем, что клиент найден по email
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
    client.changePassword("ValidPass123");
    client.activate();
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
    client.changePassword("ValidPass123");
    client.activate();
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

TEST_F(RepositoryIntegrationTest, StudioRepository_FullCycle) {
    // Arrange
    UUID studioId = UUID::generate();
    Studio studio(studioId, "Integration Test Studio", "integration@studio.com");
    studio.setDescription("Test studio for integration tests");
    
    // Act & Assert - Сохраняем
    bool saveResult = studioRepository->save(studio);
    EXPECT_TRUE(saveResult);
    
    // Проверяем сохранение
    auto foundStudio = studioRepository->findById(studioId);
    ASSERT_TRUE(foundStudio.has_value());
    EXPECT_EQ(foundStudio->getName(), "Integration Test Studio");
    EXPECT_EQ(foundStudio->getContactEmail(), "integration@studio.com");
    
    // Act & Assert - Обновляем
    studio.setDescription("Updated description");
    bool updateResult = studioRepository->update(studio);
    EXPECT_TRUE(updateResult);
    
    auto updatedStudio = studioRepository->findById(studioId);
    ASSERT_TRUE(updatedStudio.has_value());
    EXPECT_EQ(updatedStudio->getDescription(), "Updated description");
    
    // Act & Assert - Удаляем
    bool removeResult = studioRepository->remove(studioId);
    EXPECT_TRUE(removeResult);
    
    auto deletedStudio = studioRepository->findById(studioId);
    EXPECT_FALSE(deletedStudio.has_value());
}

TEST_F(RepositoryIntegrationTest, BranchRepository_FullCycle) {
    // Arrange
    UUID branchId = UUID::generate();
    UUID studioId = UUID::fromString("11111111-1111-1111-1111-111111111111");
    
    WorkingHours workingHours(std::chrono::hours(9), std::chrono::hours(22));
    Branch branch(branchId, "Integration Test Branch", "Test Address", "+1234567890", workingHours, studioId);
    
    // Act & Assert - Сохраняем
    bool saveResult = branchRepository->save(branch);
    EXPECT_TRUE(saveResult);
    
    // Проверяем сохранение
    auto foundBranch = branchRepository->findById(branchId);
    ASSERT_TRUE(foundBranch.has_value());
    EXPECT_EQ(foundBranch->getName(), "Integration Test Branch");
    EXPECT_EQ(foundBranch->getAddress(), "Test Address");
    
    // Проверяем поиск по studio_id
    auto studioBranches = branchRepository->findByStudioId(studioId);
    EXPECT_GE(studioBranches.size(), 1);
    
    // Act & Assert - Обновляем
    branch.setAddress("Updated Address");
    bool updateResult = branchRepository->update(branch);
    EXPECT_TRUE(updateResult);
    
    auto updatedBranch = branchRepository->findById(branchId);
    ASSERT_TRUE(updatedBranch.has_value());
    EXPECT_EQ(updatedBranch->getAddress(), "Updated Address");
    
    // Act & Assert - Удаляем
    bool removeResult = branchRepository->remove(branchId);
    EXPECT_TRUE(removeResult);
    
    auto deletedBranch = branchRepository->findById(branchId);
    EXPECT_FALSE(deletedBranch.has_value());
}

TEST_F(RepositoryIntegrationTest, ReviewRepository_CreateAndFind) {
    // Arrange
    UUID clientId = UUID::generate();
    UUID lessonId = UUID::generate();
    UUID reviewId = UUID::generate();
    
    // Создаем клиента
    Client client(clientId, "Review Test User", "review@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    // Создаем урок
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    Lesson lesson(lessonId, LessonType::OPEN_CLASS, "Test Lesson", startTime, 60, 
                 DifficultyLevel::BEGINNER, 20, 50.0, 
                 UUID::fromString("77777777-7777-7777-7777-777777777777"),
                 UUID::fromString("33333333-3333-3333-3333-333333333333"));
    lessonRepository->save(lesson);
    
    Review review(reviewId, clientId, lessonId, 5, "Excellent lesson!");
    
    // Act
    bool saveResult = reviewRepository->save(review);
    auto foundReview = reviewRepository->findById(reviewId);
    auto clientReviews = reviewRepository->findByClientId(clientId);
    auto lessonReviews = reviewRepository->findByLessonId(lessonId);
    auto clientLessonReview = reviewRepository->findByClientAndLesson(clientId, lessonId);
    
    // Assert
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundReview.has_value());
    EXPECT_EQ(foundReview->getRating(), 5);
    EXPECT_EQ(foundReview->getComment(), "Excellent lesson!");
    EXPECT_EQ(foundReview->getStatus(), ReviewStatus::PENDING_MODERATION);
    
    EXPECT_EQ(clientReviews.size(), 1);
    EXPECT_EQ(lessonReviews.size(), 1);
    ASSERT_TRUE(clientLessonReview.has_value());
    EXPECT_EQ(clientLessonReview->getId(), reviewId);
}

TEST_F(RepositoryIntegrationTest, ReviewRepository_FullCycle) {
    // Arrange
    UUID clientId = UUID::generate();
    UUID lessonId = UUID::generate();
    UUID reviewId = UUID::generate();
    
    // Создаем клиента
    Client client(clientId, "Review Test User", "review@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    // Создаем урок
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    Lesson lesson(lessonId, LessonType::OPEN_CLASS, "Test Lesson", startTime, 60, 
                 DifficultyLevel::BEGINNER, 20, 50.0, 
                 UUID::fromString("77777777-7777-7777-7777-777777777777"),
                 UUID::fromString("33333333-3333-3333-3333-333333333333"));
    lessonRepository->save(lesson);
    
    Review review(reviewId, clientId, lessonId, 5, "Excellent lesson!");
    
    // Act & Assert - Сохраняем
    bool saveResult = reviewRepository->save(review);
    EXPECT_TRUE(saveResult);
    
    // Проверяем сохранение
    auto foundReview = reviewRepository->findById(reviewId);
    ASSERT_TRUE(foundReview.has_value());
    EXPECT_EQ(foundReview->getRating(), 5);
    EXPECT_EQ(foundReview->getComment(), "Excellent lesson!");
    EXPECT_EQ(foundReview->getStatus(), ReviewStatus::PENDING_MODERATION);
    
    // Проверяем поиск по клиенту и уроку
    auto clientReviews = reviewRepository->findByClientId(clientId);
    auto lessonReviews = reviewRepository->findByLessonId(lessonId);
    auto clientLessonReview = reviewRepository->findByClientAndLesson(clientId, lessonId);
    
    EXPECT_EQ(clientReviews.size(), 1);
    EXPECT_EQ(lessonReviews.size(), 1);
    ASSERT_TRUE(clientLessonReview.has_value());
    EXPECT_EQ(clientLessonReview->getId(), reviewId);
    
    // Act & Assert - Обновляем
    foundReview->approve();
    bool updateResult = reviewRepository->update(*foundReview);
    EXPECT_TRUE(updateResult);
    
    auto updatedReview = reviewRepository->findById(reviewId);
    ASSERT_TRUE(updatedReview.has_value());
    EXPECT_EQ(updatedReview->getStatus(), ReviewStatus::APPROVED);
    
    // Act & Assert - Удаляем
    bool removeResult = reviewRepository->remove(reviewId);
    EXPECT_TRUE(removeResult);
    
    auto deletedReview = reviewRepository->findById(reviewId);
    EXPECT_FALSE(deletedReview.has_value());
}

TEST_F(RepositoryIntegrationTest, EnrollmentRepository_FullCycle) {
    // Arrange
    UUID clientId = UUID::generate();
    UUID lessonId = UUID::generate();
    UUID enrollmentId = UUID::generate();
    
    // Создаем клиента
    Client client(clientId, "Enrollment Test User", "enrollment@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    // Создаем урок
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    Lesson lesson(lessonId, LessonType::OPEN_CLASS, "Test Lesson", startTime, 60, 
                 DifficultyLevel::BEGINNER, 20, 50.0, 
                 UUID::fromString("77777777-7777-7777-7777-777777777777"),
                 UUID::fromString("33333333-3333-3333-3333-333333333333"));
    lessonRepository->save(lesson);
    
    Enrollment enrollment(enrollmentId, clientId, lessonId);
    
    // Act & Assert - Сохраняем
    bool saveResult = enrollmentRepository->save(enrollment);
    EXPECT_TRUE(saveResult);
    
    // Проверяем сохранение
    auto foundEnrollment = enrollmentRepository->findById(enrollmentId);
    ASSERT_TRUE(foundEnrollment.has_value());
    EXPECT_EQ(foundEnrollment->getClientId(), clientId);
    EXPECT_EQ(foundEnrollment->getLessonId(), lessonId);
    EXPECT_EQ(foundEnrollment->getStatus(), EnrollmentStatus::REGISTERED);
    
    // Проверяем поиск по клиенту и уроку
    auto clientEnrollments = enrollmentRepository->findByClientId(clientId);
    auto lessonEnrollments = enrollmentRepository->findByLessonId(lessonId);

    auto clientLessonEnrollment = enrollmentRepository->findByClientAndLesson(clientId, lessonId);
    
    EXPECT_EQ(clientEnrollments.size(), 1);
    EXPECT_EQ(lessonEnrollments.size(), 1);
    ASSERT_TRUE(clientLessonEnrollment.has_value());
    EXPECT_EQ(clientLessonEnrollment->getId(), enrollmentId);
    
    // Проверяем подсчет записей
    int enrollmentCount = enrollmentRepository->countByLessonId(lessonId);
    EXPECT_EQ(enrollmentCount, 1);
    
    // Act & Assert - Обновляем
    foundEnrollment->markAttended();
    bool updateResult = enrollmentRepository->update(*foundEnrollment);
    EXPECT_TRUE(updateResult);
    
    auto updatedEnrollment = enrollmentRepository->findById(enrollmentId);
    ASSERT_TRUE(updatedEnrollment.has_value());
    EXPECT_EQ(updatedEnrollment->getStatus(), EnrollmentStatus::ATTENDED);
    
    // Act & Assert - Удаляем
    bool removeResult = enrollmentRepository->remove(enrollmentId);
    EXPECT_TRUE(removeResult);
    
    auto deletedEnrollment = enrollmentRepository->findById(enrollmentId);
    EXPECT_FALSE(deletedEnrollment.has_value());
}

TEST_F(RepositoryIntegrationTest, SubscriptionRepository_FullCycle) {
    // Arrange
    UUID clientId = UUID::generate();
    UUID subscriptionId = UUID::generate();
    UUID subscriptionTypeId = UUID::fromString("55555555-5555-5555-5555-555555555555");
    
    // Создаем клиента
    Client client(clientId, "Subscription Test User", "subscription@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    auto startDate = std::chrono::system_clock::now();
    auto endDate = startDate + std::chrono::hours(24 * 30); // 30 дней
    Subscription subscription(subscriptionId, clientId, subscriptionTypeId, startDate, endDate, 8);
    
    // Act & Assert - Сохраняем
    bool saveResult = subscriptionRepository->save(subscription);
    EXPECT_TRUE(saveResult);
    
    // Проверяем сохранение
    auto foundSubscription = subscriptionRepository->findById(subscriptionId);
    ASSERT_TRUE(foundSubscription.has_value());
    EXPECT_EQ(foundSubscription->getClientId(), clientId);
    EXPECT_EQ(foundSubscription->getRemainingVisits(), 8);
    EXPECT_EQ(foundSubscription->getStatus(), SubscriptionStatus::ACTIVE);
    
    // Проверяем поиск по client_id
    auto clientSubscriptions = subscriptionRepository->findByClientId(clientId);
    EXPECT_EQ(clientSubscriptions.size(), 1);
    EXPECT_EQ(clientSubscriptions[0].getId(), subscriptionId);
    
    // Проверяем поиск активных подписок
    auto activeSubscriptions = subscriptionRepository->findActiveSubscriptions();
    bool foundInActive = std::any_of(activeSubscriptions.begin(), activeSubscriptions.end(),
        [subscriptionId](const Subscription& sub) {
            return sub.getId() == subscriptionId;
        });
    EXPECT_TRUE(foundInActive);
    
    // Act & Assert - Обновляем
    foundSubscription->useVisit(); // Используем одно посещение
    bool updateResult = subscriptionRepository->update(*foundSubscription);
    EXPECT_TRUE(updateResult);
    
    auto updatedSubscription = subscriptionRepository->findById(subscriptionId);
    ASSERT_TRUE(updatedSubscription.has_value());
    EXPECT_EQ(updatedSubscription->getRemainingVisits(), 7);
    
    // Act & Assert - Отменяем подписку
    updatedSubscription->cancel();
    updateResult = subscriptionRepository->update(*updatedSubscription);
    EXPECT_TRUE(updateResult);
    
    auto cancelledSubscription = subscriptionRepository->findById(subscriptionId);
    ASSERT_TRUE(cancelledSubscription.has_value());
    EXPECT_EQ(cancelledSubscription->getStatus(), SubscriptionStatus::CANCELLED);
    
    // Act & Assert - Удаляем
    bool removeResult = subscriptionRepository->remove(subscriptionId);
    EXPECT_TRUE(removeResult);
    
    auto deletedSubscription = subscriptionRepository->findById(subscriptionId);
    EXPECT_FALSE(deletedSubscription.has_value());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "🚀 Running integration tests with database connection" << std::endl;
    std::cout << "📊 Connection: postgresql://dance_user:dance_password@localhost/test_dance_studio" << std::endl;
    
    try {
        return RUN_ALL_TESTS();
    } catch (const std::exception& e) {
        std::cerr << "💥 Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    }
}