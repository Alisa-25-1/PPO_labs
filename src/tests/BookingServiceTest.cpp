#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../services/BookingService.hpp"
#include "mocks/MockBookingRepository.hpp"
#include "mocks/MockClientRepository.hpp"
#include "mocks/MockHallRepository.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;


class BookingServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create mock repositories
        auto mockBookingRepo = std::make_unique<NiceMock<MockBookingRepository>>();
        auto mockClientRepo = std::make_unique<NiceMock<MockClientRepository>>();
        auto mockHallRepo = std::make_unique<NiceMock<MockHallRepository>>();

        // Store raw pointers for setting expectations
        mockBookingRepo_ = mockBookingRepo.get();
        mockClientRepo_ = mockClientRepo.get();
        mockHallRepo_ = mockHallRepo.get();

        // Create service with injected mocks
        bookingService_ = std::make_unique<BookingService>(
            std::move(mockBookingRepo),
            std::move(mockClientRepo),
            std::move(mockHallRepo)
        );

        // Setup common test data
        testClient_ = Client(1, "John Doe", "john@test.com", "+123456789");
        testHall_ = Hall(1, "Main Hall", 50, 1);
        testTimeSlot_ = TimeSlot(std::chrono::system_clock::now() + std::chrono::hours(1), 120);
    }

    std::unique_ptr<BookingService> bookingService_;
    MockBookingRepository* mockBookingRepo_;
    MockClientRepository* mockClientRepo_;
    MockHallRepository* mockHallRepo_;
    
    Client testClient_;
    Hall testHall_;
    TimeSlot testTimeSlot_;
};

// Test successful booking creation
TEST_F(BookingServiceTest, CreateBooking_Success) {
    BookingRequestDTO request(1, 1, testTimeSlot_, "Dance practice");
    
    // Setup expectations
    EXPECT_CALL(*mockClientRepo_, findById(1))
        .WillOnce(Return(testClient_));
    EXPECT_CALL(*mockHallRepo_, exists(1))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(1, testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{})); // No conflicts
    EXPECT_CALL(*mockBookingRepo_, findByClientId(1))
        .WillOnce(Return(std::vector<Booking>{})); // No existing bookings
    EXPECT_CALL(*mockBookingRepo_, save(_))
        .WillOnce(Return(true));

    // Execute
    auto result = bookingService_->createBooking(request);

    // Verify
    EXPECT_EQ(result.clientId, 1);
    EXPECT_EQ(result.hallId, 1);
    EXPECT_EQ(result.status, "CONFIRMED");
}

// Test booking creation with client not found
TEST_F(BookingServiceTest, CreateBooking_ClientNotFound) {
    BookingRequestDTO request(999, 1, testTimeSlot_, "Dance practice");
    
    EXPECT_CALL(*mockClientRepo_, findById(999))
        .WillOnce(Return(std::optional<Client>{}));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        ValidationException
    );
}

// Test booking creation with hall not found
TEST_F(BookingServiceTest, CreateBooking_HallNotFound) {
    BookingRequestDTO request(1, 999, testTimeSlot_, "Dance practice");
    
    EXPECT_CALL(*mockClientRepo_, findById(1))
        .WillOnce(Return(testClient_));
    EXPECT_CALL(*mockHallRepo_, exists(999))
        .WillOnce(Return(false));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        ValidationException
    );
}

// Test booking creation with time conflict
TEST_F(BookingServiceTest, CreateBooking_TimeConflict) {
    BookingRequestDTO request(1, 1, testTimeSlot_, "Dance practice");
    Booking conflictingBooking(2, 2, 1, testTimeSlot_, "Other practice");
    
    EXPECT_CALL(*mockClientRepo_, findById(1))
        .WillOnce(Return(testClient_));
    EXPECT_CALL(*mockHallRepo_, exists(1))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(1, testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{conflictingBooking}));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        BookingConflictException
    );
}

// Test booking creation with inactive client
TEST_F(BookingServiceTest, CreateBooking_InactiveClient) {
    Client inactiveClient = testClient_;
    inactiveClient.deactivate();
    
    BookingRequestDTO request(1, 1, testTimeSlot_, "Dance practice");
    
    EXPECT_CALL(*mockClientRepo_, findById(1))
        .WillOnce(Return(inactiveClient));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        BusinessRuleException
    );
}

