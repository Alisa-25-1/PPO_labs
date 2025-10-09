#ifndef POSTGRESQL_BOOKING_REPOSITORY_HPP
#define POSTGRESQL_BOOKING_REPOSITORY_HPP

#include "../IBookingRepository.hpp"
#include "../../data/DatabaseConnection.hpp"           
#include "../../data/exceptions/DataAccessException.hpp" 


#include <memory>

class PostgreSQLBookingRepository : public IBookingRepository {
public:
    explicit PostgreSQLBookingRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Booking> findById(const UUID& id) override;
    std::vector<Booking> findByClientId(const UUID& clientId) override;
    std::vector<Booking> findByHallId(const UUID& hallId) override;
    std::vector<Booking> findConflictingBookings(const UUID& hallId, const TimeSlot& timeSlot) override;
    bool save(const Booking& booking) override;
    bool update(const Booking& booking) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Booking mapResultToBooking(const pqxx::row& row) const;
    TimeSlot mapResultToTimeSlot(const pqxx::row& row) const;
    std::string bookingStatusToString(BookingStatus status) const;
    BookingStatus stringToBookingStatus(const std::string& status) const;
    void validateBooking(const Booking& booking) const;
};

#endif // POSTGRESQL_BOOKING_REPOSITORY_HPP