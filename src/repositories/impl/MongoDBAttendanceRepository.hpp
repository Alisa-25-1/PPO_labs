#ifndef MONGODB_ATTENDANCE_REPOSITORY_HPP
#define MONGODB_ATTENDANCE_REPOSITORY_HPP

#include "../IAttendanceRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление
class MongoDBRepositoryFactory;

class MongoDBAttendanceRepository : public IAttendanceRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBAttendanceRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Attendance> findById(const UUID& id) override;
    std::vector<Attendance> findByClientId(const UUID& clientId) override;
    std::vector<Attendance> findByEntityId(const UUID& entityId) override;
    std::vector<Attendance> findByClientAndPeriod(const UUID& clientId, 
        const std::chrono::system_clock::time_point& start, 
        const std::chrono::system_clock::time_point& end) override;
    std::vector<Attendance> findByTypeAndStatus(AttendanceType type, AttendanceStatus status) override;
    bool save(const Attendance& attendance) override;
    bool update(const Attendance& attendance) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;
    
    int countByClientAndStatus(const UUID& clientId, AttendanceStatus status) override;
    int countByTypeAndStatus(AttendanceType type, AttendanceStatus status) override;
    std::vector<std::pair<UUID, int>> getTopClientsByVisits(int limit) override;

private:
    Attendance mapDocumentToAttendance(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapAttendanceToDocument(const Attendance& attendance) const;
    void validateAttendance(const Attendance& attendance) const;
    
    // Вспомогательные методы для преобразования enum
    std::string attendanceTypeToString(AttendanceType type) const;
    AttendanceType stringToAttendanceType(const std::string& type) const;
    std::string attendanceStatusToString(AttendanceStatus status) const;
    AttendanceStatus stringToAttendanceStatus(const std::string& status) const;
};

#endif // MONGODB_ATTENDANCE_REPOSITORY_HPP