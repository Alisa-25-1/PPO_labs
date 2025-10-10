#include "Lesson.hpp"
#include <regex>
#include <stdexcept>
#include <chrono>

Lesson::Lesson() 
    : id_(UUID()), type_(LessonType::OPEN_CLASS), name_(""), description_(""),
      startTime_(std::chrono::system_clock::now()), durationMinutes_(60),
      difficulty_(DifficultyLevel::BEGINNER), maxParticipants_(10),
      currentParticipants_(0), price_(0.0), status_(LessonStatus::SCHEDULED),
      trainerId_(UUID()), hallId_(UUID()) {}

Lesson::Lesson(const UUID& id, LessonType type, const std::string& name,
               const std::chrono::system_clock::time_point& startTime, int durationMinutes,
               DifficultyLevel difficulty, int maxParticipants, double price,
               const UUID& trainerId, const UUID& hallId)
    : id_(id), type_(type), name_(name), description_(""), startTime_(startTime),
      durationMinutes_(durationMinutes), difficulty_(difficulty), 
      maxParticipants_(maxParticipants), currentParticipants_(0), price_(price),
      status_(LessonStatus::SCHEDULED), trainerId_(trainerId), hallId_(hallId) {
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid lesson data");
    }
}

// Геттеры
UUID Lesson::getId() const { return id_; }
LessonType Lesson::getType() const { return type_; }
std::string Lesson::getName() const { return name_; }
std::string Lesson::getDescription() const { return description_; }
std::chrono::system_clock::time_point Lesson::getStartTime() const { return startTime_; }
int Lesson::getDurationMinutes() const { return durationMinutes_; }
DifficultyLevel Lesson::getDifficulty() const { return difficulty_; }
int Lesson::getMaxParticipants() const { return maxParticipants_; }
int Lesson::getCurrentParticipants() const { return currentParticipants_; }
double Lesson::getPrice() const { return price_; }
LessonStatus Lesson::getStatus() const { return status_; }
UUID Lesson::getTrainerId() const { return trainerId_; }
UUID Lesson::getHallId() const { return hallId_; }

// Сеттеры
void Lesson::setDescription(const std::string& description) {
    if (!isValidDescription(description)) {
        throw std::invalid_argument("Invalid description");
    }
    description_ = description;
}

void Lesson::setStatus(LessonStatus status) {
    status_ = status;
}

// Бизнес-логика
bool Lesson::isFull() const {
    return currentParticipants_ >= maxParticipants_;
}

bool Lesson::hasSpots() const {
    return currentParticipants_ < maxParticipants_;
}

bool Lesson::canBeBooked() const {
    return status_ == LessonStatus::SCHEDULED && hasSpots();
}

bool Lesson::isOngoing() const {
    auto now = std::chrono::system_clock::now();
    auto endTime = startTime_ + std::chrono::minutes(durationMinutes_);
    return now >= startTime_ && now <= endTime;
}

bool Lesson::isCompleted() const {
    auto now = std::chrono::system_clock::now();
    auto endTime = startTime_ + std::chrono::minutes(durationMinutes_);
    return now > endTime || status_ == LessonStatus::COMPLETED;
}

bool Lesson::addParticipant() {
    if (isFull()) {
        return false;
    }
    currentParticipants_++;
    return true;
}

bool Lesson::removeParticipant() {
    if (currentParticipants_ <= 0) {
        return false;
    }
    currentParticipants_--;
    return true;
}

TimeSlot Lesson::getTimeSlot() const {
    return TimeSlot(startTime_, durationMinutes_);
}

bool Lesson::isValid() const {
    return !id_.isNull() && id_.isValid() &&
           isValidName(name_) &&
           isValidDescription(description_) &&
           isValidDuration(durationMinutes_) &&
           isValidMaxParticipants(maxParticipants_) &&
           isValidPrice(price_) &&
           !trainerId_.isNull() && trainerId_.isValid() &&
           !hallId_.isNull() && hallId_.isValid();
}

// Валидация
bool Lesson::isValidName(const std::string& name) {
    return !name.empty() && name.length() <= 100 && name.length() >= 2;
}

bool Lesson::isValidDescription(const std::string& description) {
    return description.length() <= 1000;
}

bool Lesson::isValidDuration(int duration) {
    return duration > 0 && duration <= 8 * 60; // Максимум 8 часов
}

bool Lesson::isValidMaxParticipants(int maxParticipants) {
    return maxParticipants > 0 && maxParticipants <= 100;
}

bool Lesson::isValidPrice(double price) {
    return price >= 0.0;
}

// Операторы сравнения
bool Lesson::operator==(const Lesson& other) const {
    return id_ == other.id_ && 
           type_ == other.type_ && 
           name_ == other.name_ && 
           startTime_ == other.startTime_ && 
           durationMinutes_ == other.durationMinutes_;
}

bool Lesson::operator!=(const Lesson& other) const {
    return !(*this == other);
}