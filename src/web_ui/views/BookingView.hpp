#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

class WebApplication;

class BookingView : public Wt::WContainerWidget {
public:
    BookingView(WebApplication* app);
    
private:
    WebApplication* app_;
    Wt::WStackedWidget* contentStack_;
    
    void setupUI();
    void showBookingMenu();
    void showCreateBooking();
    void showMyBookings();
    void showCancelBooking();
};