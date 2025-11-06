#include "AttendanceService.hpp"
#include <iostream>

AttendanceService::AttendanceService(
    std::shared_ptr<IAttendanceRepository> attendanceRepo,
    std::shared_ptr<IBookingRepository> bookingRepo,
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo,
    std::shared_ptr<ILessonRepository> lessonRepo
) : attendanceRepo_(std::move(attendanceRepo)),
    bookingRepo_(std::move(bookingRepo)),
    enrollmentRepo_(std::move(enrollmentRepo)),
    lessonRepo_(std::move(lessonRepo)) {}

bool AttendanceService::createAttendanceForBooking(const UUID& bookingId, BookingStatus newStatus, const std::string& notes) {
    try {
        auto booking = bookingRepo_->findById(bookingId);
        if (!booking) {
            std::cerr << "❌ Бронирование не найдено: " << bookingId.toString() << std::endl;
            return false;
        }

        // Проверяем, нужно ли создавать запись посещаемости
        if (!shouldCreateAttendance(booking->getStatus(), newStatus)) {
            return true; // Не нужно создавать запись
        }

        // Ищем существующую запись
        auto existingAttendance = findExistingAttendance(bookingId, AttendanceType::BOOKING);
        
        if (existingAttendance) {
            // Обновляем существующую запись
            switch (newStatus) {
                case BookingStatus::COMPLETED:
                    existingAttendance->markVisited(notes);
                    break;
                case BookingStatus::CANCELLED:
                    existingAttendance->markCancelled(notes);
                    break;
                default:
                    std::cerr << "❌ Неподдерживаемый статус бронирования для посещаемости: " << static_cast<int>(newStatus) << std::endl;
                    return false;
            }
            return attendanceRepo_->update(*existingAttendance);
        } else {
            // Создаем новую запись
            UUID attendanceId = UUID::generate();
            Attendance attendance(attendanceId, booking->getClientId(), bookingId, 
                                AttendanceType::BOOKING, booking->getTimeSlot().getStartTime());
            
            switch (newStatus) {
                case BookingStatus::COMPLETED:
                    attendance.markVisited(notes);
                    break;
                case BookingStatus::CANCELLED:
                    attendance.markCancelled(notes);
                    break;
                default:
                    std::cerr << "❌ Неподдерживаемый статус бронирования для посещаемости: " << static_cast<int>(newStatus) << std::endl;
                    return false;
            }
            
            return attendanceRepo_->save(attendance);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка создания посещаемости для бронирования: " << e.what() << std::endl;
        return false;
    }
}

bool AttendanceService::createAttendanceForEnrollment(const UUID& enrollmentId, EnrollmentStatus newStatus, const std::string& notes) {
    try {
        auto enrollment = enrollmentRepo_->findById(enrollmentId);
        if (!enrollment) {
            std::cerr << "❌ Запись на занятие не найдена: " << enrollmentId.toString() << std::endl;
            return false;
        }

        auto lesson = lessonRepo_->findById(enrollment->getLessonId());
        if (!lesson) {
            std::cerr << "❌ Занятие не найдено для записи: " << enrollmentId.toString() << std::endl;
            return false;
        }

        if (!shouldCreateAttendance(enrollment->getStatus(), newStatus)) {
            return true;
        }

        auto existingAttendance = findExistingAttendance(enrollment->getLessonId(), AttendanceType::LESSON);
        
        if (existingAttendance && existingAttendance->getClientId() == enrollment->getClientId()) {
            // Обновляем существующую запись для этого клиента
            switch (newStatus) {
                case EnrollmentStatus::ATTENDED:
                    existingAttendance->markVisited(notes);
                    break;
                case EnrollmentStatus::CANCELLED:
                    existingAttendance->markCancelled(notes);
                    break;
                case EnrollmentStatus::MISSED:
                    existingAttendance->markNoShow(notes);
                    break;
                default:
                    std::cerr << "❌ Неподдерживаемый статус записи для посещаемости: " << static_cast<int>(newStatus) << std::endl;
                    return false;
            }
            return attendanceRepo_->update(*existingAttendance);
        } else {
            // Создаем новую запись
            UUID attendanceId = UUID::generate();
            Attendance attendance(attendanceId, enrollment->getClientId(), enrollment->getLessonId(),
                                AttendanceType::LESSON, lesson->getStartTime());
            
            switch (newStatus) {
                case EnrollmentStatus::ATTENDED:
                    attendance.markVisited(notes);
                    break;
                case EnrollmentStatus::CANCELLED:
                    attendance.markCancelled(notes);
                    break;
                case EnrollmentStatus::MISSED:
                    attendance.markNoShow(notes);
                    break;
                default:
                    std::cerr << "❌ Неподдерживаемый статус записи для посещаемости: " << static_cast<int>(newStatus) << std::endl;
                    return false;
            }
            
            return attendanceRepo_->save(attendance);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка создания посещаемости для записи на занятие: " << e.what() << std::endl;
        return false;
    }
}

bool AttendanceService::markBookingVisited(const UUID& bookingId, const std::string& notes) {
    return createAttendanceForBooking(bookingId, BookingStatus::COMPLETED, notes);
}

bool AttendanceService::markBookingCancelled(const UUID& bookingId, const std::string& notes) {
    return createAttendanceForBooking(bookingId, BookingStatus::CANCELLED, notes);
}

bool AttendanceService::markBookingNoShow(const UUID& bookingId, const std::string& notes) {
    // Для бронирований "не явился" тоже считается как отмена
    return createAttendanceForBooking(bookingId, BookingStatus::CANCELLED, notes + " (не явился)");
}

bool AttendanceService::markLessonVisited(const UUID& enrollmentId, const std::string& notes) {
    return createAttendanceForEnrollment(enrollmentId, EnrollmentStatus::ATTENDED, notes);
}

bool AttendanceService::markLessonCancelled(const UUID& enrollmentId, const std::string& notes) {
    return createAttendanceForEnrollment(enrollmentId, EnrollmentStatus::CANCELLED, notes);
}

bool AttendanceService::markLessonNoShow(const UUID& enrollmentId, const std::string& notes) {
    return createAttendanceForEnrollment(enrollmentId, EnrollmentStatus::MISSED, notes);
}

bool AttendanceService::shouldCreateAttendance(BookingStatus oldStatus, BookingStatus newStatus) {
    // Создаем запись только при переходе в финальные статусы
    bool shouldCreate = (oldStatus != newStatus) && 
                       (newStatus == BookingStatus::COMPLETED || newStatus == BookingStatus::CANCELLED);
    
    if (shouldCreate) {
        std::cout << "📝 Создание посещаемости для бронирования: " 
                  << static_cast<int>(oldStatus) << " -> " << static_cast<int>(newStatus) << std::endl;
    }
    
    return shouldCreate;
}

bool AttendanceService::shouldCreateAttendance(EnrollmentStatus oldStatus, EnrollmentStatus newStatus) {
    // Создаем запись только при переходе в финальные статусы
    bool shouldCreate = (oldStatus != newStatus) && 
                       (newStatus == EnrollmentStatus::ATTENDED || 
                        newStatus == EnrollmentStatus::CANCELLED || 
                        newStatus == EnrollmentStatus::MISSED);
    
    if (shouldCreate) {
        std::cout << "📝 Создание посещаемости для записи: " 
                  << static_cast<int>(oldStatus) << " -> " << static_cast<int>(newStatus) << std::endl;
    }
    
    return shouldCreate;
}

std::optional<Attendance> AttendanceService::findExistingAttendance(const UUID& entityId, AttendanceType type) {
    try {
        auto attendances = attendanceRepo_->findByEntityId(entityId);
        for (const auto& attendance : attendances) {
            if (attendance.getType() == type) {
                return attendance;
            }
        }
        return std::nullopt;
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка поиска существующей записи посещаемости: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::vector<Attendance> AttendanceService::getClientAttendance(const UUID& clientId, 
                                                              const std::chrono::system_clock::time_point& startDate,
                                                              const std::chrono::system_clock::time_point& endDate) const {
    try {
        return attendanceRepo_->findByClientAndPeriod(clientId, startDate, endDate);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения посещаемости клиента: " << e.what() << std::endl;
        return {};
    }
}

std::vector<Attendance> AttendanceService::getAttendanceByTypeAndStatus(AttendanceType type, AttendanceStatus status) const {
    try {
        return attendanceRepo_->findByTypeAndStatus(type, status);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения посещаемости по типу и статусу: " << e.what() << std::endl;
        return {};
    }
}

bool AttendanceService::updateAttendanceNotes(const UUID& attendanceId, const std::string& notes) {
    try {
        auto attendance = attendanceRepo_->findById(attendanceId);
        if (!attendance) {
            std::cerr << "❌ Запись посещаемости не найдена: " << attendanceId.toString() << std::endl;
            return false;
        }
        
        if (!Attendance::isValidNotes(notes)) {
            std::cerr << "❌ Неверный формат заметок" << std::endl;
            return false;
        }
        
        attendance->setNotes(notes);
        return attendanceRepo_->update(*attendance);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка обновления заметок посещаемости: " << e.what() << std::endl;
        return false;
    }
}