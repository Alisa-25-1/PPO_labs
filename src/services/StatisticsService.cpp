#include "StatisticsService.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

// Constructor 
StatisticsService::StatisticsService(
    std::shared_ptr<IAttendanceRepository> attendanceRepo,
    std::shared_ptr<IClientRepository> clientRepo,
    std::shared_ptr<ILessonRepository> lessonRepo,
    std::shared_ptr<IBookingRepository> bookingRepo,
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo,
    std::shared_ptr<AttendanceService> attendanceService  
) : attendanceRepo_(std::move(attendanceRepo)),
    clientRepo_(std::move(clientRepo)),
    lessonRepo_(std::move(lessonRepo)),
    bookingRepo_(std::move(bookingRepo)),
    enrollmentRepo_(std::move(enrollmentRepo)),
    attendanceService_(std::move(attendanceService)) {} 

StudioStatsDTO StatisticsService::getStudioStats() {
    StudioStatsDTO stats{};
    
    try {
        auto clients = clientRepo_->findAll();
        stats.totalClients = clients.size();
        
        // Статистика по занятиям
        stats.visitedLessons = attendanceRepo_->countByTypeAndStatus(AttendanceType::LESSON, AttendanceStatus::VISITED);
        stats.cancelledLessons = attendanceRepo_->countByTypeAndStatus(AttendanceType::LESSON, AttendanceStatus::CANCELLED);
        stats.noShowLessons = attendanceRepo_->countByTypeAndStatus(AttendanceType::LESSON, AttendanceStatus::NO_SHOW);
        stats.totalLessons = stats.visitedLessons + stats.cancelledLessons + stats.noShowLessons;
        
        // Статистика по бронированиям
        stats.visitedBookings = attendanceRepo_->countByTypeAndStatus(AttendanceType::BOOKING, AttendanceStatus::VISITED);
        stats.cancelledBookings = attendanceRepo_->countByTypeAndStatus(AttendanceType::BOOKING, AttendanceStatus::CANCELLED);
        stats.noShowBookings = attendanceRepo_->countByTypeAndStatus(AttendanceType::BOOKING, AttendanceStatus::NO_SHOW);
        stats.totalBookings = stats.visitedBookings + stats.cancelledBookings + stats.noShowBookings;
        
        // Общий рейтинг посещаемости
        int totalVisited = stats.visitedLessons + stats.visitedBookings;
        int totalScheduled = stats.totalLessons + stats.totalBookings;
        stats.overallAttendanceRate = calculateAttendanceRate(totalVisited, totalScheduled);
        
    } catch (const std::exception& e) {
        // Логируем ошибку, но возвращаем пустую статистику
        std::cerr << "Ошибка при получении статистики студии: " << e.what() << std::endl;
    }
    
    return stats;
}

