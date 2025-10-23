#ifndef LESSONDTO_HPP
#define LESSONDTO_HPP

#include "../types/uuid.hpp"
#include "../models/TimeSlot.hpp"
#include "../models/Lesson.hpp"
#include <string>

struct LessonRequestDTO {
    UUID trainerId;
    UUID hallId;
    std::chrono::system_clock::time_point startTime;
    int durationMinutes;
    LessonType type;
    std::string name;
    std::string description;
    DifficultyLevel difficulty;
    int maxParticipants;
    double price;
    
    bool validate() const {
        return !trainerId.isNull() && trainerId.isValid() &&
               !hallId.isNull() && hallId.isValid() &&
               startTime > std::chrono::system_clock::now() &&
               durationMinutes > 0 && durationMinutes <= 480 &&
               !name.empty() && name.length() <= 100 &&
               description.length() <= 1000 &&
               maxParticipants > 0 && maxParticipants <= 100 &&
               price >= 0.0;
    }
};

struct LessonResponseDTO {
    UUID lessonId;
    UUID trainerId;
    UUID hallId;
    TimeSlot timeSlot;
    LessonType type;
    std::string name;
    std::string description;
    DifficultyLevel difficulty;
    int maxParticipants;
    int currentParticipants;
    double price;
    LessonStatus status;
    std::string trainerName;
    std::string hallName;
    
    LessonResponseDTO(const Lesson& lesson) {
        lessonId = lesson.getId();
        trainerId = lesson.getTrainerId();
        hallId = lesson.getHallId();
        timeSlot = lesson.getTimeSlot();
        type = lesson.getType();
        name = lesson.getName();
        description = lesson.getDescription();
        difficulty = lesson.getDifficulty();
        maxParticipants = lesson.getMaxParticipants();
        currentParticipants = lesson.getCurrentParticipants();
        price = lesson.getPrice();
        status = lesson.getStatus();
        // trainerName и hallName будут заполняться дополнительными запросами
    }
};

#endif // LESSONDTO_HPP