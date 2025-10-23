#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WDateEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

class WebApplication;

class BookingCreateWidget : public Wt::WContainerWidget {
public:
    BookingCreateWidget(WebApplication* app);
    
private:
    WebApplication* app_;
    
    Wt::WComboBox* hallComboBox_;
    Wt::WDateEdit* dateEdit_;
    Wt::WComboBox* timeComboBox_;
    Wt::WComboBox* durationComboBox_;
    Wt::WLineEdit* purposeEdit_;
    Wt::WPushButton* createButton_;
    Wt::WPushButton* backButton_;
    Wt::WText* statusText_;

    void setupUI();
    void handleCreate();
    void handleBack();
    void updateStatus(const std::string& message, bool isError = false);
    void loadAvailableHalls();
    void loadTimeSlots();
};