#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/BookingService.hpp"
#include "mocks/MockBookingRepository.hpp"
#include "mocks/MockClientRepository.hpp"
#include "mocks/MockDanceHallRepository.hpp"
#include "mocks/MockBranchRepository.hpp"
#include "mocks/MockBranchService.hpp"
#include "mocks/MockAttendanceService.hpp"  
#include "mocks/MockLessonRepository.hpp"
#include "mocks/MockAttendanceRepository.hpp"  
#include "mocks/MockEnrollmentRepository.hpp"  
#include "../../services/exceptions/BookingException.hpp"
#include "../../services/exceptions/ValidationException.hpp"

using namespace testing;

class BookingServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем моки как shared_ptr
        auto mockBookingRepo = std::make_shared<MockBookingRepository>();
        auto mockClientRepo = std::make_shared<MockClientRepository>();
        auto mockHallRepo = std::make_shared<MockDanceHallRepository>();
        auto mockBranchRepo = std::make_shared<MockBranchRepository>();
        auto mockBranchService = std::make_shared<MockBranchService>();
        auto mockLessonRepo = std::make_shared<MockLessonRepository>();
        
        // Создаем моки для зависимостей AttendanceService
        auto mockAttendanceRepo = std::make_shared<MockAttendanceRepository>();
        auto mockEnrollmentRepo = std::make_shared<MockEnrollmentRepository>();
        
        // Создаем AttendanceService с его зависимостями
        auto mockAttendanceService = std::make_shared<MockAttendanceService>(
            mockAttendanceRepo,
            mockBookingRepo, 
            mockEnrollmentRepo,
            mockLessonRepo
        );

        // Сохраняем указатели для доступа в тестах
        mockBookingRepo_ = mockBookingRepo.get();
        mockClientRepo_ = mockClientRepo.get();
        mockHallRepo_ = mockHallRepo.get();
        mockBranchRepo_ = mockBranchRepo.get();
        mockBranchService_ = mockBranchService.get();
        mockLessonRepo_ = mockLessonRepo.get();
        mockAttendanceService_ = mockAttendanceService.get(); 

        bookingService_ = std::make_unique<BookingService>(
            mockBookingRepo,      // 1. bookingRepo
            mockClientRepo,       // 2. clientRepo  
            mockHallRepo,         // 3. hallRepo
            mockBranchRepo,       // 4. branchRepo
            mockBranchService,    // 5. branchService
            mockLessonRepo,       // 6. lessonRepo
            mockAttendanceService // 7. attendanceService
        );
    }

    void TearDown() override {
    }

    std::unique_ptr<BookingService> bookingService_;
    MockBookingRepository* mockBookingRepo_;
    MockClientRepository* mockClientRepo_;
    MockDanceHallRepository* mockHallRepo_;
    MockBranchRepository* mockBranchRepo_;
    MockBranchService* mockBranchService_;
    MockLessonRepository* mockLessonRepo_;
    MockAttendanceService* mockAttendanceService_;  

    // Вспомогательные методы остаются без изменений
    UUID createTestClientId() { return UUID::generate(); }
    UUID createTestHallId() { return UUID::generate(); }
    UUID createTestBookingId() { return UUID::generate(); }
    UUID createTestBranchId() { return UUID::generate(); }
    UUID createTestStudioId() { return UUID::generate(); }
    
    BookingRequestDTO createValidBookingRequest() {
        UUID clientId = createTestClientId();
        UUID hallId = createTestHallId();
        
        auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
        TimeSlot timeSlot(startTime, 60);
        
        return BookingRequestDTO{clientId, hallId, timeSlot, "Репетиция танцевальной группы"};
    }
    
    Client createTestClient(const UUID& id, bool active = true) {
        return Client(id, "Test Client", "test@email.com", "+74956743212");
    }
    
    DanceHall createTestHall(const UUID& id, const UUID& branchId = UUID::generate()) {
        return DanceHall(id, "Test Hall", 20, branchId);
    }
    
    Branch createTestBranch(const UUID& id) {
        using namespace std::chrono;
        UUID studioId = createTestStudioId();
        BranchAddress address(UUID::generate(), "Россия", "Москва", "ул. Тестовая", "10", hours(3));
        
        return Branch(id, "Test Branch", "+79255052590", WorkingHours{hours(9), hours(21)}, studioId, address);
    }

    Booking createTestBooking(const UUID& bookingId, const UUID& clientId, const UUID& hallId, 
                             const TimeSlot& timeSlot, const std::string& purpose = "Тест") {
        return Booking(bookingId, clientId, hallId, timeSlot, purpose);
    }

    Lesson createTestLesson(const UUID& id, const UUID& hallId, const TimeSlot& timeSlot, 
                       const std::string& name = "Тестовое занятие") {
        return Lesson(
            id, LessonType::OPEN_CLASS, name, timeSlot.getStartTime(), 
            timeSlot.getDurationMinutes(), DifficultyLevel::BEGINNER, 
            10, 100.0, createTestClientId(), hallId
        );
    }
};


