#pragma once
#include "../repositories/IAttendanceRepository.hpp"
#include "../repositories/IClientRepository.hpp"
#include "../repositories/ILessonRepository.hpp"
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/IEnrollmentRepository.hpp"
#include "AttendanceService.hpp"
#include "../types/uuid.hpp"
#include "../models/Enrollment.hpp" 
#include <memory>
#include <vector>
#include <map>
#include <iostream>

struct ClientStatsDTO {
    UUID clientId;
    std::string clientName;
    int totalLessons;
    int visitedLessons;
    int cancelledLessons;
    int noShowLessons;
    int totalBookings;
    int visitedBookings;
    int cancelledBookings;
    int noShowBookings;
    double attendanceRate;
};

struct StudioStatsDTO {
    int totalClients;
    int totalLessons;
    int totalBookings;
    int visitedLessons;
    int cancelledLessons;
    int noShowLessons;
    int visitedBookings;
    int cancelledBookings;
    int noShowBookings;
    double overallAttendanceRate;
    std::vector<std::pair<std::string, int>> topClients;
};

class StatisticsService {
private:
    std::shared_ptr<IAttendanceRepository> attendanceRepo_;
    std::shared_ptr<IClientRepository> clientRepo_;
    std::shared_ptr<ILessonRepository> lessonRepo_;
    std::shared_ptr<IBookingRepository> bookingRepo_;
    std::shared_ptr<IEnrollmentRepository> enrollmentRepo_;
    std::shared_ptr<AttendanceService> attendanceService_;

public:
    StatisticsService(
        std::shared_ptr<IAttendanceRepository> attendanceRepo,
        std::shared_ptr<IClientRepository> clientRepo,
        std::shared_ptr<ILessonRepository> lessonRepo,
        std::shared_ptr<IBookingRepository> bookingRepo,
        std::shared_ptr<IEnrollmentRepository> enrollmentRepo,
        std::shared_ptr<AttendanceService> attendanceService
    );

    StudioStatsDTO getStudioStats();
    ClientStatsDTO getClientStats(const UUID& clientId);
    std::vector<ClientStatsDTO> getAllClientsStats();
    std::map<std::string, int> getMonthlyStats(int year, int month);
    bool migrateExistingData();
    
private:
    double calculateAttendanceRate(int visited, int total) const;
    bool migrateBookingsToAttendance();
    bool migrateEnrollmentsToAttendance();
    std::string attendanceStatusToString(AttendanceStatus status);
};