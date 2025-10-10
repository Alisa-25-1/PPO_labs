#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/LessonService.hpp"
#include "mocks/MockLessonRepository.hpp"
#include "mocks/MockEnrollmentRepository.hpp"
#include "mocks/MockTrainerRepository.hpp"
#include "mocks/MockDanceHallRepository.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class LessonServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        testTrainerId_ = UUID::fromString("11111111-1111-1111-1111-111111111111");
        testHallId_ = UUID::fromString("22222222-2222-2222-2222-222222222222");
        testLessonId_ = UUID::fromString("33333333-3333-3333-3333-333333333333");
        testBranchId_ = UUID::fromString("44444444-4444-4444-4444-444444444444");
        nonExistentId_ = UUID::fromString("99999999-9999-9999-9999-999999999999");

        auto mockLessonRepo = std::make_unique<NiceMock<MockLessonRepository>>();
        auto mockEnrollmentRepo = std::make_unique<NiceMock<MockEnrollmentRepository>>();
        auto mockTrainerRepo = std::make_unique<NiceMock<MockTrainerRepository>>();
        auto mockHallRepo = std::make_unique<NiceMock<MockDanceHallRepository>>();

        mockLessonRepo_ = mockLessonRepo.get();
        mockEnrollmentRepo_ = mockEnrollmentRepo.get();
        mockTrainerRepo_ = mockTrainerRepo.get();
        mockHallRepo_ = mockHallRepo.get();

        lessonService_ = std::make_unique<LessonService>(
            std::move(mockLessonRepo),
            std::move(mockEnrollmentRepo),
            std::move(mockTrainerRepo),
            std::move(mockHallRepo)
        );

        // Создаем валидного тренера с полными данными
        std::vector<std::string> specializations = {"Ballet", "Contemporary"};
        testTrainer_ = std::make_unique<Trainer>(testTrainerId_, "John Trainer", specializations);
        testTrainer_->setQualificationLevel("senior");
        testTrainer_->setBiography("Experienced trainer with 10 years of experience");
        
        // Создаем валидный зал
        testHall_ = std::make_unique<DanceHall>(testHallId_, "Main Hall", 50, testBranchId_);
        testHall_->setDescription("Large dance hall with mirrors and sound system");
        testHall_->setFloorType("wooden");
        testHall_->setEquipment("Mirrors, sound system, ballet barres");
        
        // Создаем временной слот
        testTimeSlot_ = std::make_unique<TimeSlot>(std::chrono::system_clock::now() + std::chrono::hours(2), 60);
        
        // Создаем урок
        testLesson_ = std::make_unique<Lesson>(testLessonId_, LessonType::OPEN_CLASS, "Ballet Class", 
                                              testTimeSlot_->getStartTime(), testTimeSlot_->getDurationMinutes(),
                                              DifficultyLevel::BEGINNER, 10, 25.0, testTrainerId_, testHallId_);
        testLesson_->setDescription("Beginner ballet class for all ages");
    }

    void TearDown() override {
        testLesson_.reset();
        testTimeSlot_.reset();
        testHall_.reset();
        testTrainer_.reset();
        lessonService_.reset();
    }

    std::unique_ptr<LessonService> lessonService_;
    
    UUID testTrainerId_;
    UUID testHallId_;
    UUID testLessonId_;
    UUID testBranchId_;
    UUID nonExistentId_;
    
    std::unique_ptr<Trainer> testTrainer_;
    std::unique_ptr<DanceHall> testHall_;
    std::unique_ptr<TimeSlot> testTimeSlot_;
    std::unique_ptr<Lesson> testLesson_;

    MockLessonRepository* mockLessonRepo_;
    MockEnrollmentRepository* mockEnrollmentRepo_;
    MockTrainerRepository* mockTrainerRepo_;
    MockDanceHallRepository* mockHallRepo_;
};

