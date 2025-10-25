#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <string>

// Статусы аккаунта
enum class AccountStatus {
    ACTIVE,         // Активный 
    INACTIVE,       // Неактивный
    SUSPENDED       // Приостановлен
};

// Статусы бронирования
enum class BookingStatus {
    PENDING,        // На подтверждении
    CONFIRMED,      // Подтверждено
    CANCELLED,      // Отменено
    COMPLETED       // Завершено
};

enum class LessonType {
    OPEN_CLASS,      // Открытый класс
    SPECIAL_COURSE,  // Специальный курс  
    INDIVIDUAL,      // Индивидуальное
    MASTERCLASS      // Мастер-класс
};

enum class DifficultyLevel {
    BEGINNER,        // Начинающие
    INTERMEDIATE,    // Средний
    ADVANCED,        // Продвинутый
    ALL_LEVELS       // Все уровни
};

enum class LessonStatus {
    SCHEDULED,       // Запланировано
    ONGOING,         // В процессе
    COMPLETED,       // Завершено
    CANCELLED        // Отменено
};

enum class SubscriptionStatus {
    ACTIVE,          // Активный
    SUSPENDED,       // Приостановлен
    EXPIRED,         // Истекший
    CANCELLED        // Отмененный
};

enum class EnrollmentStatus {
    REGISTERED,      // Зарегистрирован
    CANCELLED,       // Отменено
    ATTENDED,        // Посещено
    MISSED           // Пропущено
};

enum class ReviewStatus {
    PENDING_MODERATION, // На модерации
    APPROVED,           // Одобрен
    REJECTED            // Отклонен
};

enum class AttendanceType {
    LESSON,
    BOOKING
};

enum class AttendanceStatus {
    SCHEDULED,
    VISITED,
    CANCELLED, 
    NO_SHOW
};

// Дополним EnumUtils
namespace EnumUtils {
    std::string accountStatusToString(AccountStatus status);
    std::string bookingStatusToString(BookingStatus status);
    std::string lessonTypeToString(LessonType type);
    std::string difficultyLevelToString(DifficultyLevel level);
    std::string lessonStatusToString(LessonStatus status);
    std::string subscriptionStatusToString(SubscriptionStatus status);
    std::string enrollmentStatusToString(EnrollmentStatus status);
    std::string reviewStatusToString(ReviewStatus status);
    
    LessonType stringToLessonType(const std::string& str);
    DifficultyLevel stringToDifficultyLevel(const std::string& str);
}

#endif // ENUMS_HPP