#include "ScheduleService.hpp"
#include <algorithm>

ScheduleService::ScheduleService(
    std::unique_ptr<ILessonRepository> lessonRepo,
    std::unique_ptr<IBookingRepository> bookingRepo,
    std::unique_ptr<IDanceHallRepository> hallRepo
) : lessonRepository_(std::move(lessonRepo)),
    bookingRepository_(std::move(bookingRepo)),
    hallRepository_(std::move(hallRepo)) {}

void ScheduleService::validateBranch(const UUID& branchId) const {
    // В реальной реализации здесь должна быть проверка существования филиала
    // Для упрощения предположим, что филиал существует
}

void ScheduleService::validateDateRange(const std::chrono::system_clock::time_point& startDate, 
                                       const std::chrono::system_clock::time_point& endDate) const {
    if (startDate >= endDate) {
        throw ValidationException("Start date must be before end date");
    }
}

ScheduleResponseDTO ScheduleService::getBranchSchedule(const UUID& branchId, 
                                                      const std::chrono::system_clock::time_point& startDate,
                                                      const std::chrono::system_clock::time_point& endDate) {
    validateBranch(branchId);
    validateDateRange(startDate, endDate);
    
    ScheduleResponseDTO response(branchId, startDate, endDate);
    
    // Получаем занятия для филиала в указанном диапазоне дат
    // Для упрощения предположим, что у нас есть метод для получения занятий по филиалу и дате
    // Здесь мы будем использовать существующие методы репозитория и фильтровать по залам филиала
    
    // Получаем все залы филиала
    auto halls = hallRepository_->findByBranchId(branchId);
    
    for (const auto& hall : halls) {
        // Получаем занятия для зала
        auto lessons = lessonRepository_->findByHallId(hall.getId());
        for (const auto& lesson : lessons) {
            auto lessonTime = lesson.getStartTime();
            if (lessonTime >= startDate && lessonTime <= endDate) {
                ScheduleSlotDTO slot(lesson.getTimeSlot(), lesson.getId(), "lesson", 
                                    lesson.getName(), lesson.getDescription(), 
                                    EnumUtils::lessonStatusToString(lesson.getStatus()));
                response.addSlot(slot);
            }
        }
        
        // Получаем бронирования для зала
        auto bookings = bookingRepository_->findByHallId(hall.getId());
        for (const auto& booking : bookings) {
            auto bookingTime = booking.getTimeSlot().getStartTime();
            if (bookingTime >= startDate && bookingTime <= endDate) {
                ScheduleSlotDTO slot(booking.getTimeSlot(), booking.getId(), "booking", 
                                    "Бронирование", booking.getPurpose(), 
                                    EnumUtils::bookingStatusToString(booking.getStatus()));
                response.addSlot(slot);
            }
        }
    }
    
    return response;
}

ScheduleResponseDTO ScheduleService::getHallSchedule(const UUID& hallId,
                                                    const std::chrono::system_clock::time_point& startDate,
                                                    const std::chrono::system_clock::time_point& endDate) {
    validateDateRange(startDate, endDate);
    
    if (!hallRepository_->exists(hallId)) {
        throw ValidationException("Hall not found");
    }
    
    ScheduleResponseDTO response(hallId, startDate, endDate);
    
    // Получаем занятия для зала
    auto lessons = lessonRepository_->findByHallId(hallId);
    for (const auto& lesson : lessons) {
        auto lessonTime = lesson.getStartTime();
        if (lessonTime >= startDate && lessonTime <= endDate) {
            ScheduleSlotDTO slot(lesson.getTimeSlot(), lesson.getId(), "lesson", 
                                lesson.getName(), lesson.getDescription(), 
                                EnumUtils::lessonStatusToString(lesson.getStatus()));
            response.addSlot(slot);
        }
    }
    
    // Получаем бронирования для зала
    auto bookings = bookingRepository_->findByHallId(hallId);
    for (const auto& booking : bookings) {
        auto bookingTime = booking.getTimeSlot().getStartTime();
        if (bookingTime >= startDate && bookingTime <= endDate) {
            ScheduleSlotDTO slot(booking.getTimeSlot(), booking.getId(), "booking", 
                                "Бронирование", booking.getPurpose(), 
                                EnumUtils::bookingStatusToString(booking.getStatus()));
            response.addSlot(slot);
        }
    }
    
    return response;
}

std::vector<TimeSlot> ScheduleService::getAvailableTimeSlots(const UUID& hallId,
                                                            const std::chrono::system_clock::time_point& date) {
    if (!hallRepository_->exists(hallId)) {
        throw ValidationException("Hall not found");
    }
    
    // Получаем все занятия и бронирования для зала на указанную дату
    auto startOfDay = date;
    auto endOfDay = date + std::chrono::hours(24);
    
    std::vector<TimeSlot> busySlots;
    
    // Занятия
    auto lessons = lessonRepository_->findByHallId(hallId);
    for (const auto& lesson : lessons) {
        auto lessonTime = lesson.getStartTime();
        if (lessonTime >= startOfDay && lessonTime < endOfDay) {
            busySlots.push_back(lesson.getTimeSlot());
        }
    }
    
    // Бронирования
    auto bookings = bookingRepository_->findByHallId(hallId);
    for (const auto& booking : bookings) {
        auto bookingTime = booking.getTimeSlot().getStartTime();
        if (bookingTime >= startOfDay && bookingTime < endOfDay) {
            busySlots.push_back(booking.getTimeSlot());
        }
    }
    
    // Генерируем доступные слоты (каждый час с 9:00 до 22:00)
    std::vector<TimeSlot> availableSlots;
    auto currentTime = std::chrono::system_clock::to_time_t(date);
    std::tm* tm = std::localtime(&currentTime);
    
    for (int hour = 9; hour < 22; hour++) {
        tm->tm_hour = hour;
        tm->tm_min = 0;
        auto slotStart = std::chrono::system_clock::from_time_t(std::mktime(tm));
        TimeSlot slot(slotStart, 60); // Слоты по 1 часу
        
        // Проверяем, не пересекается ли слот с занятыми
        bool isAvailable = true;
        for (const auto& busySlot : busySlots) {
            if (slot.overlapsWith(busySlot)) {
                isAvailable = false;
                break;
            }
        }
        
        if (isAvailable) {
            availableSlots.push_back(slot);
        }
    }
    
    return availableSlots;
}

bool ScheduleService::isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const {
    if (!hallRepository_->exists(hallId)) {
        return false;
    }
    
    // Проверяем занятия
    auto lessons = lessonRepository_->findByHallId(hallId);
    for (const auto& lesson : lessons) {
        if (lesson.getTimeSlot().overlapsWith(timeSlot)) {
            return false;
        }
    }
    
    // Проверяем бронирования
    auto bookings = bookingRepository_->findByHallId(hallId);
    for (const auto& booking : bookings) {
        if (booking.getTimeSlot().overlapsWith(timeSlot)) {
            return false;
        }
    }
    
    return true;
}