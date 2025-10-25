#ifndef MOCKATTENDANCEREPOSITORY_HPP
#define MOCKATTENDANCEREPOSITORY_HPP

#include <gmock/gmock.h>
#include "../../../repositories/IAttendanceRepository.hpp"

class MockAttendanceRepository : public IAttendanceRepository {
public:
    MOCK_METHOD(std::optional<Attendance>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Attendance>, findByClientId, (const UUID& clientId), (override));
    MOCK_METHOD(std::vector<Attendance>, findByEntityId, (const UUID& entityId), (override));
    MOCK_METHOD(std::vector<Attendance>, findByClientAndPeriod, (
        const UUID& clientId, 
        const std::chrono::system_clock::time_point& start, 
        const std::chrono::system_clock::time_point& end), (override));
    MOCK_METHOD(std::vector<Attendance>, findByTypeAndStatus, (AttendanceType type, AttendanceStatus status), (override));
    MOCK_METHOD(bool, save, (const Attendance& attendance), (override));
    MOCK_METHOD(bool, update, (const Attendance& attendance), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
    
    MOCK_METHOD(int, countByClientAndStatus, (const UUID& clientId, AttendanceStatus status), (override));
    MOCK_METHOD(int, countByTypeAndStatus, (AttendanceType type, AttendanceStatus status), (override));
    MOCK_METHOD(std::vector<std::pair<UUID, int>>, getTopClientsByVisits, (int limit), (override)); 
};

#endif // MOCKATTENDANCEREPOSITORY_HPP