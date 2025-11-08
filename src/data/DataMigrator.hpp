#ifndef DATAMIGRATOR_HPP
#define DATAMIGRATOR_HPP

#include "IRepositoryFactory.hpp"
#include "../models/Client.hpp"
#include "../models/DanceHall.hpp"
#include "../models/Booking.hpp"
#include "../models/Lesson.hpp"
#include "../models/Trainer.hpp"
#include "../models/Enrollment.hpp"
#include "../models/Review.hpp"
#include "../models/Subscription.hpp"
#include "../models/SubscriptionType.hpp"
#include "../models/Attendance.hpp"
#include "../models/Studio.hpp"
#include "../models/Branch.hpp"
#include "../repositories/impl/PostgreSQLClientRepository.hpp"
#include <memory>
#include <iostream>

class DataMigrator {
private:
    std::shared_ptr<IRepositoryFactory> sourceFactory_;
    std::shared_ptr<IRepositoryFactory> targetFactory_;
    std::string migrationStrategy_;  // "upsert" или "overwrite"

public:
    DataMigrator(std::shared_ptr<IRepositoryFactory> sourceFactory, 
                 std::shared_ptr<IRepositoryFactory> targetFactory,
                 const std::string& strategy = "upsert");
                 
    // Основной метод для запуска полной миграции
    bool migrateAll();

private:
    // Приватные методы для миграции каждой сущности
    bool migrateStudios();
    bool migrateBranches();
    bool migrateDanceHalls();
    bool migrateTrainers();
    bool migrateClients();
    bool migrateSubscriptionTypes();
    bool migrateSubscriptions();
    bool migrateLessons();
    bool migrateEnrollments();
    bool migrateBookings();
    bool migrateReviews();
    bool migrateAttendance();
};

#endif // DATAMIGRATOR_HPP