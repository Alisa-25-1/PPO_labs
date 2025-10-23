#pragma once

#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WContainerWidget.h>
#include "controllers/AuthController.hpp"
#include "controllers/BookingController.hpp"

class LoginWidget;
class ClientDashboard;
class RegistrationWidget;
class BookingView;

class WebApplication : public Wt::WApplication {
public:
    WebApplication(const Wt::WEnvironment& env);
    
    void showLogin();
    void showDashboard();
    void showRegistration();
    void showBookingView();  
    
    AuthController* getAuthController() { return authController_.get(); }
    BookingController* getBookingController() { return bookingController_.get(); }

private:
    Wt::WStackedWidget* mainStack_;
    LoginWidget* loginView_;
    ClientDashboard* dashboardView_;
    RegistrationWidget* registrationView_;
    BookingView* bookingView_;
    
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<BookingController> bookingController_;
    
    void setupStyles();
    void initializeControllers();
};