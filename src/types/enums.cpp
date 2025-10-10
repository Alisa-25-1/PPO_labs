#include "enums.hpp"

std::string EnumUtils::accountStatusToString(AccountStatus status) {
    switch (status) {
        case AccountStatus::ACTIVE: return "ACTIVE";
        case AccountStatus::INACTIVE: return "INACTIVE";
        case AccountStatus::SUSPENDED: return "SUSPENDED";
        default: return "UNKNOWN";
    }
}

std::string EnumUtils::bookingStatusToString(BookingStatus status) {
    switch (status) {
        case BookingStatus::PENDING: return "PENDING";
        case BookingStatus::CONFIRMED: return "CONFIRMED";
        case BookingStatus::CANCELLED: return "CANCELLED";
        case BookingStatus::COMPLETED: return "COMPLETED";
        default: return "UNKNOWN";
    }
}

std::string EnumUtils::lessonTypeToString(LessonType type) {
    switch (type) {
        case LessonType::OPEN_CLASS: return "OPEN_CLASS";
        case LessonType::SPECIAL_COURSE: return "SPECIAL_COURSE";
        case LessonType::INDIVIDUAL: return "INDIVIDUAL";
        case LessonType::MASTERCLASS: return "MASTERCLASS";
        default: return "UNKNOWN";
    }
}

std::string EnumUtils::difficultyLevelToString(DifficultyLevel level) {
    switch (level) {
        case DifficultyLevel::BEGINNER: return "BEGINNER";
        case DifficultyLevel::INTERMEDIATE: return "INTERMEDIATE";
        case DifficultyLevel::ADVANCED: return "ADVANCED";
        default: return "UNKNOWN";
    }
}

std::string EnumUtils::lessonStatusToString(LessonStatus status) {
    switch (status) {
        case LessonStatus::SCHEDULED: return "SCHEDULED";
        case LessonStatus::ONGOING: return "ONGOING";
        case LessonStatus::COMPLETED: return "COMPLETED";
        case LessonStatus::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

std::string EnumUtils::subscriptionStatusToString(SubscriptionStatus status) {
    switch (status) {
        case SubscriptionStatus::ACTIVE: return "ACTIVE";
        case SubscriptionStatus::SUSPENDED: return "SUSPENDED";
        case SubscriptionStatus::EXPIRED: return "EXPIRED";
        case SubscriptionStatus::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

std::string EnumUtils::enrollmentStatusToString(EnrollmentStatus status) {
    switch (status) {
        case EnrollmentStatus::REGISTERED: return "REGISTERED";
        case EnrollmentStatus::CANCELLED: return "CANCELLED";
        case EnrollmentStatus::ATTENDED: return "ATTENDED";
        case EnrollmentStatus::MISSED: return "MISSED";
        default: return "UNKNOWN";
    }
}

std::string EnumUtils::reviewStatusToString(ReviewStatus status) {
    switch (status) {
        case ReviewStatus::PENDING_MODERATION: return "PENDING_MODERATION";
        case ReviewStatus::APPROVED: return "APPROVED";
        case ReviewStatus::REJECTED: return "REJECTED";
        default: return "UNKNOWN";
    }
}