#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WComboBox.h>
#include "WebApplication.hpp"

class LessonView : public Wt::WContainerWidget {
private:
    WebApplication* app_;
    
    Wt::WComboBox* filterComboBox_;
    Wt::WTable* lessonsTable_;
    Wt::WTable* enrollmentsTable_;
    Wt::WText* statusText_;

public:
    LessonView(WebApplication* app);
    
private:
    void setupUI();
    void loadLessons();
    void loadEnrollments();
    void setupLessonsTable();
    void setupEnrollmentsTable();
    void handleEnroll(const UUID& lessonId);
    void handleCancelEnrollment(const UUID& enrollmentId);
    void handleFilterChange();
};