#ifndef POSTGRESQL_ATTENDANCE_REPOSITORY_HPP
#define POSTGRESQL_ATTENDANCE_REPOSITORY_HPP

#include "../IAttendanceRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include <memory>

class PostgreSQLAttendanceRepository : public IAttendanceRepository {
public:
    explicit PostgreSQLAttendanceRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Attendance> findById(const UUID& id) override;
    std::vector<Attendance> findByClientId(const UUID& clientId) override;
    std::vector<Attendance> findByEntityId(const UUID& entityId) override;
    std::vector<Attendance> findByClientAndPeriod(const UUID& clientId, 
        const std::chrono::system_clock::time_point& start, 
        const std::chrono::system_clock::time_point& end) override;
    std::vector<Attendance> findByTypeAndStatus(AttendanceType type, AttendanceStatus status) override;
    std::vector<Attendance> findAll() override;
    bool save(const Attendance& attendance) override;
    bool update(const Attendance& attendance) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;
    
    int countByClientAndStatus(const UUID& clientId, AttendanceStatus status) override;
    int countByTypeAndStatus(AttendanceType type, AttendanceStatus status) override;
    std::vector<std::pair<UUID, int>> getTopClientsByVisits(int limit) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Attendance mapResultToAttendance(const pqxx::row& row) const;
    void validateAttendance(const Attendance& attendance) const;
    std::string attendanceTypeToString(AttendanceType type) const;
    AttendanceType stringToAttendanceType(const std::string& type) const;
    std::string attendanceStatusToString(AttendanceStatus status) const;
    AttendanceStatus stringToAttendanceStatus(const std::string& status) const;
};

#endif // POSTGRESQL_ATTENDANCE_REPOSITORY_HPP