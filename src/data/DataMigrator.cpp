#include "DataMigrator.hpp"
#include "../core/Logger.hpp"
#include <iostream>

DataMigrator::DataMigrator(std::shared_ptr<IRepositoryFactory> sourceFactory, 
                         std::shared_ptr<IRepositoryFactory> targetFactory, 
                         const std::string& strategy)
    : sourceFactory_(std::move(sourceFactory)), 
      targetFactory_(std::move(targetFactory)),
      migrationStrategy_(strategy) {}

bool DataMigrator::migrateAll() {
    auto& logger = Logger::getInstance();
    logger.info("Starting complete data migration between databases", "DataMigrator");
    
    try {
        bool success = true;
        
        // Миграция в правильном порядке для сохранения ссылочной целостности
        success = migrateStudios() && success;
        success = migrateBranches() && success;
        success = migrateDanceHalls() && success;
        success = migrateTrainers() && success;
        success = migrateClients() && success;
        success = migrateSubscriptionTypes() && success;
        success = migrateSubscriptions() && success;
        success = migrateLessons() && success;
        success = migrateEnrollments() && success;
        success = migrateBookings() && success;
        success = migrateReviews() && success;
        success = migrateAttendance() && success;
        
        if (success) {
            logger.info("Data migration completed successfully", "DataMigrator");
            //std::cout << "✅ Data migration completed successfully!" << std::endl;
        } else {
            logger.error("Data migration completed with errors", "DataMigrator");
            //std::cerr << "❌ Data migration completed with errors!" << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        logger.error(std::string("Data migration failed: ") + e.what(), "DataMigrator");
        //std::cerr << "💥 Data migration failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateStudios() {
    try {
        auto sourceRepo = sourceFactory_->createStudioRepository();
        auto targetRepo = targetFactory_->createStudioRepository();
        
        auto studios = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& studio : studios) {
            // Проверяем, существует ли уже студия в целевой БД
            bool exists = targetRepo->exists(studio.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    // Обновляем существующую запись
                    if (targetRepo->update(studio)) {
                        updatedCount++;
                        std::cout << "✅ Обновлена студия: " << studio.getName() << std::endl;
                    } else {
                        std::cerr << "❌ Не удалось обновить студию: " << studio.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    // Пропускаем существующие записи в режиме upsert
                    skippedCount++;
                    //std::cout << "⚠️  Пропущена существующая студия: " << studio.getName() << std::endl;
                }
            } else {
                // Вставляем новую запись
                if (targetRepo->save(studio)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирована студия: " << studio.getName() << std::endl;
                } else {
                    std::cerr << "❌ Не удалось мигрировать студию: " << studio.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Студии: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << studios.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Ошибка миграции студий: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateBranches() {
    try {
        auto sourceRepo = sourceFactory_->createBranchRepository();
        auto targetRepo = targetFactory_->createBranchRepository();
        
        auto branches = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& branch : branches) {
            // Проверяем существование студии в целевой БД
            auto studioRepo = targetFactory_->createStudioRepository();
            if (!studioRepo->exists(branch.getStudioId())) {
                std::cerr << "❌ Referenced studio not found: " << branch.getStudioId().toString() 
                         << " for branch: " << branch.getName() << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(branch.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(branch)) {
                        updatedCount++;
                        std::cout << "✅ Обновлен филиал: " << branch.getName() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update branch: " << branch.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущен существующий филиал: " << branch.getName() << std::endl;
                }
            } else {
                if (targetRepo->save(branch)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирован филиал: " << branch.getName() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate branch: " << branch.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Филиалы: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << branches.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating branches: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateDanceHalls() {
    try {
        auto sourceRepo = sourceFactory_->createDanceHallRepository();
        auto targetRepo = targetFactory_->createDanceHallRepository();
        
        auto halls = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& hall : halls) {
            // Проверяем существование филиала
            auto branchRepo = targetFactory_->createBranchRepository();
            if (!branchRepo->exists(hall.getBranchId())) {
                std::cerr << "❌ Referenced branch not found: " << hall.getBranchId().toString() 
                         << " for hall: " << hall.getName() << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(hall.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(hall)) {
                        updatedCount++;
                        std::cout << "✅ Обновлен зал: " << hall.getName() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update dance hall: " << hall.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущен существующий зал: " << hall.getName() << std::endl;
                }
            } else {
                if (targetRepo->save(hall)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирован зал: " << hall.getName() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate dance hall: " << hall.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Залы: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << halls.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating dance halls: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateTrainers() {
    try {
        auto sourceRepo = sourceFactory_->createTrainerRepository();
        auto targetRepo = targetFactory_->createTrainerRepository();
        
        auto trainers = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& trainer : trainers) {
            bool exists = targetRepo->exists(trainer.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(trainer)) {
                        updatedCount++;
                        std::cout << "✅ Обновлен тренер: " << trainer.getName() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update trainer: " << trainer.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущен существующий тренер: " << trainer.getName() << std::endl;
                }
            } else {
                if (targetRepo->save(trainer)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирован тренер: " << trainer.getName() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate trainer: " << trainer.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Тренеры: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << trainers.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating trainers: " << e.what() << std::endl;
        return false;
    }
}

// Улучшенный migrateClients с предотвращением дублирования
bool DataMigrator::migrateClients() {
    try {
        auto sourceRepo = sourceFactory_->createClientRepository();
        auto targetRepo = targetFactory_->createClientRepository();
        
        auto clients = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        int errorCount = 0;
        
        std::cout << "👥 Migrating clients (" << clients.size() << " records)..." << std::endl;
        
        for (const auto& client : clients) {
            try {
                bool shouldMigrate = true;
                std::string conflictReason;
                
                // Проверяем существование по ID
                bool existsById = targetRepo->exists(client.getId());
                
                // Проверяем существование по email (уникальное поле)
                bool existsByEmail = false;
                std::optional<Client> existingClientByEmail;
                
                if (auto postgresRepo = std::dynamic_pointer_cast<PostgreSQLClientRepository>(targetRepo)) {
                    existsByEmail = postgresRepo->emailExists(client.getEmail());
                } else {
                    existingClientByEmail = targetRepo->findByEmail(client.getEmail());
                    existsByEmail = existingClientByEmail.has_value();
                }
                
                // Логика разрешения конфликтов
                if (existsById && existsByEmail) {
                    // ID и email уже существуют - это тот же клиент
                    if (migrationStrategy_ == "overwrite") {
                        // Обновляем существующего клиента
                        if (targetRepo->update(client)) {
                            updatedCount++;
                            std::cout << "✅ Updated existing client: " << client.getName() << std::endl;
                        } else {
                            errorCount++;
                            std::cerr << "❌ Failed to update client: " << client.getId().toString() << std::endl;
                        }
                    } else {
                        // Пропускаем в режиме upsert
                        skippedCount++;
                        std::cout << "⚠️  Skipped existing client: " << client.getName() << std::endl;
                    }
                    shouldMigrate = false;
                }
                else if (!existsById && existsByEmail) {
                    // Конфликт: другой клиент с таким email уже существует
                    std::cerr << "❌ Email conflict: Client " << client.getName() 
                              << " (" << client.getId().toString() << ") has email " 
                              << client.getEmail() << " that belongs to another client" << std::endl;
                    skippedCount++;
                    errorCount++;
                    shouldMigrate = false;
                }
                else if (existsById && !existsByEmail) {
                    // Клиент с таким ID существует, но email изменился
                    if (migrationStrategy_ == "overwrite") {
                        if (targetRepo->update(client)) {
                            updatedCount++;
                            std::cout << "✅ Updated client with changed email: " << client.getName() << std::endl;
                        } else {
                            errorCount++;
                            std::cerr << "❌ Failed to update client: " << client.getId().toString() << std::endl;
                        }
                    } else {
                        skippedCount++;
                        std::cout << "⚠️  Skipped client with changed email: " << client.getName() << std::endl;
                    }
                    shouldMigrate = false;
                }
                
                // Мигрируем нового клиента
                if (shouldMigrate) {
                    if (targetRepo->save(client)) {
                        migratedCount++;
                        std::cout << "✅ Migrated new client: " << client.getName() << std::endl;
                    } else {
                        errorCount++;
                        std::cerr << "❌ Failed to migrate client: " << client.getId().toString() << std::endl;
                    }
                }
                
            } catch (const std::exception& e) {
                errorCount++;
                std::cerr << "💥 Error migrating client " << client.getId().toString() << ": " << e.what() << std::endl;
            }
        }
        
        std::cout << "✅ Clients: migrated " << migratedCount << ", updated " << updatedCount 
                  << ", skipped " << skippedCount << ", errors " << errorCount 
                  << "/" << clients.size() << std::endl;
        
        return errorCount == 0;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating clients: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateSubscriptionTypes() {
    try {
        auto sourceRepo = sourceFactory_->createSubscriptionTypeRepository();
        auto targetRepo = targetFactory_->createSubscriptionTypeRepository();
        
        auto subscriptionTypes = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& subscriptionType : subscriptionTypes) {
            bool exists = targetRepo->exists(subscriptionType.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(subscriptionType)) {
                        updatedCount++;
                        std::cout << "✅ Обновлен тип абонемента: " << subscriptionType.getName() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update subscription type: " << subscriptionType.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущен существующий тип абонемента: " << subscriptionType.getName() << std::endl;
                }
            } else {
                if (targetRepo->save(subscriptionType)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирован тип абонемента: " << subscriptionType.getName() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate subscription type: " << subscriptionType.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Типы абонементов: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << subscriptionTypes.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating subscription types: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateSubscriptions() {
    try {
        auto sourceRepo = sourceFactory_->createSubscriptionRepository();
        auto targetRepo = targetFactory_->createSubscriptionRepository();
        
        auto subscriptions = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& subscription : subscriptions) {
            // Проверяем существование клиента и типа подписки
            auto clientRepo = targetFactory_->createClientRepository();
            auto typeRepo = targetFactory_->createSubscriptionTypeRepository();
            
            if (!clientRepo->exists(subscription.getClientId())) {
                std::cerr << "❌ Referenced client not found: " << subscription.getClientId().toString() 
                         << " for subscription" << std::endl;
                return false;
            }
            
            if (!typeRepo->exists(subscription.getSubscriptionTypeId())) {
                std::cerr << "❌ Referenced subscription type not found: " 
                         << subscription.getSubscriptionTypeId().toString() << " for subscription" << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(subscription.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(subscription)) {
                        updatedCount++;
                        std::cout << "✅ Обновлен абонемент: " << subscription.getId().toString() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update subscription: " << subscription.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущен существующий абонемент: " << subscription.getId().toString() << std::endl;
                }
            } else {
                if (targetRepo->save(subscription)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирован абонемент: " << subscription.getId().toString() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate subscription: " << subscription.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Абонементы: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << subscriptions.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating subscriptions: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateLessons() {
    try {
        auto sourceRepo = sourceFactory_->createLessonRepository();
        auto targetRepo = targetFactory_->createLessonRepository();
        
        auto lessons = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& lesson : lessons) {
            // Проверяем существование тренера и зала
            auto trainerRepo = targetFactory_->createTrainerRepository();
            auto hallRepo = targetFactory_->createDanceHallRepository();
            
            if (!trainerRepo->exists(lesson.getTrainerId())) {
                std::cerr << "❌ Referenced trainer not found: " << lesson.getTrainerId().toString() 
                         << " for lesson: " << lesson.getName() << std::endl;
                return false;
            }
            
            if (!hallRepo->exists(lesson.getHallId())) {
                std::cerr << "❌ Referenced hall not found: " << lesson.getHallId().toString() 
                         << " for lesson: " << lesson.getName() << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(lesson.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(lesson)) {
                        updatedCount++;
                        std::cout << "✅ Обновлено занятие: " << lesson.getName() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update lesson: " << lesson.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущено существующее занятие: " << lesson.getName() << std::endl;
                }
            } else {
                if (targetRepo->save(lesson)) {
                    migratedCount++;
                    std::cout << "✅ Мигрировано занятие: " << lesson.getName() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate lesson: " << lesson.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Занятия: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << lessons.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating lessons: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateEnrollments() {
    try {
        auto sourceRepo = sourceFactory_->createEnrollmentRepository();
        auto targetRepo = targetFactory_->createEnrollmentRepository();
        
        auto enrollments = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& enrollment : enrollments) {
            // Проверяем существование клиента и занятия
            auto clientRepo = targetFactory_->createClientRepository();
            auto lessonRepo = targetFactory_->createLessonRepository();
            
            if (!clientRepo->exists(enrollment.getClientId())) {
                std::cerr << "❌ Referenced client not found: " << enrollment.getClientId().toString() 
                         << " for enrollment" << std::endl;
                return false;
            }
            
            if (!lessonRepo->exists(enrollment.getLessonId())) {
                std::cerr << "❌ Referenced lesson not found: " << enrollment.getLessonId().toString() 
                         << " for enrollment" << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(enrollment.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(enrollment)) {
                        updatedCount++;
                        std::cout << "✅ Обновлена запись: " << enrollment.getId().toString() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update enrollment: " << enrollment.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущена существующая запись: " << enrollment.getId().toString() << std::endl;
                }
            } else {
                if (targetRepo->save(enrollment)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирована запись: " << enrollment.getId().toString() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate enrollment: " << enrollment.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Записи на занятия: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << enrollments.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating enrollments: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateBookings() {
    try {
        auto sourceRepo = sourceFactory_->createBookingRepository();
        auto targetRepo = targetFactory_->createBookingRepository();
        
        auto bookings = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& booking : bookings) {
            // Проверяем существование клиента и зала
            auto clientRepo = targetFactory_->createClientRepository();
            auto hallRepo = targetFactory_->createDanceHallRepository();
            
            if (!clientRepo->exists(booking.getClientId())) {
                std::cerr << "❌ Referenced client not found: " << booking.getClientId().toString() 
                         << " for booking" << std::endl;
                return false;
            }
            
            if (!hallRepo->exists(booking.getHallId())) {
                std::cerr << "❌ Referenced hall not found: " << booking.getHallId().toString() 
                         << " for booking" << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(booking.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(booking)) {
                        updatedCount++;
                        std::cout << "✅ Обновлено бронирование: " << booking.getId().toString() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update booking: " << booking.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущено существующее бронирование: " << booking.getId().toString() << std::endl;
                }
            } else {
                if (targetRepo->save(booking)) {
                    migratedCount++;
                    std::cout << "✅ Мигрировано бронирование: " << booking.getId().toString() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate booking: " << booking.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Бронирования: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << bookings.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating bookings: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateReviews() {
    try {
        auto sourceRepo = sourceFactory_->createReviewRepository();
        auto targetRepo = targetFactory_->createReviewRepository();
        
        auto reviews = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& review : reviews) {
            // Проверяем существование клиента и занятия
            auto clientRepo = targetFactory_->createClientRepository();
            auto lessonRepo = targetFactory_->createLessonRepository();
            
            if (!clientRepo->exists(review.getClientId())) {
                std::cerr << "❌ Referenced client not found: " << review.getClientId().toString() 
                         << " for review" << std::endl;
                return false;
            }
            
            if (!lessonRepo->exists(review.getLessonId())) {
                std::cerr << "❌ Referenced lesson not found: " << review.getLessonId().toString() 
                         << " for review" << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(review.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(review)) {
                        updatedCount++;
                        std::cout << "✅ Обновлен отзыв: " << review.getId().toString() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update review: " << review.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущен существующий отзыв: " << review.getId().toString() << std::endl;
                }
            } else {
                if (targetRepo->save(review)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирован отзыв: " << review.getId().toString() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate review: " << review.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Отзывы: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << reviews.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating reviews: " << e.what() << std::endl;
        return false;
    }
}

bool DataMigrator::migrateAttendance() {
    try {
        auto sourceRepo = sourceFactory_->createAttendanceRepository();
        auto targetRepo = targetFactory_->createAttendanceRepository();
        
        auto attendances = sourceRepo->findAll();
        int migratedCount = 0;
        int updatedCount = 0;
        int skippedCount = 0;
        
        for (const auto& attendance : attendances) {
            // Проверяем существование клиента
            auto clientRepo = targetFactory_->createClientRepository();
            if (!clientRepo->exists(attendance.getClientId())) {
                std::cerr << "❌ Referenced client not found: " << attendance.getClientId().toString() 
                         << " for attendance" << std::endl;
                return false;
            }
            
            bool exists = targetRepo->exists(attendance.getId());
            
            if (exists) {
                if (migrationStrategy_ == "overwrite") {
                    if (targetRepo->update(attendance)) {
                        updatedCount++;
                        std::cout << "✅ Обновлена запись посещаемости: " << attendance.getId().toString() << std::endl;
                    } else {
                        std::cerr << "❌ Failed to update attendance: " << attendance.getId().toString() << std::endl;
                        return false;
                    }
                } else {
                    skippedCount++;
                    //std::cout << "⚠️  Пропущена существующая запись посещаемости: " << attendance.getId().toString() << std::endl;
                }
            } else {
                if (targetRepo->save(attendance)) {
                    migratedCount++;
                    std::cout << "✅ Мигрирована запись посещаемости: " << attendance.getId().toString() << std::endl;
                } else {
                    std::cerr << "❌ Failed to migrate attendance: " << attendance.getId().toString() << std::endl;
                    return false;
                }
            }
        }
        
        std::cout << "✅ Посещаемость: мигрировано " << migratedCount << ", обновлено " << updatedCount 
                  << ", пропущено " << skippedCount << "/" << attendances.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Error migrating attendance: " << e.what() << std::endl;
        return false;
    }
}