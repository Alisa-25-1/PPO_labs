#pragma once
#include "../repositories/IEnrollmentRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../repositories/ILessonRepository.hpp"
#include "../dtos/EnrollmentDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/ValidationException.hpp"
#include "exceptions/EnrollmentException.hpp"
#include <memory>
#include <vector>

class EnrollmentService {
private:
    std::unique_ptr<IEnrollmentRepository> enrollmentRepository_;
    std::unique_ptr<IClientRepository> clientRepository_;
    std::unique_ptr<ILessonRepository> lessonRepository_;

    void validateEnrollmentRequest(const EnrollmentRequestDTO& request) const;
    void validateClient(const UUID& clientId) const;
    void validateLesson(const UUID& lessonId) const;

public:
    EnrollmentService(
        std::unique_ptr<IEnrollmentRepository> enrollmentRepo,
        std::unique_ptr<IClientRepository> clientRepo,
        std::unique_ptr<ILessonRepository> lessonRepo
    );

    EnrollmentResponseDTO enrollClient(const EnrollmentRequestDTO& request);
    EnrollmentResponseDTO cancelEnrollment(const UUID& enrollmentId, const UUID& clientId);
    EnrollmentResponseDTO markAttendance(const UUID& enrollmentId, bool attended);
    std::vector<EnrollmentResponseDTO> getClientEnrollments(const UUID& clientId);
    std::vector<EnrollmentResponseDTO> getLessonEnrollments(const UUID& lessonId);
    std::optional<EnrollmentResponseDTO> getEnrollment(const UUID& clientId, const UUID& lessonId);
    bool isClientEnrolled(const UUID& clientId, const UUID& lessonId) const;
    int getLessonEnrollmentCount(const UUID& lessonId) const;
};