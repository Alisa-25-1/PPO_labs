#include "PostgreSQLEnrollmentRepository.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include "../../data/DateTimeUtils.hpp"
#include "../../data/QueryFactory.hpp"

PostgreSQLEnrollmentRepository::PostgreSQLEnrollmentRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Enrollment> PostgreSQLEnrollmentRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "status", "enrollment_date"})
            .from("enrollments")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto enrollment = mapResultToEnrollment(result[0]);
        dbConnection_->commitTransaction(work);
        return enrollment;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find enrollment by ID: ") + e.what());
    }
}

std::vector<Enrollment> PostgreSQLEnrollmentRepository::findByClientId(const UUID& clientId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "status", "enrollment_date"})
            .from("enrollments")
            .where("client_id = $1")
            .build();
        
        auto result = work.exec_params(query, clientId.toString());
        
        std::vector<Enrollment> enrollments;
        for (const auto& row : result) {
            enrollments.push_back(mapResultToEnrollment(row));
        }
        
        dbConnection_->commitTransaction(work);
        return enrollments;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find enrollments by client ID: ") + e.what());
    }
}

std::vector<Enrollment> PostgreSQLEnrollmentRepository::findByLessonId(const UUID& lessonId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "status", "enrollment_date"})
            .from("enrollments")
            .where("lesson_id = $1")
            .build();
        
        auto result = work.exec_params(query, lessonId.toString());
        
        std::vector<Enrollment> enrollments;
        for (const auto& row : result) {
            enrollments.push_back(mapResultToEnrollment(row));
        }
        
        dbConnection_->commitTransaction(work);
        return enrollments;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find enrollments by lesson ID: ") + e.what());
    }
}

std::optional<Enrollment> PostgreSQLEnrollmentRepository::findByClientAndLesson(
    const UUID& clientId, const UUID& lessonId) {
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "status", "enrollment_date"})
            .from("enrollments")
            .where("client_id = $1 AND lesson_id = $2")
            .build();
        
        auto result = work.exec_params(query, clientId.toString(), lessonId.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto enrollment = mapResultToEnrollment(result[0]);
        dbConnection_->commitTransaction(work);
        return enrollment;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find enrollment by client and lesson: ") + e.what());
    }
}

int PostgreSQLEnrollmentRepository::countByLessonId(const UUID& lessonId) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = QueryFactory::createCountEnrollmentsByLessonQuery();
        auto result = work.exec_params(query, lessonId.toString());
        
        int count = 0;
        if (!result.empty()) {
            count = result[0]["count"].as<int>();
        }
        
        dbConnection_->commitTransaction(work);
        return count;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to count enrollments by lesson ID: ") + e.what());
    }
}

bool PostgreSQLEnrollmentRepository::save(const Enrollment& enrollment) {
    validateEnrollment(enrollment);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"client_id", "$2"},
            {"lesson_id", "$3"},
            {"status", "$4"},
            {"enrollment_date", "$5"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("enrollments")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            enrollment.getId().toString(),
            enrollment.getClientId().toString(),
            enrollment.getLessonId().toString(),
            enrollmentStatusToString(enrollment.getStatus()),
            DateTimeUtils::formatTimeForPostgres(enrollment.getEnrollmentDate())
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save enrollment: ") + e.what());
    }
}

bool PostgreSQLEnrollmentRepository::update(const Enrollment& enrollment) {
    validateEnrollment(enrollment);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"client_id", "$2"},
            {"lesson_id", "$3"},
            {"status", "$4"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("enrollments")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            enrollment.getId().toString(),
            enrollment.getClientId().toString(),
            enrollment.getLessonId().toString(),
            enrollmentStatusToString(enrollment.getStatus())
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update enrollment: ") + e.what());
    }
}

bool PostgreSQLEnrollmentRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("enrollments")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove enrollment: ") + e.what());
    }
}

bool PostgreSQLEnrollmentRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("enrollments")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check enrollment existence: ") + e.what());
    }
}

Enrollment PostgreSQLEnrollmentRepository::mapResultToEnrollment(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    UUID clientId = UUID::fromString(row["client_id"].c_str());
    UUID lessonId = UUID::fromString(row["lesson_id"].c_str());
    EnrollmentStatus status = stringToEnrollmentStatus(row["status"].c_str());
    
    Enrollment enrollment(id, clientId, lessonId);
    
    // Восстанавливаем статус
    switch (status) {
        case EnrollmentStatus::CANCELLED:
            enrollment.cancel();
            break;
        case EnrollmentStatus::ATTENDED:
            enrollment.markAttended();
            break;
        case EnrollmentStatus::MISSED:
            enrollment.markMissed();
            break;
        default:
            break; // REGISTERED по умолчанию
    }
    
    return enrollment;
}

void PostgreSQLEnrollmentRepository::validateEnrollment(const Enrollment& enrollment) const {
    if (!enrollment.isValid()) {
        throw DataAccessException("Invalid enrollment data");
    }
}

std::string PostgreSQLEnrollmentRepository::enrollmentStatusToString(EnrollmentStatus status) const {
    switch (status) {
        case EnrollmentStatus::REGISTERED: return "REGISTERED";
        case EnrollmentStatus::CANCELLED: return "CANCELLED";
        case EnrollmentStatus::ATTENDED: return "ATTENDED";
        case EnrollmentStatus::MISSED: return "MISSED";
        default: return "REGISTERED";
    }
}

EnrollmentStatus PostgreSQLEnrollmentRepository::stringToEnrollmentStatus(const std::string& status) const {
    if (status == "CANCELLED") return EnrollmentStatus::CANCELLED;
    if (status == "ATTENDED") return EnrollmentStatus::ATTENDED;
    if (status == "MISSED") return EnrollmentStatus::MISSED;
    return EnrollmentStatus::REGISTERED;
}

std::vector<Enrollment> PostgreSQLEnrollmentRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "client_id", "lesson_id", "status", "enrollment_date"})
            .from("enrollments")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Enrollment> enrollments;
        for (const auto& row : result) {
            enrollments.push_back(mapResultToEnrollment(row));
        }
        
        dbConnection_->commitTransaction(work);
        return enrollments;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all enrollments: ") + e.what());
    }
}

