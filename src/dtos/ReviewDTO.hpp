#ifndef REVIEWDTO_HPP
#define REVIEWDTO_HPP

#include "../types/uuid.hpp"
#include "../models/Review.hpp"
#include <string>
#include <iomanip>

struct ReviewRequestDTO {
    UUID clientId;
    UUID lessonId;
    int rating;
    std::string comment;
    
    bool validate() const {
        return !clientId.isNull() && clientId.isValid() &&
               !lessonId.isNull() && lessonId.isValid() &&
               rating >= 1 && rating <= 5 &&
               comment.length() <= 1000;
    }
};

struct ReviewResponseDTO {
    UUID reviewId;
    UUID clientId;
    UUID lessonId;
    int rating;
    std::string comment;
    std::string publicationDate;
    std::string status;
    std::string clientName; // Может быть заполнено дополнительно
    
    ReviewResponseDTO(const Review& review) {
        reviewId = review.getId();
        clientId = review.getClientId();
        lessonId = review.getLessonId();
        rating = review.getRating();
        comment = review.getComment();
        
        auto time_t = std::chrono::system_clock::to_time_t(review.getPublicationDate());
        std::tm tm = *std::localtime(&time_t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        publicationDate = oss.str();
        
        switch (review.getStatus()) {
            case ReviewStatus::PENDING_MODERATION:
                status = "PENDING_MODERATION";
                break;
            case ReviewStatus::APPROVED:
                status = "APPROVED";
                break;
            case ReviewStatus::REJECTED:
                status = "REJECTED";
                break;
            default:
                status = "UNKNOWN";
                break;
        }
    }
};

#endif // REVIEWDTO_HPP