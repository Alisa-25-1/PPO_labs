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
        SqlQueryBuilder truncateBuilder;
        std::string truncateQuery = truncateBuilder
            .truncate({
                "reviews", "enrollments", "lessons", "trainer_specializations",
                "trainers", "bookings", "subscriptions", "subscription_types",
                "clients", "dance_halls", "branches", "studios"
            })
            .build();

        work.exec(truncateQuery);
        dbConnection->commitTransaction(work); 

    } catch (const std::exception& e) {
        std::cerr << "⚠️ Warning: Failed to clear test data: " << e.what() << std::endl;
    }
}
    void setupTestData() {
        try {
            auto work = dbConnection->beginTransaction();

             SqlQueryBuilder studioBuilder;
            std::string studioQuery = studioBuilder
                .insertInto("studios")
                .values({
                    {"id", "'11111111-1111-1111-1111-111111111111'"},
                    {"name", "'Test Studio'"},
                    {"contact_email", "'studio@test.com'"}
                })
                .build();
            work.exec(studioQuery);
            
            // Branch
            SqlQueryBuilder branchBuilder;
            std::string branchQuery = branchBuilder
                .insertInto("branches")
                .values({
                    {"id", "'22222222-2222-2222-2222-222222222222'"},
                    {"name", "'Test Branch'"},
                    {"address", "'Test Address'"},
                    {"phone", "'+1234567890'"},
                    {"open_time", "9"},
                    {"close_time", "22"},
                    {"studio_id", "'11111111-1111-1111-1111-111111111111'"}
                })
                .build();
            work.exec(branchQuery);
            
            // Dance Halls
            SqlQueryBuilder hallBuilder1;
            std::string hallQuery1 = hallBuilder1
                .insertInto("dance_halls")
                .values({
                    {"id", "'33333333-3333-3333-3333-333333333333'"},
                    {"name", "'Test Hall 1'"},
                    {"description", "'Large hall'"},
                    {"capacity", "50"},
                    {"floor_type", "'wooden'"},
                    {"equipment", "'mirrors'"},
                    {"branch_id", "'22222222-2222-2222-2222-222222222222'"}
                })
                .build();
            work.exec(hallQuery1);
            
            SqlQueryBuilder hallBuilder2;
            std::string hallQuery2 = hallBuilder2
                .insertInto("dance_halls")
                .values({
                    {"id", "'44444444-4444-4444-4444-444444444444'"},
                    {"name", "'Test Hall 2'"},
                    {"description", "'Small hall'"},
                    {"capacity", "30"},
                    {"floor_type", "'marley'"},
                    {"equipment", "'sound system'"},
                    {"branch_id", "'22222222-2222-2222-2222-222222222222'"}
                })
                .build();
            work.exec(hallQuery2);
            
            // Subscription Types
            SqlQueryBuilder subTypeBuilder1;
            std::string subTypeQuery1 = subTypeBuilder1
                .insertInto("subscription_types")
                .values({
                    {"id", "'55555555-5555-5555-5555-555555555555'"},
                    {"name", "'Monthly'"},
                    {"description", "'Monthly subscription'"},
                    {"validity_days", "30"},
                    {"visit_count", "8"},
                    {"unlimited", "false"},
                    {"price", "100.0"}
                })
                .build();
            work.exec(subTypeQuery1);
            
            SqlQueryBuilder subTypeBuilder2;
            std::string subTypeQuery2 = subTypeBuilder2
                .insertInto("subscription_types")
                .values({
                    {"id", "'66666666-6666-6666-6666-666666666666'"},
                    {"name", "'Unlimited'"},
                    {"description", "'Unlimited monthly'"},
                    {"validity_days", "30"},
                    {"visit_count", "0"},
                    {"unlimited", "true"},
                    {"price", "200.0"}
                })
                .build();
            work.exec(subTypeQuery2);
            
            // Trainer
            SqlQueryBuilder trainerBuilder;
            std::string trainerQuery = trainerBuilder
                .insertInto("trainers")
                .values({
                    {"id", "'77777777-7777-7777-7777-777777777777'"},
                    {"name", "'Test Trainer'"},
                    {"biography", "'Experienced trainer'"},
                    {"qualification_level", "'senior'"},
                    {"is_active", "true"}
                })
                .build();
            work.exec(trainerQuery);
            
            // Trainer Specializations
            SqlQueryBuilder specBuilder1;
            std::string specQuery1 = specBuilder1
                .insertInto("trainer_specializations")
                .values({
                    {"trainer_id", "'77777777-7777-7777-7777-777777777777'"},
                    {"specialization", "'Ballet'"}
                })
                .build();
            work.exec(specQuery1);
            
            SqlQueryBuilder specBuilder2;
            std::string specQuery2 = specBuilder2
                .insertInto("trainer_specializations")
                .values({
                    {"trainer_id", "'77777777-7777-7777-7777-777777777777'"},
                    {"specialization", "'Contemporary'"}
                })
                .build();
            work.exec(specQuery2);
            
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

std::shared_ptr<ResilientDatabaseConnection> RepositoryIntegrationTest::dbConnection = nullptr;

TEST_F(RepositoryIntegrationTest, ClientRepository_SaveAndFind) {
    UUID clientId = UUID::generate();
    Client client(clientId, "Integration Test User", "integration@example.com", "+12345678901");
    
    // Устанавливаем валидный пароль
    client.changePassword("ValidPass123");
    client.activate();
    
    bool saveResult = clientRepository->save(client);
    
    EXPECT_TRUE(saveResult);

    auto foundClient = clientRepository->findById(clientId);
    
    ASSERT_TRUE(foundClient.has_value());
    EXPECT_EQ(foundClient->getId(), clientId);
    EXPECT_EQ(foundClient->getName(), "Integration Test User");
    EXPECT_EQ(foundClient->getEmail(), "integration@example.com");
    EXPECT_EQ(foundClient->getPhone(), "+12345678901");
    EXPECT_TRUE(foundClient->isActive());
    
    // Проверяем, что пароль сохранился корректно
    EXPECT_TRUE(foundClient->validatePassword("ValidPass123"));
    
    auto foundByEmail = clientRepository->findByEmail("integration@example.com");
    
    ASSERT_TRUE(foundByEmail.has_value());
    EXPECT_EQ(foundByEmail->getId(), clientId);
}

TEST_F(RepositoryIntegrationTest, BookingRepository_FullCycle) {
    UUID clientId = UUID::generate();
    UUID bookingId = UUID::generate();
    UUID hallId = UUID::fromString("33333333-3333-3333-3333-333333333333");
    
    Client client(clientId, "Booking Test User", "booking@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(1);
    TimeSlot timeSlot(startTime, 120);
    Booking booking(bookingId, clientId, hallId, timeSlot, "Integration Test Booking");
    
    bool saveResult = bookingRepository->save(booking);
    EXPECT_TRUE(saveResult);
    
    auto foundBooking = bookingRepository->findById(bookingId);
    ASSERT_TRUE(foundBooking.has_value());
    EXPECT_EQ(foundBooking->getPurpose(), "Integration Test Booking");
    EXPECT_EQ(foundBooking->getStatus(), BookingStatus::PENDING);
    
    foundBooking->confirm();
    bool updateResult = bookingRepository->update(*foundBooking);
    EXPECT_TRUE(updateResult);
    
    auto updatedBooking = bookingRepository->findById(bookingId);
    ASSERT_TRUE(updatedBooking.has_value());
    EXPECT_EQ(updatedBooking->getStatus(), BookingStatus::CONFIRMED);
    
    bool removeResult = bookingRepository->remove(bookingId);
    EXPECT_TRUE(removeResult);
    
    auto deletedBooking = bookingRepository->findById(bookingId);
    EXPECT_FALSE(deletedBooking.has_value());
}

TEST_F(RepositoryIntegrationTest, SubscriptionRepository_CreateAndFind) {
    UUID clientId = UUID::generate();
    UUID subscriptionId = UUID::generate();
    UUID subscriptionTypeId = UUID::fromString("55555555-5555-5555-5555-555555555555");
    
    Client client(clientId, "Subscription Test User", "subscription@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    auto startDate = std::chrono::system_clock::now();
    auto endDate = startDate + std::chrono::hours(24 * 30); // 30 дней
    Subscription subscription(subscriptionId, clientId, subscriptionTypeId, startDate, endDate, 8);
    
    bool saveResult = subscriptionRepository->save(subscription);
    auto foundSubscription = subscriptionRepository->findById(subscriptionId);
    auto clientSubscriptions = subscriptionRepository->findByClientId(clientId);
    
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundSubscription.has_value());
    EXPECT_EQ(foundSubscription->getClientId(), clientId);
    EXPECT_EQ(foundSubscription->getRemainingVisits(), 8);
    EXPECT_EQ(foundSubscription->getStatus(), SubscriptionStatus::ACTIVE);
    
    EXPECT_EQ(clientSubscriptions.size(), 1);
    EXPECT_EQ(clientSubscriptions[0].getId(), subscriptionId);
}

TEST_F(RepositoryIntegrationTest, DanceHallRepository_BasicOperations) {
    auto hall1 = hallRepository->findById(UUID::fromString("33333333-3333-3333-3333-333333333333"));
    auto hall2 = hallRepository->findById(UUID::fromString("44444444-4444-4444-4444-444444444444"));
    auto allHalls = hallRepository->findAll();
    
    ASSERT_TRUE(hall1.has_value());
    EXPECT_EQ(hall1->getName(), "Test Hall 1");
    EXPECT_EQ(hall1->getCapacity(), 50);
    
    ASSERT_TRUE(hall2.has_value());
    EXPECT_EQ(hall2->getName(), "Test Hall 2");
    EXPECT_EQ(hall2->getCapacity(), 30);
    
    EXPECT_GE(allHalls.size(), 2);
}

TEST_F(RepositoryIntegrationTest, SubscriptionTypeRepository_CreateAndFind) {
    UUID subscriptionTypeId = UUID::generate();
    SubscriptionType subscriptionType(subscriptionTypeId, "Test Subscription", 30, 12, false, 150.0);
    subscriptionType.setDescription("Test subscription type for integration tests");
    
    bool saveResult = subscriptionTypeRepository->save(subscriptionType);
    auto foundType = subscriptionTypeRepository->findById(subscriptionTypeId);
    auto allActiveTypes = subscriptionTypeRepository->findAllActive();
    auto allTypes = subscriptionTypeRepository->findAll();
    
    EXPECT_TRUE(saveResult);
    ASSERT_TRUE(foundType.has_value());
    EXPECT_EQ(foundType->getName(), "Test Subscription");
    EXPECT_EQ(foundType->getValidityDays(), 30);
    EXPECT_EQ(foundType->getVisitCount(), 12);
    EXPECT_EQ(foundType->getPrice(), 150.0);

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
    
    subscriptionType.setDescription("Updated description");
    bool updateResult = subscriptionTypeRepository->update(subscriptionType);
    EXPECT_TRUE(updateResult);
    
    auto updatedType = subscriptionTypeRepository->findById(subscriptionTypeId);
    ASSERT_TRUE(updatedType.has_value());
    EXPECT_EQ(updatedType->getDescription(), "Updated description");
    
    bool removeResult = subscriptionTypeRepository->remove(subscriptionTypeId);
    EXPECT_TRUE(removeResult);
    
    auto deletedType = subscriptionTypeRepository->findById(subscriptionTypeId);
    EXPECT_FALSE(deletedType.has_value());
}

TEST_F(RepositoryIntegrationTest, StudioRepository_FullCycle) {
    UUID studioId = UUID::generate();
    Studio studio(studioId, "Integration Test Studio", "integration@studio.com");
    studio.setDescription("Test studio for integration tests");
    
    bool saveResult = studioRepository->save(studio);
    EXPECT_TRUE(saveResult);
    
    auto foundStudio = studioRepository->findById(studioId);
    ASSERT_TRUE(foundStudio.has_value());
    EXPECT_EQ(foundStudio->getName(), "Integration Test Studio");
    EXPECT_EQ(foundStudio->getContactEmail(), "integration@studio.com");
    
    studio.setDescription("Updated description");
    bool updateResult = studioRepository->update(studio);
    EXPECT_TRUE(updateResult);
    
    auto updatedStudio = studioRepository->findById(studioId);
    ASSERT_TRUE(updatedStudio.has_value());
    EXPECT_EQ(updatedStudio->getDescription(), "Updated description");
    
    bool removeResult = studioRepository->remove(studioId);
    EXPECT_TRUE(removeResult);
    
    auto deletedStudio = studioRepository->findById(studioId);
    EXPECT_FALSE(deletedStudio.has_value());
}

TEST_F(RepositoryIntegrationTest, BranchRepository_FullCycle) {
    UUID branchId = UUID::generate();
    UUID studioId = UUID::fromString("11111111-1111-1111-1111-111111111111");
    
    WorkingHours workingHours(std::chrono::hours(9), std::chrono::hours(22));
    Branch branch(branchId, "Integration Test Branch", "Test Address", "+1234567890", workingHours, studioId);
    
    bool saveResult = branchRepository->save(branch);
    EXPECT_TRUE(saveResult);
    
    auto foundBranch = branchRepository->findById(branchId);
    ASSERT_TRUE(foundBranch.has_value());
    EXPECT_EQ(foundBranch->getName(), "Integration Test Branch");
    EXPECT_EQ(foundBranch->getAddress(), "Test Address");
    
    auto studioBranches = branchRepository->findByStudioId(studioId);
    EXPECT_GE(studioBranches.size(), 1);

    branch.setAddress("Updated Address");
    bool updateResult = branchRepository->update(branch);
    EXPECT_TRUE(updateResult);
    
    auto updatedBranch = branchRepository->findById(branchId);
    ASSERT_TRUE(updatedBranch.has_value());
    EXPECT_EQ(updatedBranch->getAddress(), "Updated Address");
    
    bool removeResult = branchRepository->remove(branchId);
    EXPECT_TRUE(removeResult);
    
    auto deletedBranch = branchRepository->findById(branchId);
    EXPECT_FALSE(deletedBranch.has_value());
}

TEST_F(RepositoryIntegrationTest, ReviewRepository_CreateAndFind) {
    UUID clientId = UUID::generate();
    UUID lessonId = UUID::generate();
    UUID reviewId = UUID::generate();
    
    Client client(clientId, "Review Test User", "review@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    Lesson lesson(lessonId, LessonType::OPEN_CLASS, "Test Lesson", startTime, 60, 
                 DifficultyLevel::BEGINNER, 20, 50.0, 
                 UUID::fromString("77777777-7777-7777-7777-777777777777"),
                 UUID::fromString("33333333-3333-3333-3333-333333333333"));
    lessonRepository->save(lesson);
    
    Review review(reviewId, clientId, lessonId, 5, "Excellent lesson!");
    
    bool saveResult = reviewRepository->save(review);
    auto foundReview = reviewRepository->findById(reviewId);
    auto clientReviews = reviewRepository->findByClientId(clientId);
    auto lessonReviews = reviewRepository->findByLessonId(lessonId);
    auto clientLessonReview = reviewRepository->findByClientAndLesson(clientId, lessonId);
    
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
    UUID clientId = UUID::generate();
    UUID lessonId = UUID::generate();
    UUID reviewId = UUID::generate();
    
    Client client(clientId, "Review Test User", "review@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    Lesson lesson(lessonId, LessonType::OPEN_CLASS, "Test Lesson", startTime, 60, 
                 DifficultyLevel::BEGINNER, 20, 50.0, 
                 UUID::fromString("77777777-7777-7777-7777-777777777777"),
                 UUID::fromString("33333333-3333-3333-3333-333333333333"));
    lessonRepository->save(lesson);
    
    Review review(reviewId, clientId, lessonId, 5, "Excellent lesson!");
    
    bool saveResult = reviewRepository->save(review);
    EXPECT_TRUE(saveResult);
    
    auto foundReview = reviewRepository->findById(reviewId);
    ASSERT_TRUE(foundReview.has_value());
    EXPECT_EQ(foundReview->getRating(), 5);
    EXPECT_EQ(foundReview->getComment(), "Excellent lesson!");
    EXPECT_EQ(foundReview->getStatus(), ReviewStatus::PENDING_MODERATION);
    
    auto clientReviews = reviewRepository->findByClientId(clientId);
    auto lessonReviews = reviewRepository->findByLessonId(lessonId);
    auto clientLessonReview = reviewRepository->findByClientAndLesson(clientId, lessonId);
    
    EXPECT_EQ(clientReviews.size(), 1);
    EXPECT_EQ(lessonReviews.size(), 1);
    ASSERT_TRUE(clientLessonReview.has_value());
    EXPECT_EQ(clientLessonReview->getId(), reviewId);
    
    foundReview->approve();
    bool updateResult = reviewRepository->update(*foundReview);
    EXPECT_TRUE(updateResult);
    
    auto updatedReview = reviewRepository->findById(reviewId);
    ASSERT_TRUE(updatedReview.has_value());
    EXPECT_EQ(updatedReview->getStatus(), ReviewStatus::APPROVED);
    
    bool removeResult = reviewRepository->remove(reviewId);
    EXPECT_TRUE(removeResult);
    
    auto deletedReview = reviewRepository->findById(reviewId);
    EXPECT_FALSE(deletedReview.has_value());
}

TEST_F(RepositoryIntegrationTest, EnrollmentRepository_FullCycle) {
    UUID clientId = UUID::generate();
    UUID lessonId = UUID::generate();
    UUID enrollmentId = UUID::generate();
    
    Client client(clientId, "Enrollment Test User", "enrollment@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    Lesson lesson(lessonId, LessonType::OPEN_CLASS, "Test Lesson", startTime, 60, 
                 DifficultyLevel::BEGINNER, 20, 50.0, 
                 UUID::fromString("77777777-7777-7777-7777-777777777777"),
                 UUID::fromString("33333333-3333-3333-3333-333333333333"));
    lessonRepository->save(lesson);
    
    Enrollment enrollment(enrollmentId, clientId, lessonId);
    
    bool saveResult = enrollmentRepository->save(enrollment);
    EXPECT_TRUE(saveResult);
    
    auto foundEnrollment = enrollmentRepository->findById(enrollmentId);
    ASSERT_TRUE(foundEnrollment.has_value());
    EXPECT_EQ(foundEnrollment->getClientId(), clientId);
    EXPECT_EQ(foundEnrollment->getLessonId(), lessonId);
    EXPECT_EQ(foundEnrollment->getStatus(), EnrollmentStatus::REGISTERED);
    
    auto clientEnrollments = enrollmentRepository->findByClientId(clientId);
    auto lessonEnrollments = enrollmentRepository->findByLessonId(lessonId);

    auto clientLessonEnrollment = enrollmentRepository->findByClientAndLesson(clientId, lessonId);
    
    EXPECT_EQ(clientEnrollments.size(), 1);
    EXPECT_EQ(lessonEnrollments.size(), 1);
    ASSERT_TRUE(clientLessonEnrollment.has_value());
    EXPECT_EQ(clientLessonEnrollment->getId(), enrollmentId);
    
    int enrollmentCount = enrollmentRepository->countByLessonId(lessonId);
    EXPECT_EQ(enrollmentCount, 1);
    
    foundEnrollment->markAttended();
    bool updateResult = enrollmentRepository->update(*foundEnrollment);
    EXPECT_TRUE(updateResult);
    
    auto updatedEnrollment = enrollmentRepository->findById(enrollmentId);
    ASSERT_TRUE(updatedEnrollment.has_value());
    EXPECT_EQ(updatedEnrollment->getStatus(), EnrollmentStatus::ATTENDED);

    bool removeResult = enrollmentRepository->remove(enrollmentId);
    EXPECT_TRUE(removeResult);
    
    auto deletedEnrollment = enrollmentRepository->findById(enrollmentId);
    EXPECT_FALSE(deletedEnrollment.has_value());
}

TEST_F(RepositoryIntegrationTest, SubscriptionRepository_FullCycle) {
    UUID clientId = UUID::generate();
    UUID subscriptionId = UUID::generate();
    UUID subscriptionTypeId = UUID::fromString("55555555-5555-5555-5555-555555555555");
    
    Client client(clientId, "Subscription Test User", "subscription@example.com", "+12345678901");
    client.changePassword("ValidPass123");
    client.activate();
    clientRepository->save(client);
    
    auto startDate = std::chrono::system_clock::now();
    auto endDate = startDate + std::chrono::hours(24 * 30); // 30 дней
    Subscription subscription(subscriptionId, clientId, subscriptionTypeId, startDate, endDate, 8);
    
    bool saveResult = subscriptionRepository->save(subscription);
    EXPECT_TRUE(saveResult);
    
    auto foundSubscription = subscriptionRepository->findById(subscriptionId);
    ASSERT_TRUE(foundSubscription.has_value());
    EXPECT_EQ(foundSubscription->getClientId(), clientId);
    EXPECT_EQ(foundSubscription->getRemainingVisits(), 8);
    EXPECT_EQ(foundSubscription->getStatus(), SubscriptionStatus::ACTIVE);
    
    auto clientSubscriptions = subscriptionRepository->findByClientId(clientId);
    EXPECT_EQ(clientSubscriptions.size(), 1);
    EXPECT_EQ(clientSubscriptions[0].getId(), subscriptionId);
    
    auto activeSubscriptions = subscriptionRepository->findActiveSubscriptions();
    bool foundInActive = std::any_of(activeSubscriptions.begin(), activeSubscriptions.end(),
        [subscriptionId](const Subscription& sub) {
            return sub.getId() == subscriptionId;
        });
    EXPECT_TRUE(foundInActive);
    
    foundSubscription->useVisit();
    bool updateResult = subscriptionRepository->update(*foundSubscription);
    EXPECT_TRUE(updateResult);
    
    auto updatedSubscription = subscriptionRepository->findById(subscriptionId);
    ASSERT_TRUE(updatedSubscription.has_value());
    EXPECT_EQ(updatedSubscription->getRemainingVisits(), 7);
    

    updatedSubscription->cancel();
    updateResult = subscriptionRepository->update(*updatedSubscription);
    EXPECT_TRUE(updateResult);
    
    auto cancelledSubscription = subscriptionRepository->findById(subscriptionId);
    ASSERT_TRUE(cancelledSubscription.has_value());
    EXPECT_EQ(cancelledSubscription->getStatus(), SubscriptionStatus::CANCELLED);
    
    bool removeResult = subscriptionRepository->remove(subscriptionId);
    EXPECT_TRUE(removeResult);
    
    auto deletedSubscription = subscriptionRepository->findById(subscriptionId);
    EXPECT_FALSE(deletedSubscription.has_value());
}

TEST_F(RepositoryIntegrationTest, SqlBuilder_ComplexQueries) {   
    // 1. Запрос с JOIN для получения залов с информацией о филиалах
    SqlQueryBuilder joinQuery;
    std::string complexQuery = joinQuery
        .select({"dh.name as hall_name", "dh.capacity", "b.name as branch_name", "s.name as studio_name"})
        .from("dance_halls dh")
        .innerJoin("branches b", "dh.branch_id = b.id")
        .innerJoin("studios s", "b.studio_id = s.id")
        .where("dh.capacity > 20")
        .orderBy("dh.capacity", false)
        .build();
    
    EXPECT_FALSE(complexQuery.empty());
    EXPECT_NE(complexQuery.find("JOIN"), std::string::npos);
    
    // 2. Запрос с несколькими условиями WHERE
    SqlQueryBuilder multiWhereQuery;
    std::string filteredQuery = multiWhereQuery
        .select({"*"})
        .from("lessons")
        .where("difficulty = 'BEGINNER'")
        .andWhere("max_participants > 10")
        .orWhere("price < 50.0")
        .build();
    
    EXPECT_FALSE(filteredQuery.empty());
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