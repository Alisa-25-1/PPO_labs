#pragma once
#include "../repositories/ILessonRepository.hpp"
#include "../repositories/IEnrollmentRepository.hpp"
#include "../repositories/ITrainerRepository.hpp"
#include "../repositories/IDanceHallRepository.hpp"
#include "../dtos/LessonDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/ValidationException.hpp"
#include <memory>
#include <vector>

class LessonService {
private:
    std::unique_ptr<ILessonRepository> lessonRepository_;
    std::unique_ptr<IEnrollmentRepository> enrollmentRepository_;
    std::unique_ptr<ITrainerRepository> trainerRepository_;
    std::unique_ptr<IDanceHallRepository> hallRepository_;

    void validateLessonRequest(const LessonRequestDTO& request) const;
    void validateTrainer(const UUID& trainerId) const;
    void validateHall(const UUID& hallId) const;
    void checkTimeConflicts(const UUID& hallId, const TimeSlot& timeSlot, const UUID& excludeLessonId = UUID()) const;

public:
    LessonService(
        std::unique_ptr<ILessonRepository> lessonRepo,
        std::unique_ptr<IEnrollmentRepository> enrollmentRepo,
        std::unique_ptr<ITrainerRepository> trainerRepo,
        std::unique_ptr<IDanceHallRepository> hallRepo
    );

    LessonResponseDTO createLesson(const LessonRequestDTO& request);
    LessonResponseDTO updateLesson(const UUID& lessonId, const LessonRequestDTO& request);
    LessonResponseDTO cancelLesson(const UUID& lessonId);
    LessonResponseDTO getLesson(const UUID& lessonId);
    std::vector<LessonResponseDTO> getLessonsByTrainer(const UUID& trainerId);
    std::vector<LessonResponseDTO> getLessonsByHall(const UUID& hallId);
    std::vector<LessonResponseDTO> getUpcomingLessons(int days = 7);
    bool canClientEnroll(const UUID& clientId, const UUID& lessonId) const;
    int getAvailableSpots(const UUID& lessonId) const;
};