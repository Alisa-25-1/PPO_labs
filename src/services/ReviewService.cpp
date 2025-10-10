#include "ReviewService.hpp"
#include <algorithm>

ReviewService::ReviewService(
    std::unique_ptr<IReviewRepository> reviewRepo,
    std::unique_ptr<IClientRepository> clientRepo,
    std::unique_ptr<ILessonRepository> lessonRepo,
    std::unique_ptr<IEnrollmentRepository> enrollmentRepo
) : reviewRepository_(std::move(reviewRepo)),
    clientRepository_(std::move(clientRepo)),
    lessonRepository_(std::move(lessonRepo)),
    enrollmentRepository_(std::move(enrollmentRepo)) {}

void ReviewService::validateReviewRequest(const ReviewRequestDTO& request) const {
    if (!request.validate()) {
        throw ValidationException("Invalid review request data");
    }
    
    validateClient(request.clientId);
    validateLesson(request.lessonId);
    
    // Проверяем, что клиент может оставить отзыв (посетил занятие)
    if (!canClientReviewLesson(request.clientId, request.lessonId)) {
        throw BusinessRuleException("Client cannot review this lesson");
    }
    
    // Проверяем, что клиент еще не оставлял отзыв на это занятие
    if (hasClientReviewedLesson(request.clientId, request.lessonId)) {
        throw BusinessRuleException("Client has already reviewed this lesson");
    }
}

void ReviewService::validateClient(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        throw ValidationException("Client not found");
    }
    
    if (!client->isActive()) {
        throw BusinessRuleException("Client account is not active");
    }
}

void ReviewService::validateLesson(const UUID& lessonId) const {
    auto lesson = lessonRepository_->findById(lessonId);
    if (!lesson) {
        throw ValidationException("Lesson not found");
    }
    
    // Проверяем, что занятие уже завершено
    if (!lesson->isCompleted()) {
        throw BusinessRuleException("Can only review completed lessons");
    }
}

bool ReviewService::canClientReviewLesson(const UUID& clientId, const UUID& lessonId) const {
    // Проверяем, что клиент посетил это занятие
    auto enrollment = enrollmentRepository_->findByClientAndLesson(clientId, lessonId);
    return enrollment && enrollment->getStatus() == EnrollmentStatus::ATTENDED;
}

ReviewResponseDTO ReviewService::createReview(const ReviewRequestDTO& request) {
    validateReviewRequest(request);
    
    // Создаем отзыв
    UUID newId = UUID::generate();
    Review review(newId, request.clientId, request.lessonId, request.rating, request.comment);
    
    if (!reviewRepository_->save(review)) {
        throw std::runtime_error("Failed to save review");
    }
    
    return ReviewResponseDTO(review);
}

ReviewResponseDTO ReviewService::approveReview(const UUID& reviewId) {
    auto review = reviewRepository_->findById(reviewId);
    if (!review) {
        throw ReviewNotFoundException("Review not found");
    }
    
    if (review->isApproved()) {
        throw BusinessRuleException("Review is already approved");
    }
    
    review->approve();
    
    if (!reviewRepository_->update(*review)) {
        throw std::runtime_error("Failed to approve review");
    }
    
    return ReviewResponseDTO(*review);
}

ReviewResponseDTO ReviewService::rejectReview(const UUID& reviewId) {
    auto review = reviewRepository_->findById(reviewId);
    if (!review) {
        throw ReviewNotFoundException("Review not found");
    }
    
    if (review->isApproved()) {
        throw BusinessRuleException("Cannot reject approved review");
    }
    
    review->reject();
    
    if (!reviewRepository_->update(*review)) {
        throw std::runtime_error("Failed to reject review");
    }
    
    return ReviewResponseDTO(*review);
}

ReviewResponseDTO ReviewService::getReview(const UUID& reviewId) {
    auto review = reviewRepository_->findById(reviewId);
    if (!review) {
        throw ReviewNotFoundException("Review not found");
    }
    
    return ReviewResponseDTO(*review);
}

std::vector<ReviewResponseDTO> ReviewService::getClientReviews(const UUID& clientId) {
    validateClient(clientId);
    
    auto reviews = reviewRepository_->findByClientId(clientId);
    std::vector<ReviewResponseDTO> result;
    
    for (const auto& review : reviews) {
        result.push_back(ReviewResponseDTO(review));
    }
    
    return result;
}

std::vector<ReviewResponseDTO> ReviewService::getLessonReviews(const UUID& lessonId) {
    validateLesson(lessonId);
    
    auto reviews = reviewRepository_->findByLessonId(lessonId);
    std::vector<ReviewResponseDTO> result;
    
    for (const auto& review : reviews) {
        result.push_back(ReviewResponseDTO(review));
    }
    
    return result;
}

std::vector<ReviewResponseDTO> ReviewService::getPendingReviews() {
    auto reviews = reviewRepository_->findPendingModeration();
    std::vector<ReviewResponseDTO> result;
    
    for (const auto& review : reviews) {
        result.push_back(ReviewResponseDTO(review));
    }
    
    return result;
}

double ReviewService::getAverageRatingForTrainer(const UUID& trainerId) const {
    return reviewRepository_->getAverageRatingForTrainer(trainerId);
}

bool ReviewService::hasClientReviewedLesson(const UUID& clientId, const UUID& lessonId) const {
    auto existingReview = reviewRepository_->findByClientAndLesson(clientId, lessonId);
    return existingReview.has_value();
}