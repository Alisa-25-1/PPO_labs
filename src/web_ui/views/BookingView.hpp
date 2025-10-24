#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>

// Forward declarations
class WebApplication;
class BookingCreateWidget;
class BookingListWidget;

class BookingView : public Wt::WContainerWidget {
public:
    BookingView(WebApplication* app);
    
private:
    WebApplication* app_;
    Wt::WStackedWidget* contentStack_;
    
    // Виджеты для разных состояний
    Wt::WContainerWidget* menuWidget_;
    BookingCreateWidget* createWidget_;
    BookingListWidget* listWidget_;

    void setupUI();
    void showBookingMenu();
    void showCreateBooking();
    void showMyBookings();
};