#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../services/BookingService.hpp"
#include "mocks/MockBookingRepository.hpp"
#include "mocks/MockClientRepository.hpp"
#include "mocks/MockDanceHallRepository.hpp"  // Исправлено

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;

class BookingServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализируем UUID
        testClientId_ = UUID::fromString("11111111-1111-1111-1111-111111111111");
        testHallId_ = UUID::fromString("22222222-2222-2222-2222-222222222222");
        testBookingId_ = UUID::fromString("33333333-3333-3333-3333-333333333333");
        testBranchId_ = UUID::fromString("44444444-4444-4444-4444-444444444444");
        otherClientId_ = UUID::fromString("55555555-5555-5555-5555-555555555555");
        nonExistentId_ = UUID::fromString("99999999-9999-9999-9999-999999999999");

        // Создаем моки
        auto mockBookingRepo = std::make_unique<NiceMock<MockBookingRepository>>();
        auto mockClientRepo = std::make_unique<NiceMock<MockClientRepository>>();
        auto mockHallRepo = std::make_unique<NiceMock<MockDanceHallRepository>>();  // Исправлено

        mockBookingRepo_ = mockBookingRepo.get();
        mockClientRepo_ = mockClientRepo.get();
        mockHallRepo_ = mockHallRepo.get();

        // Создаем сервис
        bookingService_ = std::make_unique<BookingService>(
            std::move(mockBookingRepo),
            std::move(mockClientRepo),
            std::move(mockHallRepo)
        );

        // Создаем объекты через указатели (отложенная инициализация)
        testClient_ = std::make_unique<Client>(testClientId_, "John Doe", "john@example.com", "+12345678901");
        testHall_ = std::make_unique<DanceHall>(testHallId_, "Main Hall", 50, testBranchId_);  // Исправлено на DanceHall
        testTimeSlot_ = std::make_unique<TimeSlot>(std::chrono::system_clock::now() + std::chrono::hours(1), 120);
        testBooking_ = std::make_unique<Booking>(testBookingId_, testClientId_, testHallId_, *testTimeSlot_, "Test practice session");
        testBooking_->confirm();
    }

    void TearDown() override {
        // Очищаем объекты в обратном порядке
        testBooking_.reset();
        testTimeSlot_.reset();
        testHall_.reset();
        testClient_.reset();
        bookingService_.reset();
    }

    // Поля, которые инициализируются в SetUp()
    std::unique_ptr<BookingService> bookingService_;
    
    UUID testClientId_;
    UUID testHallId_;
    UUID testBookingId_;
    UUID testBranchId_;
    UUID otherClientId_;
    UUID nonExistentId_;
    
    // Объекты как unique_ptr для отложенной инициализации
    std::unique_ptr<Client> testClient_;
    std::unique_ptr<DanceHall> testHall_;  // Исправлено на DanceHall
    std::unique_ptr<TimeSlot> testTimeSlot_;
    std::unique_ptr<Booking> testBooking_;

    // Указатели на моки
    MockBookingRepository* mockBookingRepo_;
    MockClientRepository* mockClientRepo_;
    MockDanceHallRepository* mockHallRepo_;  // Исправлено
};

TEST_F(BookingServiceTest, CreateBooking_Success) {
    BookingRequestDTO request(testClientId_, testHallId_, *testTimeSlot_, "Dance practice session");
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockHallRepo_, exists(testHallId_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(testHallId_, *testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{}));
    EXPECT_CALL(*mockBookingRepo_, findByClientId(testClientId_))
        .WillOnce(Return(std::vector<Booking>{}));
    EXPECT_CALL(*mockBookingRepo_, save(_))
        .WillOnce(Return(true));

    auto result = bookingService_->createBooking(request);

    EXPECT_EQ(result.clientId, testClientId_);
    EXPECT_EQ(result.hallId, testHallId_);
    EXPECT_EQ(result.status, "CONFIRMED");
}

TEST_F(BookingServiceTest, CreateBooking_TimeConflict) {
    BookingRequestDTO request(testClientId_, testHallId_, *testTimeSlot_, "Dance practice session");
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockHallRepo_, exists(testHallId_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(testHallId_, *testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{*testBooking_}));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        BookingConflictException
    );
}

TEST_F(BookingServiceTest, CreateBooking_ClientNotFound) {
    BookingRequestDTO request(nonExistentId_, testHallId_, *testTimeSlot_, "Dance practice session");
    
    EXPECT_CALL(*mockClientRepo_, findById(nonExistentId_))
        .WillOnce(Return(std::optional<Client>{}));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        ValidationException
    );
}

