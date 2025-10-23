#pragma once
#include "../repositories/IReviewRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../repositories/ILessonRepository.hpp"
#include "../repositories/IEnrollmentRepository.hpp"
#include "../dtos/ReviewDTO.hpp"
#include "../types/uuid.hpp"
#include "exceptions/ValidationException.hpp"
#include "exceptions/ReviewException.hpp"
#include <memory>
#include <vector>

class ReviewService {
private:
    std::shared_ptr<IReviewRepository> reviewRepository_;
    std::shared_ptr<IClientRepository> clientRepository_;
    std::shared_ptr<ILessonRepository> lessonRepository_;
    std::shared_ptr<IEnrollmentRepository> enrollmentRepository_;

    void validateReviewRequest(const ReviewRequestDTO& request) const;
    void validateClient(const UUID& clientId) const;
    void validateLesson(const UUID& lessonId) const;
    bool canClientReviewLesson(const UUID& clientId, const UUID& lessonId) const;

public:
    ReviewService(
        std::shared_ptr<IReviewRepository> reviewRepo,
        std::shared_ptr<IClientRepository> clientRepo,
        std::shared_ptr<ILessonRepository> lessonRepo,
        std::shared_ptr<IEnrollmentRepository> enrollmentRepo
    );

    ReviewResponseDTO createReview(const ReviewRequestDTO& request);
    ReviewResponseDTO approveReview(const UUID& reviewId);
    ReviewResponseDTO rejectReview(const UUID& reviewId);
    ReviewResponseDTO getReview(const UUID& reviewId);
    std::vector<ReviewResponseDTO> getClientReviews(const UUID& clientId);
    std::vector<ReviewResponseDTO> getLessonReviews(const UUID& lessonId);
    std::vector<ReviewResponseDTO> getPendingReviews();
    double getAverageRatingForTrainer(const UUID& trainerId) const;
    bool hasClientReviewedLesson(const UUID& clientId, const UUID& lessonId) const;
};