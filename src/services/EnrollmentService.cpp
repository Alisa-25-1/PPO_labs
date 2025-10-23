#include "EnrollmentService.hpp"
#include <algorithm>

EnrollmentService::EnrollmentService(
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo,
    std::shared_ptr<IClientRepository> clientRepo,
    std::shared_ptr<ILessonRepository> lessonRepo
) : enrollmentRepository_(std::move(enrollmentRepo)),
    clientRepository_(std::move(clientRepo)),
    lessonRepository_(std::move(lessonRepo)) {}

void EnrollmentService::validateEnrollmentRequest(const EnrollmentRequestDTO& request) const {
    if (!request.validate()) {
        throw ValidationException("Invalid enrollment request data");
    }
    
    validateClient(request.clientId);
    validateLesson(request.lessonId);
    
    // Проверяем, что клиент еще не записан на это занятие
    if (isClientEnrolled(request.clientId, request.lessonId)) {
        throw EnrollmentException("Client is already enrolled in this lesson");
    }
}

void EnrollmentService::validateClient(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        throw ValidationException("Client not found");
    }
    
    if (!client->isActive()) {
        throw EnrollmentException("Client account is not active");
    }
}

void EnrollmentService::validateLesson(const UUID& lessonId) const {
    auto lesson = lessonRepository_->findById(lessonId);
    if (!lesson) {
        throw ValidationException("Lesson not found");
    }
    
    if (!lesson->canBeBooked()) {
        throw EnrollmentException("Lesson cannot be booked - it may be full, cancelled, or already started");
    }
}

EnrollmentResponseDTO EnrollmentService::enrollClient(const EnrollmentRequestDTO& request) {
    validateEnrollmentRequest(request);
    
    // Проверяем бизнес-правила через LessonService
    auto lesson = lessonRepository_->findById(request.lessonId);
    if (!lesson) {
        throw EnrollmentException("Lesson not found");
    }
    
    if (lesson->isFull()) {
        throw EnrollmentFullException("Lesson is full");
    }
    
    // Создаем запись
    UUID newId = UUID::generate();
    Enrollment enrollment(newId, request.clientId, request.lessonId);
    
    if (!enrollmentRepository_->save(enrollment)) {
        throw EnrollmentException("Failed to save enrollment");
    }
    
    // Обновляем количество участников в занятии
    lesson->addParticipant();
    if (!lessonRepository_->update(*lesson)) {
        // Откатываем запись если не удалось обновить занятие
        enrollmentRepository_->remove(newId);
        throw EnrollmentException("Failed to update lesson participants count");
    }
    
    return EnrollmentResponseDTO(enrollment);
}

EnrollmentResponseDTO EnrollmentService::cancelEnrollment(const UUID& enrollmentId, const UUID& clientId) {
    auto enrollment = enrollmentRepository_->findById(enrollmentId);
    if (!enrollment) {
        throw EnrollmentNotFoundException("Enrollment not found");
    }
    
    if (enrollment->getClientId() != clientId) {
        throw EnrollmentException("Client can only cancel their own enrollments");
    }
    
    if (!enrollment->canBeCancelled()) {
        throw EnrollmentException("Enrollment cannot be cancelled");
    }
    
    enrollment->cancel();
    
    if (!enrollmentRepository_->update(*enrollment)) {
        throw EnrollmentException("Failed to cancel enrollment");
    }
    
    // Уменьшаем количество участников в занятии
    auto lesson = lessonRepository_->findById(enrollment->getLessonId());
    if (lesson) {
        lesson->removeParticipant();
        lessonRepository_->update(*lesson);
    }
    
    return EnrollmentResponseDTO(*enrollment);
}

EnrollmentResponseDTO EnrollmentService::markAttendance(const UUID& enrollmentId, bool attended) {
    auto enrollment = enrollmentRepository_->findById(enrollmentId);
    if (!enrollment) {
        throw EnrollmentNotFoundException("Enrollment not found");
    }
    
    if (enrollment->getStatus() != EnrollmentStatus::REGISTERED) {
        throw EnrollmentException("Only registered enrollments can be marked for attendance");
    }
    
    if (attended) {
        enrollment->markAttended();
    } else {
        enrollment->markMissed();
    }
    
    if (!enrollmentRepository_->update(*enrollment)) {
        throw EnrollmentException("Failed to update enrollment attendance");
    }
    
    return EnrollmentResponseDTO(*enrollment);
}

std::vector<EnrollmentResponseDTO> EnrollmentService::getClientEnrollments(const UUID& clientId) {
    validateClient(clientId);
    
    auto enrollments = enrollmentRepository_->findByClientId(clientId);
    std::vector<EnrollmentResponseDTO> result;
    
    for (const auto& enrollment : enrollments) {
        result.push_back(EnrollmentResponseDTO(enrollment));
    }
    
    return result;
}

std::vector<EnrollmentResponseDTO> EnrollmentService::getLessonEnrollments(const UUID& lessonId) {
    validateLesson(lessonId);
    
    auto enrollments = enrollmentRepository_->findByLessonId(lessonId);
    std::vector<EnrollmentResponseDTO> result;
    
    for (const auto& enrollment : enrollments) {
        result.push_back(EnrollmentResponseDTO(enrollment));
    }
    
    return result;
}

std::optional<EnrollmentResponseDTO> EnrollmentService::getEnrollment(const UUID& clientId, const UUID& lessonId) {
    auto enrollment = enrollmentRepository_->findByClientAndLesson(clientId, lessonId);
    if (!enrollment) {
        return std::nullopt;
    }
    
    return EnrollmentResponseDTO(*enrollment);
}

bool EnrollmentService::isClientEnrolled(const UUID& clientId, const UUID& lessonId) const {
    auto enrollment = enrollmentRepository_->findByClientAndLesson(clientId, lessonId);
    return enrollment.has_value() && enrollment->getStatus() == EnrollmentStatus::REGISTERED;
}

int EnrollmentService::getLessonEnrollmentCount(const UUID& lessonId) const {
    return enrollmentRepository_->countByLessonId(lessonId);
}