#ifndef POSTGRESQL_ENROLLMENT_REPOSITORY_HPP
#define POSTGRESQL_ENROLLMENT_REPOSITORY_HPP

#include "../IEnrollmentRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <memory>

class PostgreSQLEnrollmentRepository : public IEnrollmentRepository {
public:
    explicit PostgreSQLEnrollmentRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Enrollment> findById(const UUID& id) override;
    std::vector<Enrollment> findByClientId(const UUID& clientId) override;
    std::vector<Enrollment> findByLessonId(const UUID& lessonId) override;
    std::optional<Enrollment> findByClientAndLesson(const UUID& clientId, const UUID& lessonId) override;
    int countByLessonId(const UUID& lessonId) override;
    std::vector<Enrollment> findAll() override;

    bool save(const Enrollment& enrollment) override;
    bool update(const Enrollment& enrollment) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Enrollment mapResultToEnrollment(const pqxx::row& row) const;
    void validateEnrollment(const Enrollment& enrollment) const;
    std::string enrollmentStatusToString(EnrollmentStatus status) const;
    EnrollmentStatus stringToEnrollmentStatus(const std::string& status) const;
};

#endif // POSTGRESQL_ENROLLMENT_REPOSITORY_HPP