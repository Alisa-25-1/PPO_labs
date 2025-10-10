#pragma once
#include "../types/uuid.hpp"
#include "../models/Enrollment.hpp"
#include <memory>
#include <optional>
#include <vector>

class IEnrollmentRepository {
public:
    virtual ~IEnrollmentRepository() = default;
    
    virtual std::optional<Enrollment> findById(const UUID& id) = 0;
    virtual std::vector<Enrollment> findByClientId(const UUID& clientId) = 0;
    virtual std::vector<Enrollment> findByLessonId(const UUID& lessonId) = 0;
    virtual std::optional<Enrollment> findByClientAndLesson(const UUID& clientId, const UUID& lessonId) = 0;
    virtual int countByLessonId(const UUID& lessonId) = 0;
    virtual bool save(const Enrollment& enrollment) = 0;
    virtual bool update(const Enrollment& enrollment) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};