#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>

class WebApplication;
class LessonScheduleWidget;
class MyEnrollmentsWidget;
class EnrollmentHistoryWidget;

class LessonView : public Wt::WContainerWidget {
public:
    LessonView(WebApplication* app);
    
private:
    WebApplication* app_;
    Wt::WStackedWidget* contentStack_;
    
    // Виджеты для разных состояний
    Wt::WContainerWidget* menuWidget_;
    LessonScheduleWidget* scheduleWidget_;
    MyEnrollmentsWidget* myEnrollmentsWidget_;
    EnrollmentHistoryWidget* historyWidget_;

    void setupUI();
    void showLessonMenu();
    void showSchedule();
    void showMyEnrollments();
    void showEnrollmentHistory();
};