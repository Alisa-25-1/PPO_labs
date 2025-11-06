#ifndef ATTENDANCE_HPP
#define ATTENDANCE_HPP

#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include <chrono>
#include <string>

class Attendance {
private:
    UUID id_;
    UUID clientId_;
    UUID entityId_; // ID занятия или бронирования
    AttendanceType type_; // LESSON или BOOKING
    AttendanceStatus status_; // VISITED, CANCELLED, NO_SHOW
    std::chrono::system_clock::time_point scheduledTime_;
    std::chrono::system_clock::time_point actualTime_;
    std::string notes_;
    double amountPaid_; // Сумма оплаты (для статистики доходов)
    int durationMinutes_; // Фактическая продолжительность

public:
    Attendance();
    Attendance(const UUID& id, const UUID& clientId, const UUID& entityId, 
               AttendanceType type, const std::chrono::system_clock::time_point& scheduledTime);
    
    // Геттеры
    UUID getId() const;
    UUID getClientId() const;
    UUID getEntityId() const;
    AttendanceType getType() const;
    AttendanceStatus getStatus() const;
    std::chrono::system_clock::time_point getScheduledTime() const;
    std::chrono::system_clock::time_point getActualTime() const;
    std::string getNotes() const;
    double getAmountPaid() const;
    int getDurationMinutes() const;
    
    // Сеттеры
    void setAmountPaid(double amount);
    void setDurationMinutes(int minutes);
    void setNotes(const std::string& notes);
    
    // Бизнес-логика
    void markVisited(const std::string& notes = "");
    void markCancelled(const std::string& notes = "");
    void markNoShow(const std::string& notes = "");
    bool isVisited() const;
    bool isCancelled() const;
    bool isNoShow() const;
    bool isValid() const;
    
    // Финансовые методы
    bool isPaid() const;
    double calculateRevenue() const;
    
    // Валидация
    static bool isValidNotes(const std::string& notes);
    static bool isValidAmount(double amount);
    
    // Операторы сравнения
    bool operator==(const Attendance& other) const;
    bool operator!=(const Attendance& other) const;
};

#endif // ATTENDANCE_HPP