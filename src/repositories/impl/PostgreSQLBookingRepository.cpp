#include "PostgreSQLBookingRepository.hpp"
#include <pqxx/pqxx>
#include "../../data/DateTimeUtils.hpp"

PostgreSQLBookingRepository::PostgreSQLBookingRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Booking> PostgreSQLBookingRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at "
            "FROM bookings WHERE id = $1";
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto booking = mapResultToBooking(result[0]);
        dbConnection_->commitTransaction(work);
        return booking;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find booking by ID: ") + e.what());
    }
}

std::vector<Booking> PostgreSQLBookingRepository::findByClientId(const UUID& clientId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at "
            "FROM bookings WHERE client_id = $1";
        
        auto result = work.exec_params(query, clientId.toString());
        
        std::vector<Booking> bookings;
        for (const auto& row : result) {
            bookings.push_back(mapResultToBooking(row));
        }
        
        dbConnection_->commitTransaction(work);
        return bookings;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find bookings by client ID: ") + e.what());
    }
}

std::vector<Booking> PostgreSQLBookingRepository::findByHallId(const UUID& hallId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "SELECT id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at "
            "FROM bookings WHERE hall_id = $1";
        
        auto result = work.exec_params(query, hallId.toString());
        
        std::vector<Booking> bookings;
        for (const auto& row : result) {
            bookings.push_back(mapResultToBooking(row));
        }
        
        dbConnection_->commitTransaction(work);
        return bookings;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find bookings by hall ID: ") + e.what());
    }
}

std::vector<Booking> PostgreSQLBookingRepository::findConflictingBookings(
    const UUID& hallId, const TimeSlot& timeSlot) {
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        // Правильный запрос с явным приведением типов
        std::string query = 
            "SELECT id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at "
            "FROM bookings "
            "WHERE hall_id = $1 AND status IN ('PENDING', 'CONFIRMED') "
            "AND (start_time, start_time + (duration_minutes * interval '1 minute')) "
            "OVERLAPS ($2::timestamp, $2::timestamp + ($3 * interval '1 minute'))";
        
        auto startTimeStr = DateTimeUtils::formatTimeForPostgres(timeSlot.getStartTime());
        auto result = work.exec_params(
            query, 
            hallId.toString(),
            startTimeStr,
            timeSlot.getDurationMinutes()
        );
        
        std::vector<Booking> bookings;
        for (const auto& row : result) {
            bookings.push_back(mapResultToBooking(row));
        }
        
        dbConnection_->commitTransaction(work);
        return bookings;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find conflicting bookings: ") + e.what());
    }
}

bool PostgreSQLBookingRepository::save(const Booking& booking) {
    validateBooking(booking);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "INSERT INTO bookings (id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
        
        work.exec_params(
            query,
            booking.getId().toString(),
            booking.getClientId().toString(),
            booking.getHallId().toString(),
            DateTimeUtils::formatTimeForPostgres(booking.getTimeSlot().getStartTime()),
            booking.getTimeSlot().getDurationMinutes(),
            booking.getPurpose(),
            bookingStatusToString(booking.getStatus()),
            DateTimeUtils::formatTimeForPostgres(booking.getCreatedAt())
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save booking: ") + e.what());
    }
}

bool PostgreSQLBookingRepository::update(const Booking& booking) {
    validateBooking(booking);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = 
            "UPDATE bookings SET client_id = $2, hall_id = $3, start_time = $4, "
            "duration_minutes = $5, purpose = $6, status = $7 "
            "WHERE id = $1";
        
        auto result = work.exec_params(
            query,
            booking.getId().toString(),
            booking.getClientId().toString(),
            booking.getHallId().toString(),
            DateTimeUtils::formatTimeForPostgres(booking.getTimeSlot().getStartTime()),
            booking.getTimeSlot().getDurationMinutes(),
            booking.getPurpose(),
            bookingStatusToString(booking.getStatus())
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update booking: ") + e.what());
    }
}

bool PostgreSQLBookingRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "DELETE FROM bookings WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove booking: ") + e.what());
    }
}

bool PostgreSQLBookingRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = "SELECT 1 FROM bookings WHERE id = $1";
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check booking existence: ") + e.what());
    }
}

Booking PostgreSQLBookingRepository::mapResultToBooking(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    UUID clientId = UUID::fromString(row["client_id"].c_str());
    UUID hallId = UUID::fromString(row["hall_id"].c_str());
    
    // Используем утилиту для парсинга времени
    auto startTime = DateTimeUtils::parseTimeFromPostgres(row["start_time"].c_str());
    int duration = row["duration_minutes"].as<int>();
    TimeSlot timeSlot(startTime, duration);
    
    std::string purpose = row["purpose"].c_str();
    BookingStatus status = stringToBookingStatus(row["status"].c_str());
    
    Booking booking(id, clientId, hallId, timeSlot, purpose);
    
    // Восстанавливаем статус
    switch (status) {
        case BookingStatus::CONFIRMED:
            booking.confirm();
            break;
        case BookingStatus::CANCELLED:
            booking.cancel();
            break;
        case BookingStatus::COMPLETED:
            booking.complete();
            break;
        default:
            break; // PENDING по умолчанию
    }
    
    return booking;
}

std::string PostgreSQLBookingRepository::bookingStatusToString(BookingStatus status) const {
    switch (status) {
        case BookingStatus::PENDING: return "PENDING";
        case BookingStatus::CONFIRMED: return "CONFIRMED";
        case BookingStatus::CANCELLED: return "CANCELLED";
        case BookingStatus::COMPLETED: return "COMPLETED";
        default: return "PENDING";
    }
}

BookingStatus PostgreSQLBookingRepository::stringToBookingStatus(const std::string& status) const {
    if (status == "CONFIRMED") return BookingStatus::CONFIRMED;
    if (status == "CANCELLED") return BookingStatus::CANCELLED;
    if (status == "COMPLETED") return BookingStatus::COMPLETED;
    return BookingStatus::PENDING;
}

void PostgreSQLBookingRepository::validateBooking(const Booking& booking) const {
    if (!booking.isValid()) {
        throw DataAccessException("Invalid booking data");
    }
}