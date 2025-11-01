#include "BookingController.hpp"
#include "../../services/BookingService.hpp"
#include "../../repositories/impl/PostgreSQLBookingRepository.hpp"
#include "../../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../../repositories/impl/PostgreSQLDanceHallRepository.hpp"
#include "../../repositories/impl/PostgreSQLBranchRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include <iostream>

BookingController::BookingController(std::shared_ptr<BookingService> bookingService)
    : bookingService_(std::move(bookingService)) {
    std::cout << "✅ BookingController создан с реальным сервисом" << std::endl;
}

BookingResponseDTO BookingController::createBooking(const BookingRequestDTO& request) {
    try {
        std::cout << "📝 Создание бронирования для клиента: " << request.clientId.toString() << std::endl;
        
        if (!validateBookingRequest(request)) {
            throw ValidationException("Invalid booking request data");
        }
        
        return bookingService_->createBooking(request);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка создания бронирования: " << e.what() << std::endl;
        throw;
    }
}

BookingResponseDTO BookingController::cancelBooking(const UUID& bookingId, const UUID& clientId) {
    try {
        std::cout << "❌ Отмена бронирования: " << bookingId.toString() << " для клиента: " << clientId.toString() << std::endl;
        return bookingService_->cancelBooking(bookingId, clientId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка отмены бронирования: " << e.what() << std::endl;
        throw;
    }
}

std::vector<BookingResponseDTO> BookingController::getClientBookings(const UUID& clientId) {
    try {
        std::cout << "📋 Получение бронирований клиента: " << clientId.toString() << std::endl;
        return bookingService_->getClientBookings(clientId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения бронирований: " << e.what() << std::endl;
        return {};
    }
}

std::vector<DanceHall> BookingController::getAvailableHalls() {
    try {
        std::cout << "🏟️ Получение доступных залов" << std::endl;
        
        // Используем DanceHallRepository для получения всех залов
        // В реальной системе нужно добавить метод в BookingService
        // Пока используем напрямую репозиторий через сервис
        if (bookingService_) {
            return bookingService_->getAllHalls();
        }
        return {};
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения залов: " << e.what() << std::endl;
        return {};
    }
}

// Новый метод для получения доступных продолжительностей
std::vector<int> BookingController::getAvailableDurations(const UUID& hallId, 
                                                         const std::chrono::system_clock::time_point& startTime) {
    try {
        std::cout << "⏱️ Запрос доступных продолжительностей для зала: " << hallId.toString() 
                  << " в " << DateTimeUtils::formatTime(startTime) << std::endl;
        return bookingService_->getAvailableDurations(hallId, startTime);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения доступных продолжительностей: " << e.what() << std::endl;
        return {};
    }
}

std::vector<TimeSlot> BookingController::getAvailableTimeSlots(const UUID& hallId, 
                                                              const std::chrono::system_clock::time_point& date) {
    try {
        std::cout << "⏰ Получение доступных слотов для зала: " << hallId.toString() << std::endl;
        return bookingService_->getAvailableTimeSlots(hallId, date);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения доступных слотов: " << e.what() << std::endl;
        return {};
    }
}

bool BookingController::validateBookingRequest(const BookingRequestDTO& request) const {
    return request.validate();
}

bool BookingController::isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const {
    try {
        std::cout << "🔍 Проверка доступности слота для зала: " << hallId.toString() << std::endl;
        return bookingService_->isTimeSlotAvailable(hallId, timeSlot);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка проверки доступности слота: " << e.what() << std::endl;
        return false;
    }
}

std::string BookingController::getHallName(const UUID& hallId) {
    try {
        std::cout << "🏷️ Получение названия зала: " << hallId.toString() << std::endl;
        
        // Используем сервис для получения зала по ID
        if (bookingService_) {
            auto hall = bookingService_->getHallById(hallId);
            if (hall) {
                return hall->getName();
            }
        }
        
        return "Неизвестный зал";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения названия зала: " << e.what() << std::endl;
        return "Неизвестный зал";
    }
}

std::vector<Branch> BookingController::getBranches() {
    try {
        std::cout << "🏢 Получение списка филиалов" << std::endl;
        return bookingService_->getAllBranches();
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения филиалов: " << e.what() << std::endl;
        throw std::runtime_error("Failed to get branches: " + std::string(e.what()));
    }
}

std::vector<DanceHall> BookingController::getHallsByBranch(const UUID& branchId) {
    try {
        std::cout << "🏟️ Получение залов филиала: " << branchId.toString() << std::endl;
        return bookingService_->getHallsByBranch(branchId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения залов филиала: " << e.what() << std::endl;
        throw std::runtime_error("Failed to get halls by branch: " + std::string(e.what()));
    }
}

std::chrono::minutes BookingController::getTimezoneOffsetForHall(const UUID& hallId) {
    try {
        return bookingService_->getTimezoneOffsetForHall(hallId);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения часового пояса для зала: " << e.what() << std::endl;
        return std::chrono::hours(3);
    }
}