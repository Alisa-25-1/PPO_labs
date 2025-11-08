#pragma once
#include "../types/uuid.hpp"
#include "../models/Lesson.hpp"
#include "../models/TimeSlot.hpp"
#include <memory>
#include <optional>
#include <vector>

class ILessonRepository {
public:
    virtual ~ILessonRepository() = default;
    
    virtual std::optional<Lesson> findById(const UUID& id) = 0;
    virtual std::vector<Lesson> findByTrainerId(const UUID& trainerId) = 0;
    virtual std::vector<Lesson> findByHallId(const UUID& hallId) = 0;
    virtual std::vector<Lesson> findConflictingLessons(const UUID& hallId, const TimeSlot& timeSlot) = 0;
    virtual std::vector<Lesson> findUpcomingLessons(int days = 7) = 0;
    virtual std::vector<Lesson> findAll() = 0; 
    virtual bool save(const Lesson& lesson) = 0;
    virtual bool update(const Lesson& lesson) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};