TEST_F(BookingServiceTest, CreateBooking_ValidRequest_Success) {
    // Arrange
    auto request = createValidBookingRequest();
    auto client = createTestClient(request.clientId);
    auto hall = createTestHall(request.hallId);
    auto branch = createTestBranch(createTestBranchId());
    
    // Настраиваем ожидания для моков
    EXPECT_CALL(*mockClientRepo_, findById(request.clientId))
        .WillOnce(Return(client));
    EXPECT_CALL(*mockHallRepo_, exists(request.hallId))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBranchService_, getBranchForHall(request.hallId))
        .WillOnce(Return(branch));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(request.hallId, request.timeSlot))
        .WillOnce(Return(std::vector<Booking>{}));
    EXPECT_CALL(*mockLessonRepo_, findConflictingLessons(request.hallId, request.timeSlot))
        .WillOnce(Return(std::vector<Lesson>{}));
    EXPECT_CALL(*mockBookingRepo_, save(_))
        .WillOnce(Return(true));
    
    // Act
    auto response = bookingService_->createBooking(request);
    
    // Assert
    EXPECT_EQ(response.clientId, request.clientId);
    EXPECT_EQ(response.hallId, request.hallId);
    EXPECT_EQ(response.status, "CONFIRMED");
}

// Тест для completeBooking
TEST_F(BookingServiceTest, CompleteBooking_ValidBooking_Success) {
    // Arrange
    UUID bookingId = createTestBookingId();
    auto booking = createTestBooking(bookingId, createTestClientId(), createTestHallId(), 
                                    TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(24), 60));
    booking.confirm();
    
    EXPECT_CALL(*mockBookingRepo_, findById(bookingId))
        .WillOnce(Return(booking));
    EXPECT_CALL(*mockBookingRepo_, update(_))
        .WillOnce(Return(true));
    
    // Ожидаем вызов attendanceService
    EXPECT_CALL(*mockAttendanceService_, createAttendanceForBooking(bookingId, BookingStatus::COMPLETED, "Завершено"))
        .WillOnce(Return(true));
    
    // Act
    auto response = bookingService_->completeBooking(bookingId);
    
    // Assert
    EXPECT_EQ(response.status, "COMPLETED");
}

TEST_F(BookingServiceTest, IsTimeSlotAvailable_NoConflicts_ReturnsTrue) {
    // Arrange
    UUID hallId = createTestHallId();
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    TimeSlot timeSlot(startTime, 60);
    
    EXPECT_CALL(*mockHallRepo_, exists(hallId))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(hallId, timeSlot))
        .WillOnce(Return(std::vector<Booking>{}));
    EXPECT_CALL(*mockLessonRepo_, findConflictingLessons(hallId, timeSlot))
        .WillOnce(Return(std::vector<Lesson>{}));
    
    // Act
    bool isAvailable = bookingService_->isTimeSlotAvailable(hallId, timeSlot);
    
    // Assert
    EXPECT_TRUE(isAvailable);
}

// Тест получения бронирований клиента
TEST_F(BookingServiceTest, GetClientBookings_ClientExists_ReturnsBookings) {
    // Arrange
    UUID clientId = createTestClientId();
    auto client = createTestClient(clientId);
    
    std::vector<Booking> testBookings = {
        createTestBooking(createTestBookingId(), clientId, createTestHallId(), 
                TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(24), 60), "Репетиция")
    };
    
    EXPECT_CALL(*mockClientRepo_, findById(clientId))
        .WillOnce(Return(client));
    EXPECT_CALL(*mockBookingRepo_, findByClientId(clientId))
        .WillOnce(Return(testBookings));
    
    // Act
    auto bookings = bookingService_->getClientBookings(clientId);
    
    // Assert
    EXPECT_EQ(bookings.size(), 1);
}

