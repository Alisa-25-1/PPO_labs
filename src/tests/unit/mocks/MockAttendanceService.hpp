#ifndef MOCK_ATTENDANCE_SERVICE_HPP
#define MOCK_ATTENDANCE_SERVICE_HPP

#include <gmock/gmock.h>
#include "../../../services/AttendanceService.hpp"

class MockAttendanceService : public AttendanceService {
public:
    // Конструктор с правильными параметрами
    MockAttendanceService(
        std::shared_ptr<IAttendanceRepository> attendanceRepo,
        std::shared_ptr<IBookingRepository> bookingRepo,
        std::shared_ptr<IEnrollmentRepository> enrollmentRepo,
        std::shared_ptr<ILessonRepository> lessonRepo
    ) : AttendanceService(attendanceRepo, bookingRepo, enrollmentRepo, lessonRepo) {}

    // Используем MOCK_METHOD с правильным количеством аргументов
    MOCK_METHOD(bool, createAttendanceForBooking, (const UUID&, BookingStatus, const std::string&), (override));
    MOCK_METHOD(bool, createAttendanceForEnrollment, (const UUID&, EnrollmentStatus, const std::string&), (override));
    MOCK_METHOD(bool, markBookingVisited, (const UUID&, const std::string&), (override));
    MOCK_METHOD(bool, markBookingCancelled, (const UUID&, const std::string&), (override));
    MOCK_METHOD(bool, markBookingNoShow, (const UUID&, const std::string&), (override));
    MOCK_METHOD(bool, markLessonVisited, (const UUID&, const std::string&), (override));
    MOCK_METHOD(bool, markLessonCancelled, (const UUID&, const std::string&), (override));
    MOCK_METHOD(bool, markLessonNoShow, (const UUID&, const std::string&), (override));
    
    MOCK_METHOD(std::vector<Attendance>, getClientAttendance, (const UUID&, const std::chrono::system_clock::time_point&, const std::chrono::system_clock::time_point&), (const, override));
    MOCK_METHOD(std::vector<Attendance>, getAttendanceByTypeAndStatus, (AttendanceType, AttendanceStatus), (const, override));
    
    MOCK_METHOD(bool, updateAttendanceNotes, (const UUID&, const std::string&), (override));
};

#endif // MOCK_ATTENDANCE_SERVICE_HPP