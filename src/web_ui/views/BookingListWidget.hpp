#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "../../types/uuid.hpp"
#include "../../dtos/BookingDTO.hpp"
#include "../../data/DateTimeUtils.hpp"

class WebApplication;

class BookingListWidget : public Wt::WContainerWidget {
public:
    BookingListWidget(WebApplication* app);
    void loadBookings();
    
private:
    WebApplication* app_;
    Wt::WTable* bookingsTable_;
    Wt::WText* statusText_;

    void setupUI();
    void handleCancelBooking(const UUID& bookingId);
    void performCancelBooking(const UUID& bookingId);
    void updateStatus(const std::string& message, bool isError = false);
    
    // Вспомогательные методы для работы с бизнес-логикой
    std::vector<BookingResponseDTO> getClientBookingsFromService();
    bool cancelBookingThroughService(const UUID& bookingId);
    UUID getCurrentClientId();
    std::string getHallNameById(const UUID& hallId);
    std::string formatDateTime(const std::chrono::system_clock::time_point& timePoint, const UUID& hallId);
    std::string getStatusDisplayName(const std::string& status);
    std::string getStatusStyleClass(const std::string& status);
};