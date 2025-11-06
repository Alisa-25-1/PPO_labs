#pragma once

#include "../types/uuid.hpp"
#include "../models/Attendance.hpp"
#include <chrono>
#include <vector>
#include <optional>

class IAttendanceService {
public:
    virtual ~IAttendanceService() = default;
    
    virtual bool createAttendanceForBooking(const UUID& bookingId, BookingStatus newStatus, const std::string& notes = "") = 0;
    virtual bool createAttendanceForEnrollment(const UUID& enrollmentId, EnrollmentStatus newStatus, const std::string& notes = "") = 0;
    
    virtual bool markBookingVisited(const UUID& bookingId, const std::string& notes = "") = 0;
    virtual bool markBookingCancelled(const UUID& bookingId, const std::string& notes = "") = 0;
    virtual bool markBookingNoShow(const UUID& bookingId, const std::string& notes = "") = 0;
    
    virtual bool markLessonVisited(const UUID& enrollmentId, const std::string& notes = "") = 0;
    virtual bool markLessonCancelled(const UUID& enrollmentId, const std::string& notes = "") = 0;
    virtual bool markLessonNoShow(const UUID& enrollmentId, const std::string& notes = "") = 0;

    virtual std::vector<Attendance> getClientAttendance(const UUID& clientId, 
                                               const std::chrono::system_clock::time_point& startDate,
                                               const std::chrono::system_clock::time_point& endDate) const = 0;

    virtual std::vector<Attendance> getAttendanceByTypeAndStatus(AttendanceType type, AttendanceStatus status) const = 0;

    virtual bool updateAttendanceNotes(const UUID& attendanceId, const std::string& notes) = 0;
};
