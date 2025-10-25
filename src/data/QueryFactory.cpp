#include "QueryFactory.hpp"

// Все методы уже реализованы как статические в .cpp файле
// Компилятор понимает это по объявлению в .hpp

std::string QueryFactory::createFindConflictingBookingsQuery() {
    return 
        "SELECT id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at "
        "FROM bookings "
        "WHERE hall_id = $1 AND status IN ('PENDING', 'CONFIRMED') "
        "AND (start_time, start_time + (duration_minutes * interval '1 minute')) "
        "OVERLAPS ($2::timestamp, $2::timestamp + ($3 * interval '1 minute'))";
}

std::string QueryFactory::createFindConflictingLessonsQuery() {
    return 
        "SELECT id, type, name, description, start_time, duration_minutes, "
        "difficulty, max_participants, current_participants, price, status, "
        "trainer_id, hall_id "
        "FROM lessons "
        "WHERE hall_id = $1 AND status IN ('SCHEDULED', 'ONGOING') "
        "AND (start_time, start_time + (duration_minutes * interval '1 minute')) "
        "OVERLAPS ($2::timestamp, $2::timestamp + ($3 * interval '1 minute'))";
}

std::string QueryFactory::createFindUpcomingLessonsQuery() {
    return 
        "SELECT id, type, name, description, start_time, duration_minutes, "
        "difficulty, max_participants, current_participants, price, status, "
        "trainer_id, hall_id "
        "FROM lessons WHERE start_time BETWEEN CURRENT_TIMESTAMP AND CURRENT_TIMESTAMP + $1 * INTERVAL '1 day' "
        "AND status = 'SCHEDULED'";
}

std::string QueryFactory::createGetAverageRatingForTrainerQuery() {
    return 
        "SELECT AVG(r.rating) as avg_rating "
        "FROM reviews r "
        "JOIN lessons l ON r.lesson_id = l.id "
        "WHERE l.trainer_id = $1 AND r.status = 'APPROVED'";
}

std::string QueryFactory::createCountEnrollmentsByLessonQuery() {
    return 
        "SELECT COUNT(*) as count FROM enrollments WHERE lesson_id = $1 AND status = 'REGISTERED'";
}

std::string QueryFactory::createFindExpiringSubscriptionsQuery() {
    return 
        "SELECT id, client_id, subscription_type_id, start_date, end_date, "
        "remaining_visits, status, purchase_date "
        "FROM subscriptions WHERE status = 'ACTIVE' AND end_date BETWEEN CURRENT_DATE AND CURRENT_DATE + $1 * INTERVAL '1 day'";
}

std::string QueryFactory::createFindBySpecializationQuery() {
    return 
        "SELECT t.id, t.name, t.biography, t.qualification_level, t.is_active "
        "FROM trainers t "
        "JOIN trainer_specializations ts ON t.id = ts.trainer_id "
        "WHERE ts.specialization = $1 AND t.is_active = true";
}

std::string QueryFactory::createGetTrainerSpecializationsQuery() {
    return 
        "SELECT specialization FROM trainer_specializations WHERE trainer_id = $1";
}

std::string QueryFactory::createFindByStudioIdQuery() {
    return 
        "SELECT id, name, address, phone, open_time, close_time, studio_id "
        "FROM branches WHERE studio_id = $1";
}

std::string QueryFactory::createFindMainStudioQuery() {
    return 
        "SELECT id, name, description, contact_email "
        "FROM studios ORDER BY id LIMIT 1";
}

std::string QueryFactory::createFindByBranchIdQuery() {
    return 
        "SELECT id, name, description, capacity, floor_type, equipment, branch_id "
        "FROM dance_halls WHERE branch_id = $1";
}

std::string QueryFactory::createFindByClientAndLessonQuery() {
    return 
        "SELECT id, client_id, lesson_id, rating, comment, publication_date, status "
        "FROM reviews WHERE client_id = $1 AND lesson_id = $2";
}

std::string QueryFactory::createFindPendingModerationQuery() {
    return 
        "SELECT id, client_id, lesson_id, rating, comment, publication_date, status "
        "FROM reviews WHERE status = 'PENDING_MODERATION'";
}

std::string QueryFactory::createFindActiveSubscriptionsQuery() {
    return 
        "SELECT id, client_id, subscription_type_id, start_date, end_date, "
        "remaining_visits, status, purchase_date "
        "FROM subscriptions WHERE status = 'ACTIVE'";
}

std::string QueryFactory::createFindAllActiveSubscriptionTypesQuery() {
    return 
        "SELECT id, name, description, validity_days, visit_count, is_unlimited, price "
        "FROM subscription_types WHERE is_active = true";
}

std::string QueryFactory::createFindAttendanceByClientAndPeriodQuery() {
    return R"(
        SELECT id, client_id, entity_id, type, status, scheduled_time, actual_time, notes
        FROM attendance 
        WHERE client_id = $1 
        AND scheduled_time BETWEEN $2 AND $3
        ORDER BY scheduled_time DESC
    )";
}