TEST_F(BookingServiceTest, CreateBooking_HallNotFound) {
    BookingRequestDTO request(testClientId_, nonExistentId_, *testTimeSlot_, "Dance practice session");
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(*testClient_));
    EXPECT_CALL(*mockHallRepo_, exists(nonExistentId_))
        .WillOnce(Return(false));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        ValidationException
    );
}

TEST_F(BookingServiceTest, CreateBooking_InactiveClient) {
    // Создаем неактивного клиента специально для этого теста
    auto inactiveClient = Client(testClientId_, "Inactive John", "inactive@example.com", "+12345678901");
    inactiveClient.deactivate();
    
    BookingRequestDTO request(testClientId_, testHallId_, *testTimeSlot_, "Dance practice session");
    
    EXPECT_CALL(*mockClientRepo_, findById(testClientId_))
        .WillOnce(Return(inactiveClient));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        BusinessRuleException
    );
}

TEST_F(BookingServiceTest, CancelBooking_Success) {
    EXPECT_CALL(*mockBookingRepo_, findById(testBookingId_))
        .WillOnce(Return(*testBooking_));
    EXPECT_CALL(*mockBookingRepo_, update(_))
        .WillOnce(Return(true));

    auto result = bookingService_->cancelBooking(testBookingId_, testClientId_);

    EXPECT_EQ(result.status, "CANCELLED");
}

TEST_F(BookingServiceTest, CancelBooking_NotFound) {
    EXPECT_CALL(*mockBookingRepo_, findById(nonExistentId_))
        .WillOnce(Return(std::optional<Booking>{}));

    EXPECT_THROW(
        bookingService_->cancelBooking(nonExistentId_, testClientId_),
        BookingNotFoundException
    );
}

TEST_F(BookingServiceTest, CancelBooking_UnauthorizedClient) {
    EXPECT_CALL(*mockBookingRepo_, findById(testBookingId_))
        .WillOnce(Return(*testBooking_));

    EXPECT_THROW(
        bookingService_->cancelBooking(testBookingId_, otherClientId_),
        BusinessRuleException
    );
}

TEST_F(BookingServiceTest, CanClientBook_MaximumBookingsReached) {
    // Создаем 3 активных бронирования
    std::vector<Booking> activeBookings;
    for (int i = 0; i < 3; i++) {
        UUID bookingId = UUID::fromString("77777777-7777-7777-7777-77777777777" + std::to_string(i));
        Booking booking(bookingId, testClientId_, testHallId_, *testTimeSlot_, "Practice session " + std::to_string(i));
        booking.confirm();
        activeBookings.push_back(booking);
    }
    
    EXPECT_CALL(*mockBookingRepo_, findByClientId(testClientId_))
        .WillOnce(Return(activeBookings));

    EXPECT_FALSE(bookingService_->canClientBook(testClientId_));
}

TEST_F(BookingServiceTest, IsTimeSlotAvailable_Available) {
    EXPECT_CALL(*mockHallRepo_, exists(testHallId_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(testHallId_, *testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{}));

    EXPECT_TRUE(bookingService_->isTimeSlotAvailable(testHallId_, *testTimeSlot_));
}

TEST_F(BookingServiceTest, IsTimeSlotAvailable_NotAvailable) {
    EXPECT_CALL(*mockHallRepo_, exists(testHallId_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(testHallId_, *testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{*testBooking_}));

    EXPECT_FALSE(bookingService_->isTimeSlotAvailable(testHallId_, *testTimeSlot_));
}

TEST_F(BookingServiceTest, CreateBooking_InvalidPurpose) {
    // Пустая цель должна вызвать исключение
    BookingRequestDTO request(testClientId_, testHallId_, *testTimeSlot_, "");
    
    EXPECT_THROW(
        bookingService_->createBooking(request),
        ValidationException
    );
}

// Добавляем тест для метода getDanceHallBookings
TEST_F(BookingServiceTest, GetDanceHallBookings_Success) {
    std::vector<Booking> hallBookings = {*testBooking_};
    
    EXPECT_CALL(*mockHallRepo_, exists(testHallId_))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findByHallId(testHallId_))
        .WillOnce(Return(hallBookings));

    auto result = bookingService_->getDanceHallBookings(testHallId_);

    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].hallId, testHallId_);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}