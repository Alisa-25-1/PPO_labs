#pragma once

#include <Wt/WApplication.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WContainerWidget.h>
#include "controllers/AuthController.hpp"
#include "controllers/BookingController.hpp"
#include "controllers/SubscriptionController.hpp"
#include "controllers/LessonController.hpp"
#include "models/UserSession.hpp"

class LoginWidget;
class ClientDashboard;
class RegistrationWidget;
class BookingView;
class PurchaseSubscriptionWidget;
class SubscriptionListWidget;
class SubscriptionView;
class LessonView;

class WebApplication : public Wt::WApplication {
public:
    WebApplication(const Wt::WEnvironment& env);
    
    void showLogin();
    void showDashboard();
    void showRegistration();
    void showBookingView();  
    void showSubscriptionView();
    void showLessonView();
    
    AuthController* getAuthController() { return authController_.get(); }
    BookingController* getBookingController() { return bookingController_.get(); }
    SubscriptionController* getSubscriptionController() const { return subscriptionController_.get(); }
    LessonController* getLessonController() { return lessonController_.get(); }
    
    // Методы для работы с сессией
    UserSession& getUserSession() { return userSession_; }
    bool isUserLoggedIn() const { return userSession_.isAuthenticated(); }
    UUID getCurrentClientId() const { return userSession_.getUserId(); }
    void loginUser(const AuthResponseDTO& authResponse);
    void logoutUser();

private:
    Wt::WStackedWidget* mainStack_;
    LoginWidget* loginView_;
    ClientDashboard* dashboardView_;
    RegistrationWidget* registrationView_;
    BookingView* bookingView_;
    SubscriptionView* subscriptionView_;
    LessonView* lessonView_;
    
    std::unique_ptr<AuthController> authController_;
    std::unique_ptr<BookingController> bookingController_;
    std::unique_ptr<SubscriptionController> subscriptionController_;
    std::unique_ptr<LessonController> lessonController_;
    UserSession userSession_;
    
    void setupStyles();
    void initializeControllers();
    void handleDatabaseError(const std::string& context); 
};