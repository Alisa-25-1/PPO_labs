#include "Review.hpp"
#include <algorithm>
#include <ctime>

Review::Review(const UUID& id, const UUID& clientId, const UUID& lessonId, 
               int rating, const std::string& comment)
    : id_(id), clientId_(clientId), lessonId_(lessonId), 
      rating_(rating), comment_(comment), 
      publicationDate_(std::chrono::system_clock::now()),
      status_(ReviewStatus::PENDING_MODERATION) {
}

// Геттеры
UUID Review::getId() const { 
    return id_; 
}

UUID Review::getClientId() const { 
    return clientId_; 
}

UUID Review::getLessonId() const { 
    return lessonId_; 
}

int Review::getRating() const { 
    return rating_; 
}

std::string Review::getComment() const { 
    return comment_; 
}

std::chrono::system_clock::time_point Review::getPublicationDate() const { 
    return publicationDate_; 
}

ReviewStatus Review::getStatus() const { 
    return status_; 
}

// Методы управления статусом
void Review::approve() {
    status_ = ReviewStatus::APPROVED;
}

void Review::reject() {
    status_ = ReviewStatus::REJECTED;
}

// Проверки статуса
bool Review::isApproved() const {
    return status_ == ReviewStatus::APPROVED;
}

bool Review::isPending() const {
    return status_ == ReviewStatus::PENDING_MODERATION;
}

bool Review::isRejected() const {
    return status_ == ReviewStatus::REJECTED;
}

// Валидация
bool Review::isValid() const {
    return isValidRating(rating_) && isValidComment(comment_);
}

// Статические методы валидации
bool Review::isValidRating(int rating) {
    return rating >= 1 && rating <= 5;
}

bool Review::isValidComment(const std::string& comment) {
    return !comment.empty() && comment.length() <= 1000;
}

// Операторы сравнения
bool Review::operator==(const Review& other) const {
    return id_ == other.id_ &&
           clientId_ == other.clientId_ &&
           lessonId_ == other.lessonId_ &&
           rating_ == other.rating_ &&
           comment_ == other.comment_ &&
           status_ == other.status_;
}

bool Review::operator!=(const Review& other) const {
    return !(*this == other);
}