ClientStatsDTO StatisticsService::getClientStats(const UUID& clientId) {
    ClientStatsDTO stats{};
    stats.clientId = clientId;
    
    try {
        auto client = clientRepo_->findById(clientId);
        if (!client) {
            throw std::runtime_error("Client not found");
        }
        stats.clientName = client->getName();
        
        // Статистика по занятиям
        stats.visitedLessons = attendanceRepo_->countByClientAndStatus(clientId, AttendanceStatus::VISITED);
        stats.cancelledLessons = attendanceRepo_->countByClientAndStatus(clientId, AttendanceStatus::CANCELLED);
        stats.noShowLessons = attendanceRepo_->countByClientAndStatus(clientId, AttendanceStatus::NO_SHOW);
        stats.totalLessons = stats.visitedLessons + stats.cancelledLessons + stats.noShowLessons;
        
        // Статистика по бронированиям
        auto clientAttendances = attendanceRepo_->findByClientId(clientId);
        for (const auto& attendance : clientAttendances) {
            if (attendance.getType() == AttendanceType::BOOKING) {
                stats.totalBookings++;
                if (attendance.isVisited()) stats.visitedBookings++;
                if (attendance.isCancelled()) stats.cancelledBookings++;
                if (attendance.isNoShow()) stats.noShowBookings++;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении статистики клиента: " << e.what() << std::endl;
    }
    
    return stats;
}

std::vector<ClientStatsDTO> StatisticsService::getAllClientsStats() {
    std::vector<ClientStatsDTO> result;
    
    try {
        auto clients = clientRepo_->findAll();
        for (const auto& client : clients) {
            auto stats = getClientStats(client.getId());
            result.push_back(stats);
        }
        
        // Сортируем по рейтингу посещаемости (по убыванию)
        std::sort(result.begin(), result.end(), 
                 [](const ClientStatsDTO& a, const ClientStatsDTO& b) {
                     return a.attendanceRate > b.attendanceRate;
                 });
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении статистики всех клиентов: " << e.what() << std::endl;
    }
    
    return result;
}

std::map<std::string, int> StatisticsService::getMonthlyStats(int year, int month) {
    std::map<std::string, int> monthlyStats;
    
    monthlyStats["Занятий проведено"] = 0;
    monthlyStats["Бронирований использовано"] = 0;
    monthlyStats["Новых клиентов"] = 0;
    monthlyStats["Отменено занятий"] = 0;
    
    return monthlyStats;
}

bool StatisticsService::migrateExistingData() {
    try {
        std::cout << "🔄 Начало миграции существующих данных..." << std::endl;
        
        bool success = true;
        success &= migrateBookingsToAttendance();
        success &= migrateEnrollmentsToAttendance();
        
        if (success) {
            std::cout << "✅ Миграция данных завершена успешно" << std::endl;
        } else {
            std::cerr << "❌ Ошибка при миграции данных" << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка миграции: " << e.what() << std::endl;
        return false;
    }
}

bool StatisticsService::migrateBookingsToAttendance() {
    try {
        auto allBookings = bookingRepo_->findAll();
        int migrated = 0;
        int skipped = 0;
        
        std::cout << "🔍 Найдено бронирований для миграции: " << allBookings.size() << std::endl;
        
        for (const auto& booking : allBookings) {
            try {
                if (booking.isCompleted() || booking.isCancelled()) {
                    AttendanceStatus attendanceStatus;
                    
                    if (booking.isCompleted()) {
                        attendanceStatus = AttendanceStatus::VISITED;
                    } else if (booking.isCancelled()) {
                        attendanceStatus = AttendanceStatus::CANCELLED;
                    } else {
                        skipped++;
                        continue;
                    }
                    
                    Attendance attendance(
                        UUID::generate(),
                        booking.getClientId(),
                        booking.getId(),
                        AttendanceType::BOOKING,
                        booking.getTimeSlot().getStartTime()
                    );
                    attendance.markVisited("Миграция: исторические данные");
                    
                    if (attendanceRepo_->save(attendance)) {
                        migrated++;
                        std::cout << "✅ Мигрировано бронирование: " << booking.getId().toString() 
                                  << " -> " << (attendanceStatus == AttendanceStatus::VISITED ? "VISITED" : "CANCELLED") 
                                  << std::endl;
                    } else {
                        std::cerr << "❌ Не удалось сохранить посещаемость для бронирования: " 
                                  << booking.getId().toString() << std::endl;
                    }
                } else {
                    skipped++;
                }
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при миграции бронирования " << booking.getId().toString() 
                          << ": " << e.what() << std::endl;
            }
        }
        
        std::cout << "📊 Мигрировано бронирований в посещаемость: " << migrated 
                  << ", пропущено: " << skipped << std::endl;
        return migrated > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка миграции бронирований: " << e.what() << std::endl;
        return false;
    }
}

bool StatisticsService::migrateEnrollmentsToAttendance() {
    try {
        auto allEnrollments = enrollmentRepo_->findAll();
        int migrated = 0;
        int skipped = 0;
        
        std::cout << "🔍 Найдено записей на занятия для миграции: " << allEnrollments.size() << std::endl;
        
        for (const auto& enrollment : allEnrollments) {
            try {
                if (enrollment.getStatus() != EnrollmentStatus::REGISTERED) {
                    AttendanceStatus attendanceStatus;
                    
                    switch (enrollment.getStatus()) {
                        case EnrollmentStatus::ATTENDED:
                            attendanceStatus = AttendanceStatus::VISITED;
                            break;
                        case EnrollmentStatus::CANCELLED:
                            attendanceStatus = AttendanceStatus::CANCELLED;
                            break;
                        case EnrollmentStatus::MISSED:
                            attendanceStatus = AttendanceStatus::NO_SHOW;
                            break;
                        default:
                            skipped++;
                            continue;
                    }
                    
                    // Получаем информацию о занятии для времени
                    auto lesson = lessonRepo_->findById(enrollment.getLessonId());
                    if (!lesson) {
                        std::cerr << "❌ Занятие не найдено для записи: " << enrollment.getId().toString() << std::endl;
                        skipped++;
                        continue;
                    }
                    
                    Attendance attendance(
                        UUID::generate(),
                        enrollment.getClientId(),
                        enrollment.getLessonId(),
                        AttendanceType::LESSON,
                        lesson->getStartTime()
                    );

                    switch (attendanceStatus) {
                        case AttendanceStatus::VISITED:
                            attendance.markVisited("Миграция: исторические данные");
                            break;
                        case AttendanceStatus::CANCELLED:
                            attendance.markCancelled("Миграция: исторические данные");
                            break;
                        case AttendanceStatus::NO_SHOW:
                            attendance.markNoShow("Миграция: исторические данные");
                            break;
                        default:
                            break;
                    }
                    
                    if (attendanceRepo_->save(attendance)) {
                        migrated++;
                        std::cout << "✅ Мигрирована запись на занятие: " << enrollment.getId().toString() 
                                  << " -> " << attendanceStatusToString(attendanceStatus) << std::endl;
                    } else {
                        std::cerr << "❌ Не удалось сохранить посещаемость для записи: " 
                                  << enrollment.getId().toString() << std::endl;
                    }
                } else {
                    skipped++;
                }
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при миграции записи " << enrollment.getId().toString() 
                          << ": " << e.what() << std::endl;
            }
        }
        
        std::cout << "📊 Мигрировано записей на занятия: " << migrated 
                  << ", пропущено: " << skipped << std::endl;
        return migrated > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка миграции записей: " << e.what() << std::endl;
        return false;
    }
}

std::string StatisticsService::attendanceStatusToString(AttendanceStatus status) {
    switch (status) {
        case AttendanceStatus::VISITED: return "VISITED";
        case AttendanceStatus::CANCELLED: return "CANCELLED";
        case AttendanceStatus::NO_SHOW: return "NO_SHOW";
        default: return "SCHEDULED";
    }
}

double StatisticsService::calculateAttendanceRate(int visited, int total) const {
    if (total == 0) return 0.0;
    return (static_cast<double>(visited) / total) * 100.0;
}