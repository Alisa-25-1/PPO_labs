#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/BookingService.hpp"
#include "mocks/MockBookingRepository.hpp"
#include "mocks/MockClientRepository.hpp"
#include "mocks/MockDanceHallRepository.hpp"
#include "mocks/MockBranchRepository.hpp"
#include "mocks/MockBranchService.hpp"
#include "mocks/MockAttendanceRepository.hpp" // ДОБАВЛЕНО
#include "../../services/exceptions/BookingException.hpp"
#include "../../services/exceptions/ValidationException.hpp"

using namespace testing;

class BookingServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем моки как shared_ptr (совместимо с BookingService)
        auto mockBookingRepo = std::make_shared<MockBookingRepository>();
        auto mockClientRepo = std::make_shared<MockClientRepository>();
        auto mockHallRepo = std::make_shared<MockDanceHallRepository>();
        auto mockBranchRepo = std::make_shared<MockBranchRepository>();
        auto mockBranchService = std::make_shared<MockBranchService>();
        auto mockAttendanceRepo = std::make_shared<MockAttendanceRepository>();

        // Сохраняем указатели для доступа в тестах
        mockBookingRepo_ = mockBookingRepo.get();
        mockClientRepo_ = mockClientRepo.get();
        mockHallRepo_ = mockHallRepo.get();
        mockBranchRepo_ = mockBranchRepo.get();
        mockBranchService_ = mockBranchService.get();
        mockAttendanceRepo_ = mockAttendanceRepo.get();

        // Создаем сервис с передачей всех моков как shared_ptr (6 параметров)
        bookingService_ = std::make_unique<BookingService>(
            mockBookingRepo,
            mockClientRepo,
            mockHallRepo,
            mockBranchRepo,
            mockBranchService,
            mockAttendanceRepo // ДОБАВЛЕНО
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
    MockAttendanceRepository* mockAttendanceRepo_; // ДОБАВЛЕНО

    // Вспомогательные методы для создания тестовых данных
    UUID createTestClientId() { return UUID::generate(); }
    UUID createTestHallId() { return UUID::generate(); }
    UUID createTestBookingId() { return UUID::generate(); }
    UUID createTestBranchId() { return UUID::generate(); }
    UUID createTestStudioId() { return UUID::generate(); }
    
    BookingRequestDTO createValidBookingRequest() {
        UUID clientId = createTestClientId();
        UUID hallId = createTestHallId();
        
        auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
        TimeSlot timeSlot(startTime, 60); // 1 час
        
        return BookingRequestDTO{clientId, hallId, timeSlot, "Репетиция танцевальной группы"};
    }
    
    // ИСПРАВЛЕНО: правильный конструктор Client (4 параметра)
    Client createTestClient(const UUID& id, bool active = true) {
        return Client(id, "Test Client", "test@email.com", "+74956743212");
    }
    
    // ИСПРАВЛЕНО: правильный конструктор DanceHall (4 параметра)
    DanceHall createTestHall(const UUID& id, const UUID& branchId = UUID::generate()) {
        return DanceHall(id, "Test Hall", 20, branchId);
    }
    
    Branch createTestBranch(const UUID& id) {
        using namespace std::chrono;
        UUID studioId = createTestStudioId();
        return Branch(id, "Test Branch", "Test Address", "+123456789", 
                     WorkingHours{hours(9), hours(21)}, studioId);
    }
};

// Базовый тест создания бронирования
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
    EXPECT_CALL(*mockBookingRepo_, save(_))
        .WillOnce(Return(true));
    
    // Также ожидаем создание записи посещаемости
    EXPECT_CALL(*mockAttendanceRepo_, save(_))
        .WillOnce(Return(true));
    
    // Act
    auto response = bookingService_->createBooking(request);
    
    // Assert
    EXPECT_EQ(response.clientId, request.clientId);
    EXPECT_EQ(response.hallId, request.hallId);
    EXPECT_EQ(response.status, "CONFIRMED");
}

// Тест проверки доступности временного слота
TEST_F(BookingServiceTest, IsTimeSlotAvailable_NoConflicts_ReturnsTrue) {
    // Arrange
    UUID hallId = createTestHallId();
    auto startTime = std::chrono::system_clock::now() + std::chrono::hours(24);
    TimeSlot timeSlot(startTime, 60);
    
    EXPECT_CALL(*mockHallRepo_, exists(hallId))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(hallId, timeSlot))
        .WillOnce(Return(std::vector<Booking>{}));
    
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
        Booking(createTestBookingId(), clientId, createTestHallId(), 
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
        Booking(createTestBookingId(), createTestClientId(), request.hallId, 
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