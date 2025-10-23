#include "BookingController.hpp"
#include "../../repositories/impl/PostgreSQLBookingRepository.hpp"
#include "../../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../../repositories/impl/PostgreSQLDanceHallRepository.hpp"
#include "../../repositories/impl/PostgreSQLBranchRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include <iostream>

BookingController::BookingController(std::shared_ptr<BookingService> bookingService)
    : bookingService_(std::move(bookingService)) {
}

BookingResponseDTO BookingController::createBooking(const BookingRequestDTO& request) {
    try {
        if (!validateBookingRequest(request)) {
            throw ValidationException("Invalid booking request data");
        }
        
        return bookingService_->createBooking(request);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка создания бронирования: " << e.what() << std::endl;
        throw;
    }
}

BookingResponseDTO BookingController::cancelBooking(const UUID& bookingId, const UUID& clientId) {
    try {
        return bookingService_->cancelBooking(bookingId, clientId);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка отмены бронирования: " << e.what() << std::endl;
        throw;
    }
}

std::vector<BookingResponseDTO> BookingController::getClientBookings(const UUID& clientId) {
    try {
        return bookingService_->getClientBookings(clientId);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения бронирований: " << e.what() << std::endl;
        return {};
    }
}

std::vector<DanceHall> BookingController::getAvailableHalls() {
    try {
        // Здесь нужно получить доступные залы через репозиторий
        // Пока вернем пустой список, реализуем позже
        return {};
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения залов: " << e.what() << std::endl;
        return {};
    }
}

std::vector<TimeSlot> BookingController::getAvailableTimeSlots(const UUID& hallId, 
                                                              const std::chrono::system_clock::time_point& date) {
    try {
        return bookingService_->getAvailableTimeSlots(hallId, date);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения доступных слотов: " << e.what() << std::endl;
        return {};
    }
}

bool BookingController::validateBookingRequest(const BookingRequestDTO& request) const {
    return request.validate();
}

bool BookingController::isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const {
    try {
        return bookingService_->isTimeSlotAvailable(hallId, timeSlot);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка проверки доступности слота: " << e.what() << std::endl;
        return false;
    }
}