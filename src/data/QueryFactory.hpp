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
    static std::string createFindByStudioIdQuery(); 

    // Studios queries
    static std::string createFindMainStudioQuery(); 

    // Dance_halls queries
    static std::string createFindByBranchIdQuery(); 

    // Reviews queries
    static std::string createFindByClientAndLessonQuery(); 
    static std::string createFindPendingModerationQuery(); 

    // Subscriptions queries
    static std::string createFindActiveSubscriptionsQuery(); 

    // Subscriptions_type queries
    static std::string createFindAllActiveSubscriptionTypesQuery(); 

    // Attendance queries
    static std::string createFindAttendanceByClientAndPeriodQuery(); 
};

#endif // QUERYFACTORY_HPP