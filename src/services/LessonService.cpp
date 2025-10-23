#include "LessonService.hpp"
#include <algorithm>

LessonService::LessonService(
    std::shared_ptr<ILessonRepository> lessonRepo,
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo,
    std::shared_ptr<ITrainerRepository> trainerRepo,
    std::shared_ptr<IDanceHallRepository> hallRepo
) : lessonRepository_(std::move(lessonRepo)),
    enrollmentRepository_(std::move(enrollmentRepo)),
    trainerRepository_(std::move(trainerRepo)),
    hallRepository_(std::move(hallRepo)) {}

void LessonService::validateLessonRequest(const LessonRequestDTO& request) const {
    if (!request.validate()) {
        throw ValidationException("Invalid lesson request data");
    }
    
    validateTrainer(request.trainerId);
    validateHall(request.hallId);
    
    // Проверяем, что время занятия в будущем (только для создания новых уроков)
    if (request.startTime <= std::chrono::system_clock::now()) {
        throw ValidationException("Lesson must be scheduled for future time");
    }
    
    // Проверяем, что время занятия в будущем
    if (request.startTime <= std::chrono::system_clock::now()) {
        throw ValidationException("Lesson must be scheduled for future time");
    }
}

void LessonService::validateTrainer(const UUID& trainerId) const {
    auto trainer = trainerRepository_->findById(trainerId);
    if (!trainer) {
        throw ValidationException("Trainer not found");
    }
    
    if (!trainer->isActive()) {
        throw BusinessRuleException("Trainer is not active");
    }
}

void LessonService::validateHall(const UUID& hallId) const {
    if (!hallRepository_->exists(hallId)) {
        throw ValidationException("Dance hall not found");
    }
}

void LessonService::checkTimeConflicts(const UUID& hallId, const TimeSlot& timeSlot, const UUID& excludeLessonId) const {
    auto conflictingLessons = lessonRepository_->findConflictingLessons(hallId, timeSlot);
    
    // Исключаем текущее занятие (для обновления)
    if (!excludeLessonId.isNull()) {
        conflictingLessons.erase(
            std::remove_if(conflictingLessons.begin(), conflictingLessons.end(),
                [excludeLessonId](const Lesson& lesson) {
                    return lesson.getId() == excludeLessonId;
                }),
            conflictingLessons.end()
        );
    }
    
    if (!conflictingLessons.empty()) {
        throw BusinessRuleException("Time slot conflicts with existing lesson");
    }
}

LessonResponseDTO LessonService::createLesson(const LessonRequestDTO& request) {
    validateLessonRequest(request);
    
    TimeSlot timeSlot(request.startTime, request.durationMinutes);
    checkTimeConflicts(request.hallId, timeSlot);
    
    UUID newId = UUID::generate();
    Lesson lesson(newId, request.type, request.name, request.startTime, 
                  request.durationMinutes, request.difficulty, request.maxParticipants, 
                  request.price, request.trainerId, request.hallId);
    lesson.setDescription(request.description);
    
    if (!lessonRepository_->save(lesson)) {
        throw std::runtime_error("Failed to save lesson");
    }
    
    return LessonResponseDTO(lesson);
}

LessonResponseDTO LessonService::updateLesson(const UUID& lessonId, const LessonRequestDTO& request) {
    auto existingLesson = lessonRepository_->findById(lessonId);
    if (!existingLesson) {
        throw std::runtime_error("Lesson not found");
    }
    
    validateLessonRequest(request);
    
    TimeSlot timeSlot(request.startTime, request.durationMinutes);
    checkTimeConflicts(request.hallId, timeSlot, lessonId);
    
    // Обновляем данные занятия
    Lesson updatedLesson(lessonId, request.type, request.name, request.startTime, 
                         request.durationMinutes, request.difficulty, request.maxParticipants, 
                         request.price, request.trainerId, request.hallId);
    updatedLesson.setDescription(request.description);
    updatedLesson.setStatus(existingLesson->getStatus()); // Сохраняем текущий статус
    
    if (!lessonRepository_->update(updatedLesson)) {
        throw std::runtime_error("Failed to update lesson");
    }
    
    return LessonResponseDTO(updatedLesson);
}

LessonResponseDTO LessonService::cancelLesson(const UUID& lessonId) {
    auto lesson = lessonRepository_->findById(lessonId);
    if (!lesson) {
        throw std::runtime_error("Lesson not found");
    }
    
    // Можно отменять только запланированные занятия
    if (lesson->getStatus() != LessonStatus::SCHEDULED) {
        throw BusinessRuleException("Only scheduled lessons can be cancelled");
    }
    
    lesson->setStatus(LessonStatus::CANCELLED);
    
    if (!lessonRepository_->update(*lesson)) {
        throw std::runtime_error("Failed to cancel lesson");
    }
    
    return LessonResponseDTO(*lesson);
}

LessonResponseDTO LessonService::getLesson(const UUID& lessonId) {
    auto lesson = lessonRepository_->findById(lessonId);
    if (!lesson) {
        throw std::runtime_error("Lesson not found");
    }
    
    return LessonResponseDTO(*lesson);
}

std::vector<LessonResponseDTO> LessonService::getLessonsByTrainer(const UUID& trainerId) {
    validateTrainer(trainerId);
    
    auto lessons = lessonRepository_->findByTrainerId(trainerId);
    std::vector<LessonResponseDTO> result;
    
    for (const auto& lesson : lessons) {
        result.push_back(LessonResponseDTO(lesson));
    }
    
    return result;
}

std::vector<LessonResponseDTO> LessonService::getLessonsByHall(const UUID& hallId) {
    validateHall(hallId);
    
    auto lessons = lessonRepository_->findByHallId(hallId);
    std::vector<LessonResponseDTO> result;
    
    for (const auto& lesson : lessons) {
        result.push_back(LessonResponseDTO(lesson));
    }
    
    return result;
}

std::vector<LessonResponseDTO> LessonService::getUpcomingLessons(int days) {
    auto lessons = lessonRepository_->findUpcomingLessons(days);
    std::vector<LessonResponseDTO> result;
    
    for (const auto& lesson : lessons) {
        result.push_back(LessonResponseDTO(lesson));
    }
    
    return result;
}

bool LessonService::canClientEnroll(const UUID& clientId, const UUID& lessonId) const {
    auto lesson = lessonRepository_->findById(lessonId);
    if (!lesson) {
        return false;
    }
    
    // Проверяем, что занятие еще не началось и есть свободные места
    if (!lesson->canBeBooked()) {
        return false;
    }
    
    // Проверяем, что клиент еще не записан
    auto existingEnrollment = enrollmentRepository_->findByClientAndLesson(clientId, lessonId);
    if (existingEnrollment) {
        return false;
    }
    
    return true;
}

int LessonService::getAvailableSpots(const UUID& lessonId) const {
    auto lesson = lessonRepository_->findById(lessonId);
    if (!lesson) {
        return 0;
    }
    
    return lesson->getMaxParticipants() - lesson->getCurrentParticipants();
}