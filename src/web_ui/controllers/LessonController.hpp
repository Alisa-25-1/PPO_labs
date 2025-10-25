#pragma once

#include "../../services/LessonService.hpp"
#include "../../services/EnrollmentService.hpp"
#include "../../services/BranchService.hpp"
#include "../../dtos/LessonDTO.hpp"
#include "../../dtos/EnrollmentDTO.hpp"
#include "../../types/uuid.hpp"
#include <memory>
#include <vector>

class LessonController {
private:
    std::shared_ptr<LessonService> lessonService_;
    std::shared_ptr<EnrollmentService> enrollmentService_;
    std::shared_ptr<BranchService> branchService_;

public:
    LessonController(
        std::shared_ptr<LessonService> lessonService,
        std::shared_ptr<EnrollmentService> enrollmentService,
        std::shared_ptr<BranchService> branchService
    );

    // Методы для занятий
    std::vector<LessonResponseDTO> getUpcomingLessons(int days = 7);
    std::vector<LessonResponseDTO> getLessonsByBranch(const UUID& branchId);
    LessonResponseDTO getLesson(const UUID& lessonId);
    
    // Методы для записи на занятия
    EnrollmentResponseDTO enrollInLesson(const UUID& clientId, const UUID& lessonId);
    EnrollmentResponseDTO cancelEnrollment(const UUID& enrollmentId, const UUID& clientId);
    std::vector<EnrollmentResponseDTO> getClientEnrollments(const UUID& clientId);
    bool canClientEnroll(const UUID& clientId, const UUID& lessonId) const;
    
    // Вспомогательные методы
    std::vector<Branch> getBranches();
    std::string getBranchName(const UUID& branchId);
    std::vector<DanceHall> getHallsByBranch(const UUID& branchId);
};