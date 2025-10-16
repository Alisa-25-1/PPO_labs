#include "InputHandlers.hpp"
#include <regex>
#include <iomanip>
#include <sstream>
#include <ctime>

UUID InputHandlers::readUUID(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            UUID newUuid = UUID::generate();
            std::cout << "Сгенерирован новый UUID: " << newUuid.toString() << std::endl;
            return newUuid;
        }
        
        try {
            return UUID::fromString(input);
        } catch (const std::exception&) {
            std::cout << "❌ Неверный формат UUID. Попробуйте снова или оставьте пустым для генерации." << std::endl;
        }
    }
}

std::string InputHandlers::readString(const std::string& prompt, size_t maxLength) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            std::cout << "❌ Поле не может быть пустым. Попробуйте снова." << std::endl;
            continue;
        }
        
        if (input.length() > maxLength) {
            std::cout << "❌ Слишком длинная строка (максимум " << maxLength << " символов). Попробуйте снова." << std::endl;
            continue;
        }
        
        return input;
    }
}

int InputHandlers::readInt(const std::string& prompt, int min, int max) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        try {
            int value = std::stoi(input);
            if (value >= min && value <= max) {
                return value;
            } else {
                std::cout << "❌ Число должно быть в диапазоне от " << min << " до " << max << ". Попробуйте снова." << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "❌ Неверный формат числа. Попробуйте снова." << std::endl;
        }
    }
}

double InputHandlers::readDouble(const std::string& prompt, double min) {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        
        try {
            double value = std::stod(input);
            if (value >= min) {
                return value;
            } else {
                std::cout << "❌ Число должно быть не меньше " << min << ". Попробуйте снова." << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "❌ Неверный формат числа. Попробуйте снова." << std::endl;
        }
    }
}

std::chrono::system_clock::time_point InputHandlers::readDateTime(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " (формат: ГГГГ-ММ-ДД ЧЧ:ММ, например, 2023-12-25 14:30): ";
        std::string input;
        std::getline(std::cin, input);
        
        std::tm tm = {};
        std::istringstream ss(input);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
        
        if (ss.fail()) {
            std::cout << "❌ Неверный формат даты и времени. Используйте формат ГГГГ-ММ-ДД ЧЧ:ММ" << std::endl;
            continue;
        }
        
        // Проверка корректности даты
        auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        auto now = std::chrono::system_clock::now();
        
        if (timePoint < now) {
            std::cout << "❌ Нельзя бронировать зал в прошедшем времени. Выберите будущую дату." << std::endl;
            continue;
        }
        
        return timePoint;
    }
}

TimeSlot InputHandlers::readTimeSlot() {
    std::cout << "--- Ввод временного интервала ---" << std::endl;
    auto startTime = readDateTime("Введите время начала");
    
    while (true) {
        int duration = readInt("Введите продолжительность в минутах (кратно 60): ", 60, 8*60);
        
        if (duration % 60 != 0) {
            std::cout << "❌ Продолжительность должна быть кратной 60 минутам (1 час)." << std::endl;
            std::cout << "   Доступные значения: 60, 120, 180, 240, 300, 360, 420, 480 минут" << std::endl;
            continue;
        }
        
        return TimeSlot(startTime, duration);
    }
}

bool InputHandlers::readYesNo(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " (y/n): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input == "y" || input == "Y") {
            return true;
        } else if (input == "n" || input == "N") {
            return false;
        } else {
            std::cout << "❌ Пожалуйста, введите 'y' или 'n'." << std::endl;
        }
    }
}

std::string InputHandlers::readEmail(const std::string& prompt) {
    while (true) {
        std::string email = readString(prompt);
        if (isValidEmail(email)) {
            return email;
        } else {
            std::cout << "❌ Неверный формат email. Попробуйте снова." << std::endl;
        }
    }
}

std::string InputHandlers::readPhone(const std::string& prompt) {
    while (true) {
        std::string phone = readString(prompt);
        if (isValidPhone(phone)) {
            return phone;
        } else {
            std::cout << "❌ Неверный формат телефона. Попробуйте снова." << std::endl;
        }
    }
}

std::string InputHandlers::readPassword(const std::string& prompt) {
    while (true) {
        std::string password = readString(prompt);
        if (password.length() >= 8) {
            return password;
        } else {
            std::cout << "❌ Пароль должен содержать не менее 8 символов. Попробуйте снова." << std::endl;
        }
    }
}

bool InputHandlers::isValidEmail(const std::string& email) {
    std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, emailPattern);
}

bool InputHandlers::isValidPhone(const std::string& phone) {
    // Упрощенная проверка телефона: допускаем цифры, пробелы, скобки, дефисы и знак +
    std::regex phonePattern(R"(^\+?[0-9\s\-\(\)]{10,20}$)");
    return std::regex_match(phone, phonePattern);
}

UUID InputHandlers::readHallFromList(const std::vector<DanceHall>& halls, const std::string& prompt) {
    if (halls.empty()) {
        throw std::runtime_error("Нет доступных залов для выбора");
    }
    
    std::cout << prompt << std::endl;
    for (size_t i = 0; i < halls.size(); ++i) {
        const auto& hall = halls[i];
        std::cout << (i + 1) << ". " << hall.getName() 
                  << " (Вместимость: " << hall.getCapacity() << " чел.)"
                  << " - " << hall.getDescription() << std::endl;
    }
    
    int choice = readInt("Выберите номер зала: ", 1, static_cast<int>(halls.size()));
    return halls[choice - 1].getId();
}

LessonType InputHandlers::readLessonType() {
    std::cout << "Тип занятия:" << std::endl;
    std::cout << "1. Открытый класс" << std::endl;
    std::cout << "2. Специальный курс" << std::endl;
    std::cout << "3. Индивидуальное" << std::endl;
    std::cout << "4. Мастер-класс" << std::endl;
    
    int choice = readInt("Выберите тип: ", 1, 4);
    switch (choice) {
        case 1: return LessonType::OPEN_CLASS;
        case 2: return LessonType::SPECIAL_COURSE;
        case 3: return LessonType::INDIVIDUAL;
        case 4: return LessonType::MASTERCLASS;
        default: return LessonType::OPEN_CLASS;
    }
}

DifficultyLevel InputHandlers::readDifficultyLevel() {
    std::cout << "Уровень сложности:" << std::endl;
    std::cout << "1. Начинающий" << std::endl;
    std::cout << "2. Средний" << std::endl;
    std::cout << "3. Продвинутый" << std::endl;
    std::cout << "4. Все уровни" << std::endl;
    
    int choice = readInt("Выберите уровень: ", 1, 4);
    switch (choice) {
        case 1: return DifficultyLevel::BEGINNER;
        case 2: return DifficultyLevel::INTERMEDIATE;
        case 3: return DifficultyLevel::ADVANCED;
        case 4: return DifficultyLevel::ALL_LEVELS;
        default: return DifficultyLevel::BEGINNER;
    }
}

int InputHandlers::readRating() {
    return readInt("Оценка (1-5): ", 1, 5);
}