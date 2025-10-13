#ifndef QUERYFACTORY_HPP
#define QUERYFACTORY_HPP

#include <string>

class QueryFactory {
public:
    // Booking queries
    static std::string createFindConflictingBookingsQuery();
    
    // Lesson queries  
    static std::string createFindConflictingLessonsQuery();
    static std::string createFindUpcomingLessonsQuery();
    
    // Review queries
    static std::string createGetAverageRatingForTrainerQuery();
    
    // Enrollment queries
    static std::string createCountEnrollmentsByLessonQuery();
    
    // Subscription queries
    static std::string createFindExpiringSubscriptionsQuery();
    
    // Trainer queries
    static std::string createFindBySpecializationQuery();
    static std::string createGetTrainerSpecializationsQuery();

    // Branch queries
    std::string createFindByStudioIdQuery();

    // Studios queries
    std::string createFindMainStudioQuery();

    // Dance_halls queries
    std::string createFindByBranchIdQuery();

    // Reviews queries
    std::string createFindByClientAndLessonQuery();
    std::string createFindPendingModerationQuery();

    // Subscriptions queries
    std::string createFindActiveSubscriptionsQuery() ;

    // Subscriptions_type queries
    std::string createFindAllActiveSubscriptionTypesQuery();
};

#endif // QUERYFACTORY_HPP