TEST_F(LessonServiceTest, CreateLesson_Success) {
    LessonRequestDTO request;
    request.trainerId = testTrainerId_;
    request.hallId = testHallId_;
    request.startTime = testTimeSlot_->getStartTime();
    request.durationMinutes = testTimeSlot_->getDurationMinutes();
    request.type = LessonType::OPEN_CLASS;
    request.name = "Ballet Class";
    request.description = "Beginner ballet class";
    request.difficulty = DifficultyLevel::BEGINNER;
    request.maxParticipants = 10;
    request.price = 25.0;
    
    EXPECT_CALL(*mockTrainerRepo_, findById(testTrainerId_))
        .WillOnce(Return(*testTrainer_));
    EXPECT_CALL(*mockHallRepo_, exists(testHallId_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockLessonRepo_, findConflictingLessons(testHallId_, *testTimeSlot_))
        .WillOnce(Return(std::vector<Lesson>{}));
    EXPECT_CALL(*mockLessonRepo_, save(_))
        .WillOnce(Return(true));

    auto result = lessonService_->createLesson(request);

    EXPECT_EQ(result.trainerId, testTrainerId_);
    EXPECT_EQ(result.hallId, testHallId_);
    EXPECT_EQ(result.name, "Ballet Class");
}

TEST_F(LessonServiceTest, CreateLesson_TimeConflict) {
    LessonRequestDTO request;
    request.trainerId = testTrainerId_;
    request.hallId = testHallId_;
    request.startTime = testTimeSlot_->getStartTime();
    request.durationMinutes = testTimeSlot_->getDurationMinutes();
    request.type = LessonType::OPEN_CLASS;
    request.name = "Ballet Class";
    request.description = "Beginner ballet class";
    request.difficulty = DifficultyLevel::BEGINNER;
    request.maxParticipants = 10;
    request.price = 25.0;
    
    EXPECT_CALL(*mockTrainerRepo_, findById(testTrainerId_))
        .WillOnce(Return(*testTrainer_));
    EXPECT_CALL(*mockHallRepo_, exists(testHallId_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockLessonRepo_, findConflictingLessons(testHallId_, *testTimeSlot_))
        .WillOnce(Return(std::vector<Lesson>{*testLesson_}));

    EXPECT_THROW(
        lessonService_->createLesson(request),
        BusinessRuleException
    );
}

TEST_F(LessonServiceTest, CreateLesson_TrainerNotFound) {
    LessonRequestDTO request;
    request.trainerId = nonExistentId_;
    request.hallId = testHallId_;
    request.startTime = testTimeSlot_->getStartTime();
    request.durationMinutes = testTimeSlot_->getDurationMinutes();
    request.type = LessonType::OPEN_CLASS;
    request.name = "Ballet Class";
    request.description = "Beginner ballet class";
    request.difficulty = DifficultyLevel::BEGINNER;
    request.maxParticipants = 10;
    request.price = 25.0;
    
    EXPECT_CALL(*mockTrainerRepo_, findById(nonExistentId_))
        .WillOnce(Return(std::optional<Trainer>{}));

    EXPECT_THROW(
        lessonService_->createLesson(request),
        ValidationException
    );
}

TEST_F(LessonServiceTest, CreateLesson_HallNotFound) {
    LessonRequestDTO request;
    request.trainerId = testTrainerId_;
    request.hallId = nonExistentId_;
    request.startTime = testTimeSlot_->getStartTime();
    request.durationMinutes = testTimeSlot_->getDurationMinutes();
    request.type = LessonType::OPEN_CLASS;
    request.name = "Ballet Class";
    request.description = "Beginner ballet class";
    request.difficulty = DifficultyLevel::BEGINNER;
    request.maxParticipants = 10;
    request.price = 25.0;
    
    EXPECT_CALL(*mockTrainerRepo_, findById(testTrainerId_))
        .WillOnce(Return(*testTrainer_));
    EXPECT_CALL(*mockHallRepo_, exists(nonExistentId_))
        .WillOnce(Return(false));

    EXPECT_THROW(
        lessonService_->createLesson(request),
        ValidationException
    );
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}