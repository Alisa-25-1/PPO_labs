#ifndef TECHUI_HPP
#define TECHUI_HPP

#include <memory>
#include <iostream>
#include <vector>
#include "TechUIManagers.hpp"
#include "InputHandlers.hpp"
#include "../types/uuid.hpp"
#include "../models/Client.hpp"
#include "../models/DanceHall.hpp"
#include "../models/Booking.hpp"
#include "../models/Lesson.hpp"
#include "../models/Subscription.hpp"
#include "../models/Review.hpp"
#include "../models/Trainer.hpp"
#include "../dtos/AuthDTO.hpp"
#include "../dtos/BookingDTO.hpp"
#include "../dtos/LessonDTO.hpp"
#include "../dtos/SubscriptionDTO.hpp"
#include "../dtos/ReviewDTO.hpp"

class TechUI {
private:
    std::unique_ptr<TechUIManagers> managers_;
    UUID currentClientId_;

    // Основное меню
    void displayMainMenu();
    void handleClientMode();
    void handleAdminMode();
    
    // Режим клиента
    void handleClientAuth();
    void handleClientMenu();
    void handleClientBookings();
    void handleClientLessons();
    void handleClientSubscriptions();
    void handleClientReviews();
    
    // Конкретные операции клиента
    void registerClient();
    void loginClient();
    void createBooking();
    void viewClientBookings();
    void cancelBooking();
    void viewSchedule();
    void purchaseSubscription();
    void viewClientSubscriptions();
    void createReview();
    void viewClientReviews();
    void checkTimeSlotAvailability();
    
    // Режим администратора
    void handleAdminMenu();
    void handleAdminClients();
    void handleAdminHalls();
    void handleAdminBookings();
    void handleAdminLessons();
    void handleAdminSubscriptions();
    void handleAdminReviews();
    void handleAdminTrainers();
    
    // Административные операции
    void findClientById();
    void createHall();
    void listAllHalls();
    void findHallById();
    void viewHallBookings();
    void viewClientBookingsAdmin();
    void createLesson();
    void listAllLessons();
    void createTrainer();
    void listAllTrainers();
    void findTrainerById();
    void viewClientSubscriptionsAdmin();
    void viewSubscriptionTypes();
    void viewPendingReviews();
    void approveReview();
    void rejectReview();
    
    // Вспомогательные методы отображения
    void displayBooking(const BookingResponseDTO& booking);
    void displayLesson(const Lesson& lesson);
    void displaySubscription(const SubscriptionResponseDTO& subscription);
    void displayReview(const ReviewResponseDTO& review);
    void displayClient(const Client& client);
    void displayHall(const DanceHall& hall);
    void displayTrainer(const Trainer& trainer);

public:
    TechUI(const std::string& connectionString);
    
    void run();
    bool isClientLoggedIn() const { return !currentClientId_.isNull(); }
};

#endif // TECHUI_HPP