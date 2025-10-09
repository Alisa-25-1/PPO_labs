#ifndef TECHUI_HPP
#define TECHUI_HPP

#include <memory>
#include <iostream>
#include "../services/BookingService.hpp"
#include "../repositories/impl/PostgreSQLClientRepository.hpp"
#include "../repositories/impl/PostgreSQLHallRepository.hpp"
#include "../repositories/impl/PostgreSQLBookingRepository.hpp"
#include "../data/DatabaseConnection.hpp"

class TechUI {
private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    std::unique_ptr<BookingService> bookingService_;
    
    // Вспомогательные методы
    void displayMainMenu();
    void handleClientManagement();
    void handleHallManagement();
    void handleBookingManagement();
    void handleSubscriptionManagement();
    void handleLessonManagement();
    
    // Методы для работы с клиентами
    void createClient();
    void listClients();
    void findClientById();
    
    // Методы для работы с залами
    void createHall();
    void listHalls();
    void findHallById();
    
    // Методы для работы с бронированиями
    void createBooking();
    void listBookings();
    void cancelBooking();
    void checkTimeSlotAvailability();
    
    // Методы для работы с занятиями
    void createLesson();
    void listLessons();
    
    // Утилиты
    UUID readUUID(const std::string& prompt);
    std::string readString(const std::string& prompt);
    int readInt(const std::string& prompt);
    TimeSlot readTimeSlot();

public:
    TechUI(const std::string& connectionString);
    ~TechUI() = default;
    
    void run();
};

#endif // TECHUI_HPP