// Test booking cancellation success
TEST_F(BookingServiceTest, CancelBooking_Success) {
    Booking activeBooking(1, 1, 1, testTimeSlot_, "Dance practice");
    activeBooking.confirm();
    
    EXPECT_CALL(*mockBookingRepo_, findById(1))
        .WillOnce(Return(activeBooking));
    EXPECT_CALL(*mockBookingRepo_, update(_))
        .WillOnce(Return(true));

    auto result = bookingService_->cancelBooking(1, 1);

    EXPECT_EQ(result.status, "CANCELLED");
}

// Test booking cancellation - not found
TEST_F(BookingServiceTest, CancelBooking_NotFound) {
    EXPECT_CALL(*mockBookingRepo_, findById(999))
        .WillOnce(Return(std::optional<Booking>{}));

    EXPECT_THROW(
        bookingService_->cancelBooking(999, 1),
        BookingNotFoundException
    );
}

// Test booking cancellation - unauthorized client
TEST_F(BookingServiceTest, CancelBooking_UnauthorizedClient) {
    Booking activeBooking(1, 1, 1, testTimeSlot_, "Dance practice");
    activeBooking.confirm();
    
    EXPECT_CALL(*mockBookingRepo_, findById(1))
        .WillOnce(Return(activeBooking));

    EXPECT_THROW(
        bookingService_->cancelBooking(1, 2), // Different client ID
        BusinessRuleException
    );
}

// Test business rule - maximum 3 active bookings per client
TEST_F(BookingServiceTest, CanClientBook_MaximumBookingsReached) {
    // Create 3 active bookings
    std::vector<Booking> activeBookings;
    for (int i = 1; i <= 3; i++) {
        Booking booking(i, 1, 1, testTimeSlot_, "Practice " + std::to_string(i));
        booking.confirm();
        activeBookings.push_back(booking);
    }
    
    EXPECT_CALL(*mockBookingRepo_, findByClientId(1))
        .WillOnce(Return(activeBookings));

    EXPECT_FALSE(bookingService_->canClientBook(1));
}

// Test time slot availability check
TEST_F(BookingServiceTest, IsTimeSlotAvailable_Available) {
    EXPECT_CALL(*mockHallRepo_, exists(1))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(1, testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{}));

    EXPECT_TRUE(bookingService_->isTimeSlotAvailable(1, testTimeSlot_));
}

// Test time slot availability check - not available
TEST_F(BookingServiceTest, IsTimeSlotAvailable_NotAvailable) {
    Booking conflictingBooking(1, 1, 1, testTimeSlot_, "Practice");
    
    EXPECT_CALL(*mockHallRepo_, exists(1))
        .WillOnce(Return(true));
    EXPECT_CALL(*mockBookingRepo_, findConflictingBookings(1, testTimeSlot_))
        .WillOnce(Return(std::vector<Booking>{conflictingBooking}));

    EXPECT_FALSE(bookingService_->isTimeSlotAvailable(1, testTimeSlot_));
}

// Test validation of booking purpose
TEST_F(BookingServiceTest, CreateBooking_InvalidPurpose) {
    BookingRequestDTO request(1, 1, testTimeSlot_, ""); // Empty purpose
    
    EXPECT_THROW(
        bookingService_->createBooking(request),
        ValidationException
    );
}

// Test past booking validation
TEST_F(BookingServiceTest, CreateBooking_PastTimeSlot) {
    auto pastTime = std::chrono::system_clock::now() - std::chrono::hours(1);
    TimeSlot pastTimeSlot(pastTime, 60);
    BookingRequestDTO request(1, 1, pastTimeSlot, "Practice");
    
    EXPECT_CALL(*mockClientRepo_, findById(1))
        .WillOnce(Return(testClient_));
    EXPECT_CALL(*mockHallRepo_, exists(1))
        .WillOnce(Return(true));

    EXPECT_THROW(
        bookingService_->createBooking(request),
        BusinessRuleException
    );
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}