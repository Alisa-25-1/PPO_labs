#ifndef LESSON_HPP
#define LESSON_HPP

#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include "TimeSlot.hpp"
#include <string>
#include <chrono>

class Lesson {
private:
    UUID id_;
    LessonType type_;
    std::string name_;
    std::string description_;
    std::chrono::system_clock::time_point startTime_;
    int durationMinutes_;
    DifficultyLevel difficulty_;
    int maxParticipants_;
    int currentParticipants_;
    double price_;
    LessonStatus status_;
    UUID trainerId_;
    UUID hallId_;

public:
    Lesson();
    Lesson(const UUID& id, LessonType type, const std::string& name, 
           const std::chrono::system_clock::time_point& startTime, int durationMinutes,
           DifficultyLevel difficulty, int maxParticipants, double price,
           const UUID& trainerId, const UUID& hallId);
    
    // Геттеры
    UUID getId() const;
    LessonType getType() const;
    std::string getName() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getStartTime() const;
    int getDurationMinutes() const;
    DifficultyLevel getDifficulty() const;
    int getMaxParticipants() const;
    int getCurrentParticipants() const;
    double getPrice() const;
    LessonStatus getStatus() const;
    UUID getTrainerId() const;
    UUID getHallId() const;
    
    // Сеттеры
    void setDescription(const std::string& description);
    void setStatus(LessonStatus status);
    
    // Бизнес-логика
    bool isFull() const;
    bool hasSpots() const;
    bool canBeBooked() const;
    bool isOngoing() const;
    bool isCompleted() const;
    bool addParticipant();
    bool removeParticipant();
    TimeSlot getTimeSlot() const;
    bool isValid() const;
    
    // Валидация
    static bool isValidName(const std::string& name);
    static bool isValidDescription(const std::string& description);
    static bool isValidDuration(int duration);
    static bool isValidMaxParticipants(int maxParticipants);
    static bool isValidPrice(double price);
    
    // Операторы сравнения
    bool operator==(const Lesson& other) const;
    bool operator!=(const Lesson& other) const;
};

#endif // LESSON_HPP