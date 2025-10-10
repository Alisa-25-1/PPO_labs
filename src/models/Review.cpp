#include "Review.hpp"
#include <regex>
#include <stdexcept>

Review::Review() 
    : id_(UUID()), clientId_(UUID()), lessonId_(UUID()), rating_(0), comment_(""),
      publicationDate_(std::chrono::system_clock::now()), 
      status_(ReviewStatus::PENDING_MODERATION) {}

Review::Review(const UUID& id, const UUID& clientId, const UUID& lessonId, 
               int rating, const std::string& comment)
    : id_(id), clientId_(clientId), lessonId_(lessonId), rating_(rating), 
      comment_(comment), publicationDate_(std::chrono::system_clock::now()),
      status_(ReviewStatus::PENDING_MODERATION) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid review data");
    }
}

UUID Review::getId() const { return id_; }
UUID Review::getClientId() const { return clientId_; }
UUID Review::getLessonId() const { return lessonId_; }
int Review::getRating() const { return rating_; }
std::string Review::getComment() const { return comment_; }
std::chrono::system_clock::time_point Review::getPublicationDate() const { return publicationDate_; }
ReviewStatus Review::getStatus() const { return status_; }

void Review::approve() {
    status_ = ReviewStatus::APPROVED;
}

void Review::reject() {
    status_ = ReviewStatus::REJECTED;
}

bool Review::isApproved() const {
    return status_ == ReviewStatus::APPROVED;
}

bool Review::isPending() const {
    return status_ == ReviewStatus::PENDING_MODERATION;
}

bool Review::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           !clientId_.isNull() && clientId_.isValid() &&
           !lessonId_.isNull() && lessonId_.isValid() &&
           isValidRating(rating_) &&
           isValidComment(comment_) &&
           publicationDate_ <= std::chrono::system_clock::now();
}

bool Review::isValidRating(int rating) {
    return rating >= 1 && rating <= 5;
}

bool Review::isValidComment(const std::string& comment) {
    if (comment.empty()) {
        return true; // Комментарий может быть пустым
    }
    return comment.length() <= 1000;
}

bool Review::operator==(const Review& other) const {
    return id_ == other.id_ && 
           clientId_ == other.clientId_ && 
           lessonId_ == other.lessonId_ && 
           rating_ == other.rating_ && 
           comment_ == other.comment_;
}

bool Review::operator!=(const Review& other) const {
    return !(*this == other);
}