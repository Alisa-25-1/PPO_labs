#ifndef REVIEW_HPP
#define REVIEW_HPP

#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include <string>
#include <chrono>

class Review {
private:
    UUID id_;
    UUID clientId_;
    UUID lessonId_;
    int rating_;
    std::string comment_;
    std::chrono::system_clock::time_point publicationDate_;
    ReviewStatus status_;

public:
    Review();
    Review(const UUID& id, const UUID& clientId, const UUID& lessonId, 
           int rating, const std::string& comment);
    
    // Геттеры
    UUID getId() const;
    UUID getClientId() const;
    UUID getLessonId() const;
    int getRating() const;
    std::string getComment() const;
    std::chrono::system_clock::time_point getPublicationDate() const;
    ReviewStatus getStatus() const;
    
    // Бизнес-логика
    void approve();
    void reject();
    bool isApproved() const;
    bool isPending() const;
    bool isValid() const;
    
    // Валидация
    static bool isValidRating(int rating);
    static bool isValidComment(const std::string& comment);
    
    // Операторы сравнения
    bool operator==(const Review& other) const;
    bool operator!=(const Review& other) const;
};

#endif // REVIEW_HPP