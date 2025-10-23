#ifndef ADMINDASHBOARD_HPP
#define ADMINDASHBOARD_HPP

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenu.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WComboBox.h>
#include <Wt/WDatePicker.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include "../WebApplication.hpp"

class AdminDashboard : public Wt::WContainerWidget {
private:
    WebApplication* app_;
    Wt::WStackedWidget* contentStack_;
    Wt::WMenu* adminMenu_;

    // Views
    Wt::WContainerWidget* dashboardView_;
    Wt::WContainerWidget* clientsView_;
    Wt::WContainerWidget* bookingsView_;
    Wt::WContainerWidget* lessonsView_;
    Wt::WContainerWidget* hallsView_;
    Wt::WContainerWidget* trainersView_;
    Wt::WContainerWidget* subscriptionsView_;
    Wt::WContainerWidget* reviewsView_;

    // UI Elements
    Wt::WTable* clientsTable_;
    Wt::WTable* bookingsTable_;
    Wt::WTable* lessonsTable_;
    Wt::WTable* hallsTable_;
    Wt::WTable* trainersTable_;
    Wt::WTable* subscriptionsTable_;
    Wt::WTable* reviewsTable_;

    // Forms
    Wt::WLineEdit* clientSearchEdit_;
    Wt::WComboBox* bookingFilterComboBox_;
    Wt::WDatePicker* lessonDatePicker_;
    Wt::WLineEdit* hallNameEdit_;
    Wt::WLineEdit* trainerNameEdit_;
    Wt::WComboBox* subscriptionTypeComboBox_;
    Wt::WComboBox* reviewStatusComboBox_;

public:
    AdminDashboard(WebApplication* app);
    ~AdminDashboard() = default;

    // Navigation methods
    void showDashboard();
    void showClients();
    void showBookings();
    void showLessons();
    void showHalls();
    void showTrainers();
    void showSubscriptions();
    void showReviews();

    void refresh();

private:
    void setupUI();
    void setupAdminMenu();
    void createViews();
    
    // View initialization methods
    void setupDashboardView();
    void setupClientsView();
    void setupBookingsView();
    void setupLessonsView();
    void setupHallsView();
    void setupTrainersView();
    void setupSubscriptionsView();
    void setupReviewsView();
    
    // Data loading methods
    void loadClientsData();
    void loadBookingsData();
    void loadLessonsData();
    void loadHallsData();
    void loadTrainersData();
    void loadSubscriptionsData();
    void loadReviewsData();
    
    // Action handlers
    void handleClientSearch();
    void handleBookingFilter();
    void handleCreateLesson();
    void handleCreateHall();
    void handleCreateTrainer();
    void handleCreateSubscriptionType();
    void handleReviewModeration(const UUID& reviewId, bool approve);
    
    // Utility methods
    void setupTableHeaders(Wt::WTable* table, const std::vector<std::string>& headers);
    void showStatusMessage(const std::string& message, bool isError = false);
};

#endif // ADMINDASHBOARD_HPP