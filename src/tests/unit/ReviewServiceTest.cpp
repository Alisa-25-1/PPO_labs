#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/ReviewService.hpp"
#include "mocks/MockReviewRepository.hpp"
#include "mocks/MockClientRepository.hpp"
#include "mocks/MockLessonRepository.hpp"
#include "mocks/MockEnrollmentRepository.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class ReviewServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        testClientId_ = UUID::fromString("11111111-1111-1111-1111-111111111111");
        testLessonId_ = UUID::fromString("22222222-2222-2222-2222-222222222222");
        testReviewId_ = UUID::fromString("33333333-3333-3333-3333-333333333333");
        testTrainerId_ = UUID::fromString("44444444-4444-4444-4444-444444444444");
        testHallId_ = UUID::fromString("55555555-5555-5555-5555-555555555555");
        nonExistentId_ = UUID::fromString("99999999-9999-9999-9999-999999999999");

        auto mockReviewRepo = std::make_unique<NiceMock<MockReviewRepository>>();
        auto mockClientRepo = std::make_unique<NiceMock<MockClientRepository>>();
        auto mockLessonRepo = std::make_unique<NiceMock<MockLessonRepository>>();
        auto mockEnrollmentRepo = std::make_unique<NiceMock<MockEnrollmentRepository>>();

        mockReviewRepo_ = mockReviewRepo.get();
        mockClientRepo_ = mockClientRepo.get();
        mockLessonRepo_ = mockLessonRepo.get();
        mockEnrollmentRepo_ = mockEnrollmentRepo.get();

        // ДОБАВЬТЕ ЭТУ СТРОКУ - создание сервиса
        reviewService_ = std::make_unique<ReviewService>(
            std::move(mockReviewRepo),
            std::move(mockClientRepo),
            std::move(mockLessonRepo),
            std::move(mockEnrollmentRepo)
        );

        testClient_ = std::make_unique<Client>(testClientId_, "John Doe", "john@example.com", "+1234567890");
        testClient_->activate();
        
        // Остальной код без изменений...
        auto startTime = std::chrono::system_clock::now() + std::chrono::hours(1);
        testLesson_ = std::make_unique<Lesson>(
            testLessonId_, LessonType::OPEN_CLASS, "Completed Lesson", 
            startTime, 60, DifficultyLevel::BEGINNER, 10, 25.0, testTrainerId_, testHallId_
        );
        
        testEnrollment_ = std::make_unique<Enrollment>(
            UUID::generate(), testClientId_, testLessonId_
        );
        testEnrollment_->markAttended();
        
        testReview_ = std::make_unique<Review>(
            testReviewId_, testClientId_, testLessonId_, 5, "Great lesson!"
        );
    }

    void TearDown() override {
        testReview_.reset();
        testEnrollment_.reset();
        testLesson_.reset();
        testClient_.reset();
        reviewService_.reset();
    }

    std::unique_ptr<ReviewService> reviewService_;
    
    UUID testClientId_;
    UUID testLessonId_;
    UUID testReviewId_;
    UUID testTrainerId_;
    UUID testHallId_;
    UUID nonExistentId_;
    
    std::unique_ptr<Client> testClient_;
    std::unique_ptr<Lesson> testLesson_;
    std::unique_ptr<Enrollment> testEnrollment_;
    std::unique_ptr<Review> testReview_;

    MockReviewRepository* mockReviewRepo_;
    MockClientRepository* mockClientRepo_;
    MockLessonRepository* mockLessonRepo_;
    MockEnrollmentRepository* mockEnrollmentRepo_;
};

