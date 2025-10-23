#include "LessonView.hpp"
#include <Wt/WLabel.h>
#include <Wt/WPushButton.h>
#include <Wt/WBreak.h>
#include <Wt/WMessageBox.h>

LessonView::LessonView(WebApplication* app) 
    : app_(app) {
    setupUI();
}

void LessonView::setupUI() {
    setStyleClass("lessons-view");
    
    // Заголовок
    auto title = addNew<Wt::WText>("<h2>🎓 Мои занятия</h2>");
    title->setStyleClass("section-title");
    
    // Фильтр
    auto filterContainer = addNew<Wt::WContainerWidget>();
    filterContainer->setStyleClass("filter-container");
    
    auto filterLabel = filterContainer->addNew<Wt::WLabel>("Показать:");
    filterComboBox_ = filterContainer->addNew<Wt::WComboBox>();
    filterComboBox_->addItem("Все занятия");
    filterComboBox_->addItem("Только доступные для записи");
    filterComboBox_->addItem("Только мои записи");
    filterComboBox_->addItem("Прошедшие занятия");
    filterLabel->setBuddy(filterComboBox_);
    filterComboBox_->changed().connect(this, &LessonView::handleFilterChange);
    
    // Раздел доступных занятий
    auto availableTitle = addNew<Wt::WText>("<h3>Доступные занятия</h3>");
    availableTitle->setStyleClass("subsection-title");
    
    lessonsTable_ = addNew<Wt::WTable>();
    lessonsTable_->setStyleClass("lessons-table");
    lessonsTable_->setHeaderCount(1);
    
    // Раздел моих записей
    auto enrollmentsTitle = addNew<Wt::WText>("<h3>Мои записи</h3>");
    enrollmentsTitle->setStyleClass("subsection-title");
    
    enrollmentsTable_ = addNew<Wt::WTable>();
    enrollmentsTable_->setStyleClass("enrollments-table");
    enrollmentsTable_->setHeaderCount(1);
    
    statusText_ = addNew<Wt::WText>();
    statusText_->setStyleClass("status-text");
    
    // Загружаем данные
    loadLessons();
    loadEnrollments();
}

void LessonView::loadLessons() {
    setupLessonsTable();
    
    // Заглушка с тестовыми данными
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, int>> testLessons = {
        {"2023-12-01 10:00", "Сальса для начинающих", "Анна Иванова", "Начинающий", "Зал 1", 5},
        {"2023-12-02 11:00", "Бачата", "Петр Сидоров", "Средний", "Зал 2", 3},
        {"2023-12-03 14:00", "Танго", "Мария Петрова", "Продвинутый", "Зал 1", 2},
        {"2023-12-04 16:00", "Хип-хоп", "Алексей Козлов", "Все уровни", "Зал 3", 8}
    };
    
    int row = 1;
    for (const auto& lesson : testLessons) {
        lessonsTable_->elementAt(row, 0)->addNew<Wt::WText>(std::get<0>(lesson));
        lessonsTable_->elementAt(row, 1)->addNew<Wt::WText>(std::get<1>(lesson));
        lessonsTable_->elementAt(row, 2)->addNew<Wt::WText>(std::get<2>(lesson));
        lessonsTable_->elementAt(row, 3)->addNew<Wt::WText>(std::get<3>(lesson));
        lessonsTable_->elementAt(row, 4)->addNew<Wt::WText>(std::get<4>(lesson));
        
        auto spotsText = lessonsTable_->elementAt(row, 5)->addNew<Wt::WText>(std::to_string(std::get<5>(lesson)));
        if (std::get<5>(lesson) < 3) {
            spotsText->setStyleClass("low-spots");
        }
        
        // Кнопка записи
        if (app_->getUserSession()->isAuthenticated() && app_->getUserSession()->isClient()) {
            auto enrollBtn = lessonsTable_->elementAt(row, 6)->addNew<Wt::WPushButton>("📝 Записаться");
            enrollBtn->setStyleClass("btn btn-sm btn-success");
            enrollBtn->clicked().connect([this, row]() {
                handleEnroll(UUID::generate()); // Заглушка UUID
            });
        }
        
        row++;
    }
}