// Тест получения всех залов
TEST_F(BookingServiceTest, GetAllHalls_HallsExist_ReturnsHalls) {
    // Arrange
    std::vector<DanceHall> testHalls = {
        createTestHall(createTestHallId()),
        createTestHall(createTestHallId())
    };
    
    EXPECT_CALL(*mockHallRepo_, findAll())
        .WillOnce(Return(testHalls));
    
    // Act
    auto halls = bookingService_->getAllHalls();
    
    // Assert
    EXPECT_EQ(halls.size(), 2);
}

// Тест получения залов по филиалу
TEST_F(BookingServiceTest, GetHallsByBranch_BranchExists_ReturnsHalls) {
    // Arrange
    UUID branchId = createTestBranchId();
    std::vector<DanceHall> testHalls = {
        createTestHall(createTestHallId(), branchId),
        createTestHall(createTestHallId(), branchId)
    };
    
    EXPECT_CALL(*mockBranchService_, getHallsByBranch(branchId))
        .WillOnce(Return(testHalls));
    
    // Act
    auto halls = bookingService_->getHallsByBranch(branchId);
    
    // Assert
    EXPECT_EQ(halls.size(), 2);
}

// Тест получения всех филиалов
TEST_F(BookingServiceTest, GetAllBranches_BranchesExist_ReturnsBranches) {
    // Arrange
    std::vector<Branch> testBranches = {
        createTestBranch(createTestBranchId()),
        createTestBranch(createTestBranchId())
    };
    
    EXPECT_CALL(*mockBranchService_, getAllBranches())
        .WillOnce(Return(testBranches));
    
    // Act
    auto branches = bookingService_->getAllBranches();
    
    // Assert
    EXPECT_EQ(branches.size(), 2);
}

// Тест создания бронирования с конфликтом времени
TEST_F(BookingServiceTest, CreateBooking_TimeConflict_ThrowsException) {
    // Arrange
    auto request = createValidBookingRequest();
    auto client = createTestClient(request.clientId);
    auto hall = createTestHall(request.hallId);
    auto branch = createTestBranch(createTestBranchId());
    
    std::vector<Booking> conflictingBookings = {
        createTestBooking(createTestBookingId(), createTestClientId(), request.hallId, 
                request.timeSlot, "Другая репетиция")
    };
    
    EXPECT_CALL(*mockClientRepo_, findById(request.clientId))
        .WillOnce(Return(client));
    EXPECT_CALL(*mockHallRepo_, exists(request.hallId))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBranchService_, getBranchForHall(request.hallId))
        .WillOnce(Return(branch));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(request.hallId, request.timeSlot))
        .WillOnce(Return(conflictingBookings));
    // Не ожидаем вызов findConflictingLessons, так как уже есть конфликт с бронированием
    
    // Act & Assert
    EXPECT_THROW(bookingService_->createBooking(request), BookingConflictException);
}

// Тест создания бронирования с конфликтом занятия
TEST_F(BookingServiceTest, CreateBooking_LessonConflict_ThrowsException) {
    // Arrange
    auto request = createValidBookingRequest();
    auto client = createTestClient(request.clientId);
    auto hall = createTestHall(request.hallId);
    auto branch = createTestBranch(createTestBranchId());
    
    std::vector<Lesson> conflictingLessons = {
        createTestLesson(createTestBookingId(), request.hallId, request.timeSlot, "Танцевальный класс")
    };
    
    EXPECT_CALL(*mockClientRepo_, findById(request.clientId))
        .WillOnce(Return(client));
    EXPECT_CALL(*mockHallRepo_, exists(request.hallId))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBranchService_, getBranchForHall(request.hallId))
        .WillOnce(Return(branch));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(request.hallId, request.timeSlot))
        .WillOnce(Return(std::vector<Booking>{}));
    EXPECT_CALL(*mockLessonRepo_, findConflictingLessons(request.hallId, request.timeSlot))
        .WillOnce(Return(conflictingLessons));
    
    // Act & Assert
    EXPECT_THROW(bookingService_->createBooking(request), BookingConflictException);
}

