#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

class WebApplication;

class BookingListWidget : public Wt::WContainerWidget {
public:
    BookingListWidget(WebApplication* app);
    
private:
    WebApplication* app_;
    Wt::WTable* bookingsTable_;
    Wt::WText* statusText_;

    void setupUI();
    void loadBookings();
    void handleCancelBooking();
    void updateStatus(const std::string& message, bool isError = false);
};