#ifndef ENROLLMENTDTO_HPP
#define ENROLLMENTDTO_HPP

#include "../types/uuid.hpp"
#include "../models/Enrollment.hpp"
#include <string>
#include <iomanip>

struct EnrollmentRequestDTO {
    UUID clientId;
    UUID lessonId;
    
    bool validate() const;
};

struct EnrollmentResponseDTO {
    UUID enrollmentId;
    UUID clientId;
    UUID lessonId;
    std::string status;
    std::string enrollmentDate;
    std::string clientName;
    std::string lessonName; 
    
    EnrollmentResponseDTO(const Enrollment& enrollment) {
        enrollmentId = enrollment.getId();
        clientId = enrollment.getClientId();
        lessonId = enrollment.getLessonId();
        
        switch (enrollment.getStatus()) {
            case EnrollmentStatus::REGISTERED:
                status = "REGISTERED";
                break;
            case EnrollmentStatus::CANCELLED:
                status = "CANCELLED";
                break;
            case EnrollmentStatus::ATTENDED:
                status = "ATTENDED";
                break;
            case EnrollmentStatus::MISSED:
                status = "MISSED";
                break;
            default:
                status = "UNKNOWN";
                break;
        }
        
        auto time_t = std::chrono::system_clock::to_time_t(enrollment.getEnrollmentDate());
        std::tm tm = *std::localtime(&time_t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        enrollmentDate = oss.str();
    }
};

#endif // ENROLLMENTDTO_HPP