#ifndef MOCK_ATTENDANCE_REPOSITORY_HPP
#define MOCK_ATTENDANCE_REPOSITORY_HPP

#include <gmock/gmock.h>
#include "../../../repositories/IAttendanceRepository.hpp"

class MockAttendanceRepository : public IAttendanceRepository {
public:
    MOCK_METHOD(std::optional<Attendance>, findById, (const UUID&), (override));
    MOCK_METHOD(std::vector<Attendance>, findByClientId, (const UUID&), (override));
    MOCK_METHOD(std::vector<Attendance>, findByEntityId, (const UUID&), (override));
    MOCK_METHOD(std::vector<Attendance>, findByClientAndPeriod, (const UUID&, const std::chrono::system_clock::time_point&, const std::chrono::system_clock::time_point&), (override));
    MOCK_METHOD(std::vector<Attendance>, findByTypeAndStatus, (AttendanceType, AttendanceStatus), (override));
    MOCK_METHOD(std::vector<Attendance>, findAll, (), (override));
    MOCK_METHOD(bool, save, (const Attendance&), (override));
    MOCK_METHOD(bool, update, (const Attendance&), (override));
    MOCK_METHOD(bool, remove, (const UUID&), (override));
    MOCK_METHOD(bool, exists, (const UUID&), (override));
    MOCK_METHOD(int, countByClientAndStatus, (const UUID&, AttendanceStatus), (override));
    MOCK_METHOD(int, countByTypeAndStatus, (AttendanceType, AttendanceStatus), (override));
    MOCK_METHOD((std::vector<std::pair<UUID, int>>), getTopClientsByVisits, (int), (override));
};

#endif // MOCK_ATTENDANCE_REPOSITORY_HPP