// Тест создания бронирования с несуществующим клиентом
TEST_F(BookingServiceTest, CreateBooking_InvalidClient_ThrowsException) {
    // Arrange
    auto request = createValidBookingRequest();
    
    EXPECT_CALL(*mockClientRepo_, findById(request.clientId))
        .WillOnce(Return(std::nullopt)); // Клиент не найден
    
    // Act & Assert
    EXPECT_THROW(bookingService_->createBooking(request), ValidationException);
}

// Тест создания бронирования с несуществующим залом
TEST_F(BookingServiceTest, CreateBooking_InvalidHall_ThrowsException) {
    // Arrange
    auto request = createValidBookingRequest();
    auto client = createTestClient(request.clientId);
    
    EXPECT_CALL(*mockClientRepo_, findById(request.clientId))
        .WillOnce(Return(client));
    EXPECT_CALL(*mockHallRepo_, exists(request.hallId))
        .WillOnce(Return(false)); // Зал не существует
    
    // Act & Assert
    EXPECT_THROW(bookingService_->createBooking(request), ValidationException);
}

// Тест отмены бронирования
TEST_F(BookingServiceTest, CancelBooking_ValidBooking_Success) {
    // Arrange
    UUID bookingId = createTestBookingId();
    UUID clientId = createTestClientId();
    auto booking = createTestBooking(bookingId, clientId, createTestHallId(), 
                                    TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(24), 60));
    booking.confirm(); // Подтверждаем бронирование
    
    EXPECT_CALL(*mockBookingRepo_, findById(bookingId))
        .WillOnce(Return(booking));
    EXPECT_CALL(*mockBookingRepo_, update(_))
        .WillOnce(Return(true));
    
    // Act
    auto response = bookingService_->cancelBooking(bookingId, clientId);
    
    // Assert
    EXPECT_EQ(response.status, "CANCELLED");
}

// Тест получения информации о бронировании
TEST_F(BookingServiceTest, GetBooking_ValidBooking_ReturnsBooking) {
    // Arrange
    UUID bookingId = createTestBookingId();
    auto booking = createTestBooking(bookingId, createTestClientId(), createTestHallId(), 
                                    TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(24), 60));
    booking.confirm();
    
    EXPECT_CALL(*mockBookingRepo_, findById(bookingId))
        .WillOnce(Return(booking));
    
    // Act
    auto response = bookingService_->getBooking(bookingId);
    
    // Assert
    EXPECT_EQ(response.bookingId, bookingId);
    EXPECT_EQ(response.status, "CONFIRMED");
}

// Тест проверки возможности бронирования клиентом
TEST_F(BookingServiceTest, CanClientBook_WithinLimit_ReturnsTrue) {
    // Arrange
    UUID clientId = createTestClientId();
    std::vector<Booking> activeBookings = {
        createTestBooking(createTestBookingId(), clientId, createTestHallId(), 
                         TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(24), 60))
    };
    
    EXPECT_CALL(*mockBookingRepo_, findByClientId(clientId))
        .WillOnce(Return(activeBookings));
    
    // Act
    bool canBook = bookingService_->canClientBook(clientId);
    
    // Assert
    EXPECT_TRUE(canBook);
}

// Тест проверки возможности бронирования клиентом (превышен лимит)
TEST_F(BookingServiceTest, CanClientBook_ExceedsLimit_ReturnsFalse) {
    // Arrange
    UUID clientId = createTestClientId();
    std::vector<Booking> activeBookings = {
        createTestBooking(createTestBookingId(), clientId, createTestHallId(), 
                         TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(24), 60)),
        createTestBooking(createTestBookingId(), clientId, createTestHallId(), 
                         TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(48), 60)),
        createTestBooking(createTestBookingId(), clientId, createTestHallId(), 
                         TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(72), 60))
    };
    
    EXPECT_CALL(*mockBookingRepo_, findByClientId(clientId))
        .WillOnce(Return(activeBookings));
    
    // Act
    bool canBook = bookingService_->canClientBook(clientId);
    
    // Assert
    EXPECT_FALSE(canBook);
}