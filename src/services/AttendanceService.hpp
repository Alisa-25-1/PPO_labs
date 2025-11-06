#ifndef ATTENDANCE_SERVICE_HPP
#define ATTENDANCE_SERVICE_HPP

#include "IAttendanceService.hpp"  
#include "../repositories/IAttendanceRepository.hpp"
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/IEnrollmentRepository.hpp"
#include "../repositories/ILessonRepository.hpp"
#include "../types/uuid.hpp"
#include <memory>
#include <vector>
#include <optional>

class AttendanceService: public IAttendanceService {
private:
    std::shared_ptr<IAttendanceRepository> attendanceRepo_;
    std::shared_ptr<IBookingRepository> bookingRepo_;
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo_;
    std::shared_ptr<ILessonRepository> lessonRepo_;

public:
    AttendanceService(
        std::shared_ptr<IAttendanceRepository> attendanceRepo,
        std::shared_ptr<IBookingRepository> bookingRepo,
        std::shared_ptr<IEnrollmentRepository> enrollmentRepo,
        std::shared_ptr<ILessonRepository> lessonRepo
    );

    // Создание записей посещаемости при изменении статусов
    bool createAttendanceForBooking(const UUID& bookingId, BookingStatus newStatus, const std::string& notes = "") override;
    bool createAttendanceForEnrollment(const UUID& enrollmentId, EnrollmentStatus newStatus, const std::string& notes = "") override;
    
    // Методы для ручного управления посещаемостью
    bool markBookingVisited(const UUID& bookingId, const std::string& notes = "") override;
    bool markBookingCancelled(const UUID& bookingId, const std::string& notes = "") override;
    bool markBookingNoShow(const UUID& bookingId, const std::string& notes = "") override;
    
    bool markLessonVisited(const UUID& enrollmentId, const std::string& notes = "") override;
    bool markLessonCancelled(const UUID& enrollmentId, const std::string& notes = "") override;
    bool markLessonNoShow(const UUID& enrollmentId, const std::string& notes = "") override;

    std::vector<Attendance> getClientAttendance(const UUID& clientId, 
                                               const std::chrono::system_clock::time_point& startDate,
                                               const std::chrono::system_clock::time_point& endDate) const override;

    std::vector<Attendance> getAttendanceByTypeAndStatus(AttendanceType type, AttendanceStatus status) const override;

    bool updateAttendanceNotes(const UUID& attendanceId, const std::string& notes) override;

private:
    bool shouldCreateAttendance(BookingStatus oldStatus, BookingStatus newStatus);
    bool shouldCreateAttendance(EnrollmentStatus oldStatus, EnrollmentStatus newStatus);
    std::optional<Attendance> findExistingAttendance(const UUID& entityId, AttendanceType type);
};

#endif // ATTENDANCE_SERVICE_HPP