void LessonView::loadEnrollments() {
    setupEnrollmentsTable();
    
    // Заглушка с тестовыми данными
    std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>> testEnrollments = {
        {"2023-12-01 10:00", "Сальса для начинающих", "Анна Иванова", "Зал 1", "Записан"},
        {"2023-12-05 18:00", "Сальса продвинутая", "Анна Иванова", "Зал 2", "Записан"},
        {"2023-11-28 14:00", "Бачата", "Петр Сидоров", "Зал 3", "Посещено"}
    };
    
    int row = 1;
    for (const auto& enrollment : testEnrollments) {
        enrollmentsTable_->elementAt(row, 0)->addNew<Wt::WText>(std::get<0>(enrollment));
        enrollmentsTable_->elementAt(row, 1)->addNew<Wt::WText>(std::get<1>(enrollment));
        enrollmentsTable_->elementAt(row, 2)->addNew<Wt::WText>(std::get<2>(enrollment));
        enrollmentsTable_->elementAt(row, 3)->addNew<Wt::WText>(std::get<3>(enrollment));
        
        auto statusText = enrollmentsTable_->elementAt(row, 4)->addNew<Wt::WText>(std::get<4>(enrollment));
        if (std::get<4>(enrollment) == "Посещено") {
            statusText->setStyleClass("status-attended");
        } else {
            statusText->setStyleClass("status-registered");
        }
        
        // Кнопка отмены для активных записей
        if (std::get<4>(enrollment) == "Записан") {
            auto cancelBtn = enrollmentsTable_->elementAt(row, 5)->addNew<Wt::WPushButton>("❌ Отменить");
            cancelBtn->setStyleClass("btn btn-sm btn-outline-danger");
            cancelBtn->clicked().connect([this, row]() {
                handleCancelEnrollment(UUID::generate()); // Заглушка UUID
            });
        } else {
            enrollmentsTable_->elementAt(row, 5)->addNew<Wt::WText>("-");
        }
        
        row++;
    }
}

void LessonView::setupLessonsTable() {
    lessonsTable_->clear();
    
    // Заголовки таблицы
    lessonsTable_->elementAt(0, 0)->addNew<Wt::WText>("Дата и время");
    lessonsTable_->elementAt(0, 1)->addNew<Wt::WText>("Занятие");
    lessonsTable_->elementAt(0, 2)->addNew<Wt::WText>("Преподаватель");
    lessonsTable_->elementAt(0, 3)->addNew<Wt::WText>("Уровень");
    lessonsTable_->elementAt(0, 4)->addNew<Wt::WText>("Зал");
    lessonsTable_->elementAt(0, 5)->addNew<Wt::WText>("Свободно");
    lessonsTable_->elementAt(0, 6)->addNew<Wt::WText>("Действия");
    
    for (int i = 0; i < 7; i++) {
        lessonsTable_->elementAt(0, i)->setStyleClass("table-header");
    }
}

void LessonView::setupEnrollmentsTable() {
    enrollmentsTable_->clear();
    
    // Заголовки таблицы
    enrollmentsTable_->elementAt(0, 0)->addNew<Wt::WText>("Дата и время");
    enrollmentsTable_->elementAt(0, 1)->addNew<Wt::WText>("Занятие");
    enrollmentsTable_->elementAt(0, 2)->addNew<Wt::WText>("Преподаватель");
    enrollmentsTable_->elementAt(0, 3)->addNew<Wt::WText>("Зал");
    enrollmentsTable_->elementAt(0, 4)->addNew<Wt::WText>("Статус");
    enrollmentsTable_->elementAt(0, 5)->addNew<Wt::WText>("Действия");
    
    for (int i = 0; i < 6; i++) {
        enrollmentsTable_->elementAt(0, i)->setStyleClass("table-header");
    }
}

void LessonView::handleEnroll(const UUID& lessonId) {
    if (!app_->getUserSession()->isAuthenticated()) {
        statusText_->setText("Ошибка: необходимо войти в систему.");
        statusText_->setStyleClass("status-text error");
        return;
    }
    
    // Здесь будет реальная логика записи на занятие
    statusText_->setText("✅ Вы успешно записались на занятие!");
    statusText_->setStyleClass("status-text success");
    
    // Обновляем списки
    loadLessons();
    loadEnrollments();
}

void LessonView::handleCancelEnrollment(const UUID& enrollmentId) {
    auto messageBox = addChild(
        std::make_unique<Wt::WMessageBox>(
            "Отмена записи",
            "Вы уверены, что хотите отменить запись на это занятие?",
            Wt::Icon::Question,
            Wt::StandardButton::Yes | Wt::StandardButton::No
        )
    );
    
    messageBox->buttonClicked().connect([=](Wt::StandardButton button) {
        if (button == Wt::StandardButton::Yes) {
            // Здесь будет реальная логика отмены записи
            statusText_->setText("Запись на занятие отменена.");
            statusText_->setStyleClass("status-text info");
            loadEnrollments();
            loadLessons();
        }
        removeChild(messageBox);
    });
    
    messageBox->show();
}

void LessonView::handleFilterChange() {
    loadLessons();
}