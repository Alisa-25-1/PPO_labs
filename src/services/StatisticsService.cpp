#include "StatisticsService.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

StatisticsService::StatisticsService(
    std::shared_ptr<IAttendanceRepository> attendanceRepo,
    std::shared_ptr<IClientRepository> clientRepo,
    std::shared_ptr<ILessonRepository> lessonRepo,
    std::shared_ptr<IBookingRepository> bookingRepo,
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo
) : attendanceRepo_(std::move(attendanceRepo)),
    clientRepo_(std::move(clientRepo)),
    lessonRepo_(std::move(lessonRepo)),
    bookingRepo_(std::move(bookingRepo)),
    enrollmentRepo_(std::move(enrollmentRepo)) {}

StudioStatsDTO StatisticsService::getStudioStats() {
    StudioStatsDTO stats{};
    
    try {
        // Получаем общую статистику
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
        
        // Топ клиентов
        auto topClients = attendanceRepo_->getTopClientsByVisits(5);
        for (const auto& [clientId, visits] : topClients) {
            auto client = clientRepo_->findById(clientId);
            if (client) {
                stats.topClients.emplace_back(client->getName(), visits);
            }
        }
        
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
        // Для бронировний используем другой подход, так как они относятся к другому типу
        auto clientAttendances = attendanceRepo_->findByClientId(clientId);
        for (const auto& attendance : clientAttendances) {
            if (attendance.getType() == AttendanceType::BOOKING) {
                stats.totalBookings++;
                if (attendance.isVisited()) stats.visitedBookings++;
                if (attendance.isCancelled()) stats.cancelledBookings++;
                if (attendance.isNoShow()) stats.noShowBookings++;
            }
        }
        
        // Рейтинг посещаемости
        stats.attendanceRate = calculateAttendanceRate(stats.visitedLessons + stats.visitedBookings, 
                                                     stats.totalLessons + stats.totalBookings);
        
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
    
    // Здесь можно реализовать получение статистики по месяцам
    // Пока возвращаем заглушку
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
        
        for (const auto& booking : allBookings) {
            // Проверяем, существует ли уже запись посещаемости
            auto existingAttendances = attendanceRepo_->findByEntityId(booking.getId());
            bool exists = false;
            
            for (const auto& att : existingAttendances) {
                if (att.getClientId() == booking.getClientId() && 
                    att.getType() == AttendanceType::BOOKING) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                // Создаем новую запись посещаемости
                UUID attendanceId = UUID::generate();
                Attendance attendance(attendanceId, booking.getClientId(), booking.getId(),
                                    AttendanceType::BOOKING, booking.getTimeSlot().getStartTime());
                
                // Устанавливаем начальный статус на основе статуса бронирования
                if (booking.isCancelled()) {
                    attendance.markCancelled("Миграция: исторические данные");
                } else if (booking.isCompleted()) {
                    attendance.markVisited("Миграция: исторические данные");
                }
                // Активные бронирования остаются SCHEDULED по умолчанию
                
                attendanceRepo_->save(attendance);
                migrated++;
            }
        }
        
        std::cout << "📊 Мигрировано бронирований: " << migrated << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка миграции бронирований: " << e.what() << std::endl;
        return false;
    }
}

bool StatisticsService::migrateEnrollmentsToAttendance() {
    try {
        auto allEnrollments = enrollmentRepo_->findAll();
        int migrated = 0;
        
        for (const auto& enrollment : allEnrollments) {
            // Получаем информацию о занятии для времени
            auto lesson = lessonRepo_->findById(enrollment.getLessonId());
            if (!lesson) continue;
            
            // Проверяем существование записи
            auto existingAttendances = attendanceRepo_->findByEntityId(enrollment.getLessonId());
            bool exists = false;
            
            for (const auto& att : existingAttendances) {
                if (att.getClientId() == enrollment.getClientId() && 
                    att.getType() == AttendanceType::LESSON) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                UUID attendanceId = UUID::generate();
                Attendance attendance(attendanceId, enrollment.getClientId(), enrollment.getLessonId(),
                                    AttendanceType::LESSON, lesson->getStartTime());
                
                // Устанавливаем статус на основе enrollment
                switch (enrollment.getStatus()) {
                    case EnrollmentStatus::CANCELLED:
                        attendance.markCancelled("Миграция: исторические данные");
                        break;
                    case EnrollmentStatus::ATTENDED:
                        attendance.markVisited("Миграция: исторические данные");
                        break;
                    case EnrollmentStatus::MISSED:
                        attendance.markNoShow("Миграция: исторические данные");
                        break;
                    case EnrollmentStatus::REGISTERED:
                        // Будущие занятия остаются SCHEDULED
                        break;
                }
                
                attendanceRepo_->save(attendance);
                migrated++;
            }
        }
        
        std::cout << "📊 Мигрировано записей на занятия: " << migrated << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка миграции записей: " << e.what() << std::endl;
        return false;
    }
}

double StatisticsService::calculateAttendanceRate(int visited, int total) const {
    if (total == 0) return 0.0;
    return (static_cast<double>(visited) / total) * 100.0;
}