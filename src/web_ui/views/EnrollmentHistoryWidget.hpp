#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include "../../types/uuid.hpp"

#include "../../data/DateTimeUtils.hpp"

class WebApplication;

class EnrollmentHistoryWidget : public Wt::WContainerWidget {
public:
    EnrollmentHistoryWidget(WebApplication* app);
    void loadHistory();
    
private:
    WebApplication* app_;
    Wt::WTable* historyTable_;
    Wt::WText* statusText_;

    void setupUI();
    void updateStatus(const std::string& message, bool isError = false);
};