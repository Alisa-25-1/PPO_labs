#include "LessonController.hpp"

LessonController::LessonController(
    std::shared_ptr<LessonService> lessonService,
    std::shared_ptr<EnrollmentService> enrollmentService,
    std::shared_ptr<BranchService> branchService
) : lessonService_(std::move(lessonService)),
    enrollmentService_(std::move(enrollmentService)),
    branchService_(std::move(branchService)) {}

std::vector<LessonResponseDTO> LessonController::getUpcomingLessons(int days) {
    try {
        return lessonService_->getUpcomingLessons(days);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get upcoming lessons: " + std::string(e.what()));
    }
}

std::vector<LessonResponseDTO> LessonController::getLessonsByBranch(const UUID& branchId) {
    try {
        // Получаем все залы филиала и затем занятия для этих залов
        auto halls = branchService_->getHallsByBranch(branchId);
        std::vector<LessonResponseDTO> allLessons;
        
        for (const auto& hall : halls) {
            auto lessons = lessonService_->getLessonsByHall(hall.getId());
            allLessons.insert(allLessons.end(), lessons.begin(), lessons.end());
        }
        
        return allLessons;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get lessons by branch: " + std::string(e.what()));
    }
}

LessonResponseDTO LessonController::getLesson(const UUID& lessonId) {
    try {
        return lessonService_->getLesson(lessonId);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get lesson: " + std::string(e.what()));
    }
}

EnrollmentResponseDTO LessonController::enrollInLesson(const UUID& clientId, const UUID& lessonId) {
    try {
        EnrollmentRequestDTO request{clientId, lessonId};
        return enrollmentService_->enrollClient(request);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to enroll in lesson: " + std::string(e.what()));
    }
}

EnrollmentResponseDTO LessonController::cancelEnrollment(const UUID& enrollmentId, const UUID& clientId) {
    try {
        return enrollmentService_->cancelEnrollment(enrollmentId, clientId);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to cancel enrollment: " + std::string(e.what()));
    }
}

std::vector<EnrollmentResponseDTO> LessonController::getClientEnrollments(const UUID& clientId) {
    try {
        return enrollmentService_->getClientEnrollments(clientId);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get client enrollments: " + std::string(e.what()));
    }
}

bool LessonController::canClientEnroll(const UUID& clientId, const UUID& lessonId) const {
    try {
        return enrollmentService_->isClientEnrolled(clientId, lessonId) == false &&
               lessonService_->canClientEnroll(clientId, lessonId);
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<Branch> LessonController::getBranches() {
    try {
        return branchService_->getAllBranches();
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get branches: " + std::string(e.what()));
    }
}

std::string LessonController::getBranchName(const UUID& branchId) {
    try {
        return branchService_->getBranchName(branchId);
    } catch (const std::exception& e) {
        return "Ошибка получения названия";
    }
}