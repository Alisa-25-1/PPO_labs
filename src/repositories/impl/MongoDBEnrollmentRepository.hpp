#ifndef MONGODB_ENROLLMENT_REPOSITORY_HPP
#define MONGODB_ENROLLMENT_REPOSITORY_HPP

#include "../IEnrollmentRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

// Предварительное объявление
class MongoDBRepositoryFactory;

class MongoDBEnrollmentRepository : public IEnrollmentRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBEnrollmentRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
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
    Enrollment mapDocumentToEnrollment(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapEnrollmentToDocument(const Enrollment& enrollment) const;
    void validateEnrollment(const Enrollment& enrollment) const;
    
    // Вспомогательные методы для преобразования enum
    std::string enrollmentStatusToString(EnrollmentStatus status) const;
    EnrollmentStatus stringToEnrollmentStatus(const std::string& status) const;
};

#endif // MONGODB_ENROLLMENT_REPOSITORY_HPP