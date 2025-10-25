#include "PostgreSQLAttendanceRepository.hpp"
#include <pqxx/pqxx>
#include "../../data/DateTimeUtils.hpp"
#include "../../data/QueryFactory.hpp"
#include <iostream>

PostgreSQLAttendanceRepository::PostgreSQLAttendanceRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Attendance> PostgreSQLAttendanceRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "entity_id", "type", "status", 
                    "scheduled_time", "actual_time", "notes"})
            .from("attendance")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto attendance = mapResultToAttendance(result[0]);
        dbConnection_->commitTransaction(work);
        return attendance;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find attendance by ID: ") + e.what());
    }
}

std::vector<Attendance> PostgreSQLAttendanceRepository::findByClientId(const UUID& clientId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "entity_id", "type", "status", 
                    "scheduled_time", "actual_time", "notes"})
            .from("attendance")
            .where("client_id = $1")
            .orderBy("scheduled_time", false)
            .build();
        
        auto result = work.exec_params(query, clientId.toString());
        
        std::vector<Attendance> attendances;
        for (const auto& row : result) {
            attendances.push_back(mapResultToAttendance(row));
        }
        
        dbConnection_->commitTransaction(work);
        return attendances;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find attendances by client ID: ") + e.what());
    }
}

std::vector<Attendance> PostgreSQLAttendanceRepository::findByEntityId(const UUID& entityId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "entity_id", "type", "status", 
                    "scheduled_time", "actual_time", "notes"})
            .from("attendance")
            .where("entity_id = $1")
            .orderBy("scheduled_time", false)
            .build();
        
        auto result = work.exec_params(query, entityId.toString());
        
        std::vector<Attendance> attendances;
        for (const auto& row : result) {
            attendances.push_back(mapResultToAttendance(row));
        }
        
        dbConnection_->commitTransaction(work);
        return attendances;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find attendances by entity ID: ") + e.what());
    }
}

std::vector<Attendance> PostgreSQLAttendanceRepository::findByClientAndPeriod(
    const UUID& clientId, 
    const std::chrono::system_clock::time_point& start, 
    const std::chrono::system_clock::time_point& end) {
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        // Теперь этот вызов корректен, так как метод статический
        std::string query = QueryFactory::createFindAttendanceByClientAndPeriodQuery();
        auto startStr = DateTimeUtils::formatTimeForPostgres(start);
        auto endStr = DateTimeUtils::formatTimeForPostgres(end);
        
        auto result = work.exec_params(
            query, 
            clientId.toString(),
            startStr,
            endStr
        );
        
        std::vector<Attendance> attendances;
        for (const auto& row : result) {
            attendances.push_back(mapResultToAttendance(row));
        }
        
        dbConnection_->commitTransaction(work);
        return attendances;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find attendances by client and period: ") + e.what());
    }
}

std::vector<Attendance> PostgreSQLAttendanceRepository::findByTypeAndStatus(
    AttendanceType type, AttendanceStatus status) {
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "entity_id", "type", "status", 
                    "scheduled_time", "actual_time", "notes"})
            .from("attendance")
            .where("type = $1 AND status = $2")
            .orderBy("scheduled_time", false)
            .build();
        
        auto result = work.exec_params(
            query, 
            attendanceTypeToString(type),
            attendanceStatusToString(status)
        );
        
        std::vector<Attendance> attendances;
        for (const auto& row : result) {
            attendances.push_back(mapResultToAttendance(row));
        }
        
        dbConnection_->commitTransaction(work);
        return attendances;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find attendances by type and status: ") + e.what());
    }
}

bool PostgreSQLAttendanceRepository::save(const Attendance& attendance) {
    validateAttendance(attendance);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"client_id", "$2"},
            {"entity_id", "$3"},
            {"type", "$4"},
            {"status", "$5"},
            {"scheduled_time", "$6"},
            {"actual_time", "$7"},
            {"notes", "$8"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("attendance")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            attendance.getId().toString(),
            attendance.getClientId().toString(),
            attendance.getEntityId().toString(),
            attendanceTypeToString(attendance.getType()),
            attendanceStatusToString(attendance.getStatus()),
            DateTimeUtils::formatTimeForPostgres(attendance.getScheduledTime()),
            DateTimeUtils::formatTimeForPostgres(attendance.getActualTime()),
            attendance.getNotes()
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save attendance: ") + e.what());
    }
}

bool PostgreSQLAttendanceRepository::update(const Attendance& attendance) {
    validateAttendance(attendance);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"client_id", "$2"},
            {"entity_id", "$3"},
            {"type", "$4"},
            {"status", "$5"},
            {"scheduled_time", "$6"},
            {"actual_time", "$7"},
            {"notes", "$8"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("attendance")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            attendance.getId().toString(),
            attendance.getClientId().toString(),
            attendance.getEntityId().toString(),
            attendanceTypeToString(attendance.getType()),
            attendanceStatusToString(attendance.getStatus()),
            DateTimeUtils::formatTimeForPostgres(attendance.getScheduledTime()),
            DateTimeUtils::formatTimeForPostgres(attendance.getActualTime()),
            attendance.getNotes()
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update attendance: ") + e.what());
    }
}

bool PostgreSQLAttendanceRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("attendance")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove attendance: ") + e.what());
    }
}

bool PostgreSQLAttendanceRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("attendance")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check attendance existence: ") + e.what());
    }
}

int PostgreSQLAttendanceRepository::countByClientAndStatus(const UUID& clientId, AttendanceStatus status) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"COUNT(*)"})
            .from("attendance")
            .where("client_id = $1 AND status = $2")
            .build();
            
        auto result = work.exec_params(
            query, 
            clientId.toString(),
            attendanceStatusToString(status)
        );
        
        int count = 0;
        if (!result.empty()) {
            count = result[0]["count"].as<int>();
        }
        
        dbConnection_->commitTransaction(work);
        return count;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to count attendances by client and status: ") + e.what());
    }
}

int PostgreSQLAttendanceRepository::countByTypeAndStatus(AttendanceType type, AttendanceStatus status) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"COUNT(*)"})
            .from("attendance")
            .where("type = $1 AND status = $2")
            .build();
            
        auto result = work.exec_params(
            query, 
            attendanceTypeToString(type),
            attendanceStatusToString(status)
        );
        
        int count = 0;
        if (!result.empty()) {
            count = result[0]["count"].as<int>();
        }
        
        dbConnection_->commitTransaction(work);
        return count;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to count attendances by type and status: ") + e.what());
    }
}

std::vector<std::pair<UUID, int>> PostgreSQLAttendanceRepository::getTopClientsByVisits(int limit) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = R"(
            SELECT client_id, COUNT(*) as visit_count
            FROM attendance 
            WHERE status = 'VISITED'
            GROUP BY client_id 
            ORDER BY visit_count DESC 
            LIMIT $1
        )";
        
        auto result = work.exec_params(query, limit);
        
        std::vector<std::pair<UUID, int>> topClients;
        for (const auto& row : result) {
            UUID clientId = UUID::fromString(row["client_id"].c_str());
            int visitCount = row["visit_count"].as<int>();
            topClients.emplace_back(clientId, visitCount);
        }
        
        dbConnection_->commitTransaction(work);
        return topClients;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to get top clients by visits: ") + e.what());
    }
}

Attendance PostgreSQLAttendanceRepository::mapResultToAttendance(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    UUID clientId = UUID::fromString(row["client_id"].c_str());
    UUID entityId = UUID::fromString(row["entity_id"].c_str());
    
    AttendanceType type = stringToAttendanceType(row["type"].c_str());
    AttendanceStatus status = stringToAttendanceStatus(row["status"].c_str());
    
    auto scheduledTime = DateTimeUtils::parseTimeFromPostgres(row["scheduled_time"].c_str());
    auto actualTime = DateTimeUtils::parseTimeFromPostgres(row["actual_time"].c_str());
    
    std::string notes = row["notes"].c_str();
    
    Attendance attendance(id, clientId, entityId, type, scheduledTime);
    
    // Восстанавливаем статус
    switch (status) {
        case AttendanceStatus::VISITED:
            attendance.markVisited(notes);
            break;
        case AttendanceStatus::CANCELLED:
            attendance.markCancelled(notes);
            break;
        case AttendanceStatus::NO_SHOW:
            attendance.markNoShow(notes);
            break;
        default:
            // SCHEDULED - оставляем как есть
            break;
    }
    
    return attendance;
}

void PostgreSQLAttendanceRepository::validateAttendance(const Attendance& attendance) const {
    if (!attendance.isValid()) {
        throw DataAccessException("Invalid attendance data");
    }
}

std::string PostgreSQLAttendanceRepository::attendanceTypeToString(AttendanceType type) const {
    switch (type) {
        case AttendanceType::LESSON: return "LESSON";
        case AttendanceType::BOOKING: return "BOOKING";
        default: return "LESSON";
    }
}

AttendanceType PostgreSQLAttendanceRepository::stringToAttendanceType(const std::string& type) const {
    if (type == "BOOKING") return AttendanceType::BOOKING;
    return AttendanceType::LESSON;
}

std::string PostgreSQLAttendanceRepository::attendanceStatusToString(AttendanceStatus status) const {
    switch (status) {
        case AttendanceStatus::SCHEDULED: return "SCHEDULED";
        case AttendanceStatus::VISITED: return "VISITED";
        case AttendanceStatus::CANCELLED: return "CANCELLED";
        case AttendanceStatus::NO_SHOW: return "NO_SHOW";
        default: return "SCHEDULED";
    }
}

AttendanceStatus PostgreSQLAttendanceRepository::stringToAttendanceStatus(const std::string& status) const {
    if (status == "VISITED") return AttendanceStatus::VISITED;
    if (status == "CANCELLED") return AttendanceStatus::CANCELLED;
    if (status == "NO_SHOW") return AttendanceStatus::NO_SHOW;
    return AttendanceStatus::SCHEDULED;
}