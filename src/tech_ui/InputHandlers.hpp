#ifndef INPUTHANDLERS_HPP
#define INPUTHANDLERS_HPP

#include "../types/uuid.hpp"
#include "../models/TimeSlot.hpp"
#include "../types/enums.hpp"
#include "../models/DanceHall.hpp"
#include <iostream>
#include <string>
#include <chrono>

class InputHandlers {
public:
    static UUID readUUID(const std::string& prompt);
    static std::string readString(const std::string& prompt, size_t maxLength = 255);
    static int readInt(const std::string& prompt, int min = 0, int max = 1000000);
    static double readDouble(const std::string& prompt, double min = 0.0);
    static std::chrono::system_clock::time_point readDateTime(const std::string& prompt);
    static TimeSlot readTimeSlot();
    static bool readYesNo(const std::string& prompt);
    static std::string trim(const std::string& str);
    
    // Специализированные методы для конкретных сущностей
    static std::string readEmail(const std::string& prompt);
    static std::string readPhone(const std::string& prompt);
    static std::string readPassword(const std::string& prompt);
    
    // Enum ввод
    static LessonType readLessonType();
    static DifficultyLevel readDifficultyLevel();
    static int readRating();
    static UUID readHallFromList(const std::vector<DanceHall>& halls, const std::string& prompt);
    static std::string readQualificationLevel();
    
    // Валидация
    static bool isValidEmail(const std::string& email);
    static bool isValidPhone(const std::string& phone);
};

#endif // INPUTHANDLERS_HPP