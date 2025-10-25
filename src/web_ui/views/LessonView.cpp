#include "LessonView.hpp"
#include "../WebApplication.hpp"
#include "LessonScheduleWidget.hpp"
#include "MyEnrollmentsWidget.hpp"
#include "EnrollmentHistoryWidget.hpp"
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <iostream>

LessonView::LessonView(WebApplication* app) 
    : app_(app),
      contentStack_(nullptr),
      menuWidget_(nullptr),
      scheduleWidget_(nullptr),
      myEnrollmentsWidget_(nullptr),
      historyWidget_(nullptr) {
    
    std::cout << "🔧 Создание LessonView..." << std::endl;
    setupUI();
    std::cout << "✅ LessonView создан" << std::endl;
}

void LessonView::setupUI() {
    std::cout << "🔧 Настройка UI LessonView..." << std::endl;
    
    setStyleClass("lesson-view");
    
    // Заголовок
    auto header = addNew<Wt::WContainerWidget>();
    header->setStyleClass("lesson-header");
    auto title = header->addNew<Wt::WText>("<h1>🎓 Мои занятия</h1>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Навигационное меню
    auto nav = addNew<Wt::WContainerWidget>();
    nav->setStyleClass("lesson-nav");
    
    auto scheduleBtn = nav->addNew<Wt::WPushButton>("📅 Запись на занятие");
    scheduleBtn->setStyleClass("btn-nav");
    scheduleBtn->clicked().connect(this, &LessonView::showSchedule);
    
    auto enrollmentsBtn = nav->addNew<Wt::WPushButton>("📋 Мои записи");
    enrollmentsBtn->setStyleClass("btn-nav");
    enrollmentsBtn->clicked().connect(this, &LessonView::showMyEnrollments);
    
    auto historyBtn = nav->addNew<Wt::WPushButton>("📊 История посещений");
    historyBtn->setStyleClass("btn-nav");
    historyBtn->clicked().connect(this, &LessonView::showEnrollmentHistory);
    
    auto backBtn = nav->addNew<Wt::WPushButton>("← Назад в меню");
    backBtn->setStyleClass("btn-nav btn-back");
    backBtn->clicked().connect([this]() {
        app_->showDashboard();
    });
    
    // Контент
    contentStack_ = addNew<Wt::WStackedWidget>();
    contentStack_->setStyleClass("lesson-content");
    
    std::cout << "🔧 Создание виджетов LessonView..." << std::endl;
    
    // Создаем виджеты заранее
    menuWidget_ = contentStack_->addNew<Wt::WContainerWidget>();
    scheduleWidget_ = contentStack_->addNew<LessonScheduleWidget>(app_);
    myEnrollmentsWidget_ = contentStack_->addNew<MyEnrollmentsWidget>(app_);
    historyWidget_ = contentStack_->addNew<EnrollmentHistoryWidget>(app_);
    
    std::cout << "🔧 Настройка меню LessonView..." << std::endl;
    
    // Настраиваем меню
    menuWidget_->setStyleClass("lesson-welcome");
    menuWidget_->addNew<Wt::WText>("<h2>Добро пожаловать в систему занятий!</h2>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    menuWidget_->addNew<Wt::WText>("<p>Выберите действие из меню выше</p>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Показываем меню по умолчанию
    showLessonMenu();
    
    std::cout << "✅ UI LessonView настроен" << std::endl;
}

void LessonView::showLessonMenu() {
    std::cout << "🔄 Показываем меню занятий" << std::endl;
    if (contentStack_ && menuWidget_) {
        contentStack_->setCurrentWidget(menuWidget_);
    }
}

void LessonView::showSchedule() {
    std::cout << "🔄 Показываем расписание занятий" << std::endl;
    if (contentStack_ && scheduleWidget_) {
        contentStack_->setCurrentWidget(scheduleWidget_);
    }
}

void LessonView::showMyEnrollments() {
    std::cout << "🔄 Показываем мои записи" << std::endl;
    if (contentStack_ && myEnrollmentsWidget_) {
        myEnrollmentsWidget_->loadEnrollments();
        contentStack_->setCurrentWidget(myEnrollmentsWidget_);
    }
}

void LessonView::showEnrollmentHistory() {
    std::cout << "🔄 Показываем историю посещений" << std::endl;
    if (contentStack_ && historyWidget_) {
        historyWidget_->loadHistory();
        contentStack_->setCurrentWidget(historyWidget_);
    }
}