TEST_F(ReviewServiceTest, CreateReview_Success) {
    ReviewRequestDTO request{testClientId_, testLessonId_, 5, "Excellent lesson!"};
    
    // Создаем завершенный урок специально для этого теста
    auto completedStartTime = std::chrono::system_clock::now() - std::chrono::hours(48); // 2 дня назад
    auto completedLesson = Lesson(
        testLessonId_, LessonType::OPEN_CLASS, "Completed Lesson", 
        completedStartTime, 60, DifficultyLevel::BEGINNER, 10, 25.0, testTrainerId_, testHallId_
    );
    completedLesson.setStatus(LessonStatus::COMPLETED);
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockLessonRepo_, findById(testLessonId_))
        .WillOnce(Return(completedLesson)); // Возвращаем завершенный урок
    EXPECT_CALL(*mockEnrollmentRepo_, findByClientAndLesson(testClientId_, testLessonId_))
        .WillOnce(Return(*testEnrollment_));
    EXPECT_CALL(*mockReviewRepo_, findByClientAndLesson(testClientId_, testLessonId_))
        .WillOnce(Return(std::optional<Review>{}));
    EXPECT_CALL(*mockReviewRepo_, save(_))
        .WillOnce(Return(true));

    auto result = reviewService_->createReview(request);

    EXPECT_EQ(result.clientId, testClientId_);
    EXPECT_EQ(result.lessonId, testLessonId_);
    EXPECT_EQ(result.rating, 5);
    EXPECT_EQ(result.status, "PENDING_MODERATION");
}

TEST_F(ReviewServiceTest, CreateReview_ClientNotAttended) {
    ReviewRequestDTO request{testClientId_, testLessonId_, 5, "Excellent lesson!"};
    
    // Создаем завершенный урок
    auto completedStartTime = std::chrono::system_clock::now() - std::chrono::hours(48);
    auto completedLesson = Lesson(
        testLessonId_, LessonType::OPEN_CLASS, "Completed Lesson", 
        completedStartTime, 60, DifficultyLevel::BEGINNER, 10, 25.0, testTrainerId_, testHallId_
    );
    completedLesson.setStatus(LessonStatus::COMPLETED);
    
    auto notAttendedEnrollment = Enrollment(UUID::generate(), testClientId_, testLessonId_);
    notAttendedEnrollment.markMissed();
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockLessonRepo_, findById(testLessonId_))
        .WillOnce(Return(completedLesson)); // Завершенный урок
    EXPECT_CALL(*mockEnrollmentRepo_, findByClientAndLesson(testClientId_, testLessonId_))
        .WillOnce(Return(notAttendedEnrollment));

    EXPECT_THROW(
        reviewService_->createReview(request),
        BusinessRuleException
    );
}

TEST_F(ReviewServiceTest, CreateReview_AlreadyReviewed) {
    ReviewRequestDTO request{testClientId_, testLessonId_, 5, "Excellent lesson!"};
    
    // Создаем завершенный урок
    auto completedStartTime = std::chrono::system_clock::now() - std::chrono::hours(48);
    auto completedLesson = Lesson(
        testLessonId_, LessonType::OPEN_CLASS, "Completed Lesson", 
        completedStartTime, 60, DifficultyLevel::BEGINNER, 10, 25.0, testTrainerId_, testHallId_
    );
    completedLesson.setStatus(LessonStatus::COMPLETED);
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockLessonRepo_, findById(testLessonId_))
        .WillOnce(Return(completedLesson)); // Завершенный урок
    EXPECT_CALL(*mockEnrollmentRepo_, findByClientAndLesson(testClientId_, testLessonId_))
        .WillOnce(Return(*testEnrollment_));
    EXPECT_CALL(*mockReviewRepo_, findByClientAndLesson(testClientId_, testLessonId_))
        .WillOnce(Return(*testReview_));

    EXPECT_THROW(
        reviewService_->createReview(request),
        BusinessRuleException
    );
}

TEST_F(ReviewServiceTest, ApproveReview_Success) {
    EXPECT_CALL(*mockReviewRepo_, findById(testReviewId_))
        .WillOnce(Return(*testReview_));
    EXPECT_CALL(*mockReviewRepo_, update(_))
        .WillOnce(Return(true));

    auto result = reviewService_->approveReview(testReviewId_);

    EXPECT_EQ(result.status, "APPROVED");
}

TEST_F(ReviewServiceTest, GetPendingReviews_Success) {
    std::vector<Review> pendingReviews = {*testReview_};
    
    EXPECT_CALL(*mockReviewRepo_, findPendingModeration())
        .WillOnce(Return(pendingReviews));

    auto result = reviewService_->getPendingReviews();

    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].status, "PENDING_MODERATION");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}