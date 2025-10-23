#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WComboBox.h>
#include <Wt/WDatePicker.h>
#include <Wt/WPushButton.h> 
#include "../WebApplication.hpp"

class ScheduleView : public Wt::WContainerWidget {
private:
    WebApplication* app_;
    
    Wt::WComboBox* branchComboBox_;
    Wt::WDatePicker* datePicker_;
    Wt::WTable* scheduleTable_;
    Wt::WText* statusText_;

public:
    ScheduleView(WebApplication* app);
    
private:
    void setupUI();
    void loadSchedule();
    void setupScheduleTable();
    void handleDateChange();
    void handleBranchChange();
    void displayScheduleForDate(const std::chrono::system_clock::time_point& date);
};