#include "ScheduleView.hpp"
#include <Wt/WLabel.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WBreak.h>
#include <Wt/WCssStyleSheet.h>
#include <iomanip>
#include <sstream>

ScheduleView::ScheduleView(WebApplication* app) 
    : app_(app) {
    setupUI();
}

void ScheduleView::setupUI() {
    setStyleClass("schedule-view");
    
    // Заголовок
    auto title = addNew<Wt::WText>("<h2>📅 Расписание занятий</h2>");
    title->setStyleClass("section-title");
    
    // Элементы управления
    auto controls = addNew<Wt::WContainerWidget>();
    controls->setStyleClass("schedule-controls");
    
    // Выбор филиала
    auto branchContainer = controls->addNew<Wt::WContainerWidget>();
    branchContainer->setStyleClass("control-group");
    
    auto branchLabel = branchContainer->addNew<Wt::WLabel>("Филиал:");
    branchComboBox_ = branchContainer->addNew<Wt::WComboBox>();
    branchComboBox_->addItem("Все филиалы");
    branchComboBox_->addItem("Центральный филиал");
    branchComboBox_->addItem("Северный филиал");
    branchLabel->setBuddy(branchComboBox_);
    branchComboBox_->changed().connect(this, &ScheduleView::handleBranchChange);
    
    // Выбор даты
    auto dateContainer = controls->addNew<Wt::WContainerWidget>();
    dateContainer->setStyleClass("control-group");
    
    auto dateLabel = dateContainer->addNew<Wt::WLabel>("Дата:");
    datePicker_ = dateContainer->addNew<Wt::WDatePicker>();
    datePicker_->setDate(Wt::WDate::currentDate());
    // Убираем setBuddy для datePicker, так как WDatePicker не является WFormWidget
    // dateLabel->setBuddy(datePicker_);
    datePicker_->changed().connect(this, &ScheduleView::handleDateChange);
    
    // Кнопка обновления
    auto refreshBtn = controls->addNew<Wt::WPushButton>("🔄 Обновить");
    refreshBtn->setStyleClass("btn btn-outline-primary");
    refreshBtn->clicked().connect(this, &ScheduleView::loadSchedule);
    
    // Статус
    statusText_ = addNew<Wt::WText>();
    statusText_->setStyleClass("status-text");
    
    // Таблица расписания
    scheduleTable_ = addNew<Wt::WTable>();
    scheduleTable_->setStyleClass("schedule-table");
    scheduleTable_->setHeaderCount(1);
    
    // Загружаем начальные данные
    loadSchedule();
}

void ScheduleView::loadSchedule() {
    auto selectedDate = datePicker_->date();
    // Исправляем преобразование даты
    auto time_t = selectedDate.toJulianDay() * 86400; // Простое преобразование для демонстрации
    auto timePoint = std::chrono::system_clock::from_time_t(time_t);
    
    displayScheduleForDate(timePoint);
}

void ScheduleView::setupScheduleTable() {
    scheduleTable_->clear();
    
    // Заголовки таблицы
    scheduleTable_->elementAt(0, 0)->addNew<Wt::WText>("Время");
    scheduleTable_->elementAt(0, 1)->addNew<Wt::WText>("Зал");
    scheduleTable_->elementAt(0, 2)->addNew<Wt::WText>("Занятие");
    scheduleTable_->elementAt(0, 3)->addNew<Wt::WText>("Преподаватель");
    scheduleTable_->elementAt(0, 4)->addNew<Wt::WText>("Уровень");
    scheduleTable_->elementAt(0, 5)->addNew<Wt::WText>("Свободно");
    scheduleTable_->elementAt(0, 6)->addNew<Wt::WText>("Действия");
    
    // Стили для заголовков
    for (int i = 0; i < 7; i++) {
        scheduleTable_->elementAt(0, i)->setStyleClass("table-header");
    }
}

void ScheduleView::displayScheduleForDate(const std::chrono::system_clock::time_point& date) {
    setupScheduleTable();
    
    // Заглушка с тестовыми данными
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, int>> testLessons = {
        {"10:00-11:00", "Зал 1", "Сальса для начинающих", "Анна Иванова", "Начинающий", 5},
        {"11:00-12:00", "Зал 2", "Бачата", "Петр Сидоров", "Средний", 3},
        {"14:00-15:30", "Зал 1", "Танго", "Мария Петрова", "Продвинутый", 2},
        {"16:00-17:00", "Зал 3", "Хип-хоп", "Алексей Козлов", "Все уровни", 8},
        {"18:00-19:30", "Зал 2", "Сальса продвинутая", "Анна Иванова", "Продвинутый", 1}
    };
    
    int row = 1;
    for (const auto& lesson : testLessons) {
        scheduleTable_->elementAt(row, 0)->addNew<Wt::WText>(std::get<0>(lesson));
        scheduleTable_->elementAt(row, 1)->addNew<Wt::WText>(std::get<1>(lesson));
        scheduleTable_->elementAt(row, 2)->addNew<Wt::WText>(std::get<2>(lesson));
        scheduleTable_->elementAt(row, 3)->addNew<Wt::WText>(std::get<3>(lesson));
        scheduleTable_->elementAt(row, 4)->addNew<Wt::WText>(std::get<4>(lesson));
        
        auto spotsText = scheduleTable_->elementAt(row, 5)->addNew<Wt::WText>(std::to_string(std::get<5>(lesson)));
        if (std::get<5>(lesson) < 3) {
            spotsText->setStyleClass("low-spots");
        }
        
        // Кнопка записи - исправляем синтаксис
        if (app_->getUserSession()->isAuthenticated() && app_->getUserSession()->isClient()) {
            auto enrollBtn = scheduleTable_->elementAt(row, 6)->addNew<Wt::WPushButton>("📝 Записаться");
            enrollBtn->setStyleClass("btn btn-sm btn-success");
            enrollBtn->clicked().connect([this, row]() {
                statusText_->setText("Запись на занятие " + std::to_string(row));
            });
        } else {
            scheduleTable_->elementAt(row, 6)->addNew<Wt::WText>("Войдите для записи");
        }
        
        row++;
    }
    
    if (testLessons.empty()) {
        statusText_->setText("На выбранную дату занятий не найдено.");
        statusText_->setStyleClass("status-text info");
    } else {
        statusText_->setText("Найдено " + std::to_string(testLessons.size()) + " занятий");
        statusText_->setStyleClass("status-text success");
    }
}

void ScheduleView::handleDateChange() {
    loadSchedule();
}

void ScheduleView::handleBranchChange() {
    loadSchedule();
}