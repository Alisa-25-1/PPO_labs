#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "../../types/uuid.hpp"
#include "../../dtos/EnrollmentDTO.hpp"

class WebApplication;

class MyEnrollmentsWidget : public Wt::WContainerWidget {
public:
    MyEnrollmentsWidget(WebApplication* app);
    void loadEnrollments();
    
private:
    WebApplication* app_;
    Wt::WTable* enrollmentsTable_;
    Wt::WText* statusText_;

    void setupUI();
    void handleCancelEnrollment(const UUID& enrollmentId);
    void updateStatus(const std::string& message, bool isError = false);
};