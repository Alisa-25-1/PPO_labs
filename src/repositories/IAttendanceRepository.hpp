#pragma once
#include "../types/uuid.hpp"
#include "../models/Attendance.hpp"
#include <memory>
#include <optional>
#include <vector>

class IAttendanceRepository {
public:
    virtual ~IAttendanceRepository() = default;
    
    virtual std::optional<Attendance> findById(const UUID& id) = 0;
    virtual std::vector<Attendance> findByClientId(const UUID& clientId) = 0;
    virtual std::vector<Attendance> findByEntityId(const UUID& entityId) = 0;
    virtual std::vector<Attendance> findByClientAndPeriod(const UUID& clientId, 
        const std::chrono::system_clock::time_point& start, 
        const std::chrono::system_clock::time_point& end) = 0;
    virtual std::vector<Attendance> findByTypeAndStatus(AttendanceType type, AttendanceStatus status) = 0;
    virtual std::vector<Attendance> findAll() = 0;
    virtual bool save(const Attendance& attendance) = 0;
    virtual bool update(const Attendance& attendance) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
    
    // Статистические методы
    virtual int countByClientAndStatus(const UUID& clientId, AttendanceStatus status) = 0;
    virtual int countByTypeAndStatus(AttendanceType type, AttendanceStatus status) = 0;
    virtual std::vector<std::pair<UUID, int>> getTopClientsByVisits(int limit) = 0;
};