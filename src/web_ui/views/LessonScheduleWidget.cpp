#include "LessonScheduleWidget.hpp"
#include "../WebApplication.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <Wt/WBreak.h>
#include <iostream>
#include <iomanip>
#include <sstream>

LessonScheduleWidget::LessonScheduleWidget(WebApplication* app) 
    : app_(app),
      branchComboBox_(nullptr),
      dateEdit_(nullptr),
      searchButton_(nullptr),
      lessonsTable_(nullptr),
      statusText_(nullptr) {
    
    setupUI();
}

void LessonScheduleWidget::setupUI() {
    setStyleClass("lesson-schedule-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("lesson-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("lesson-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>📅 Запись на занятие</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Форма поиска - ДОБАВЛЕНО ОБЪЯВЛЕНИЕ ПЕРЕМЕННОЙ
    auto form = card->addNew<Wt::WContainerWidget>();
    form->setStyleClass("lesson-search-form");
    
    // Выбор филиала
    auto branchGroup = form->addNew<Wt::WContainerWidget>();
    branchGroup->setStyleClass("form-group");
    
    auto branchLabel = branchGroup->addNew<Wt::WText>("<label class='form-label'>🏢 Выберите филиал</label>");
    branchLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    branchComboBox_ = branchGroup->addNew<Wt::WComboBox>();
    branchComboBox_->setStyleClass("form-input");
    
    // Дата
    auto dateGroup = form->addNew<Wt::WContainerWidget>();
    dateGroup->setStyleClass("form-group");
    
    auto dateLabel = dateGroup->addNew<Wt::WText>("<label class='form-label'>📅 Дата занятий</label>");
    dateLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    dateEdit_ = dateGroup->addNew<Wt::WDateEdit>();
    dateEdit_->setStyleClass("form-input");
    dateEdit_->setDate(Wt::WDate::currentDate());
    
    // Устанавливаем ограничения по дате
    Wt::WDate maxDate = Wt::WDate::currentDate().addDays(30);
    dateEdit_->setTop(maxDate);
    dateEdit_->setBottom(Wt::WDate::currentDate());
    
    // Кнопка поиска
    auto buttonGroup = form->addNew<Wt::WContainerWidget>();
    buttonGroup->setStyleClass("lesson-button-group");
    
    searchButton_ = buttonGroup->addNew<Wt::WPushButton>("🔍 Найти занятия");
    searchButton_->setStyleClass("btn-lesson-search");
    searchButton_->clicked().connect(this, &LessonScheduleWidget::handleSearch);
    
    // Таблица занятий
    auto content = card->addNew<Wt::WContainerWidget>();
    content->setStyleClass("lesson-list-content");
    
    lessonsTable_ = content->addNew<Wt::WTable>();
    lessonsTable_->setStyleClass("lesson-table");
    
    // Статус
    statusText_ = content->addNew<Wt::WText>();
    statusText_->setStyleClass("lesson-status");
    
    // Загружаем филиалы
    loadBranches();
    
    // Автоматически выполняем поиск при загрузке
    handleSearch();
}

void LessonScheduleWidget::handleSearch() {
    try {
        // Очищаем таблицу
        lessonsTable_->clear();
        
        // Проверяем, выбран ли филиал
        if (branchComboBox_->currentIndex() <= 0) {
            updateStatus("❌ Пожалуйста, выберите филиал", true);
            return;
        }
        
        // Получаем ID выбранного филиала из нашего mapping
        int selectedIndex = branchComboBox_->currentIndex();
        auto it = branchIdMap_.find(selectedIndex);
        if (it == branchIdMap_.end()) {
            updateStatus("❌ Ошибка: неверный выбор филиала", true);
            return;
        }
        
        UUID branchId = it->second;
        Wt::WDate selectedDate = dateEdit_->date();

        // Получаем часовой пояс филиала
        auto branchTimezoneOffset = app_->getLessonController()->getTimezoneOffsetForBranch(branchId);
        
        // Получаем занятия для выбранного филиала
        auto allLessons = app_->getLessonController()->getLessonsByBranch(branchId);
        
        // Фильтруем занятия по выбранной дате
        std::vector<LessonResponseDTO> filteredLessons;
        auto selectedTimePoint = DateTimeUtils::createDateTime(
            selectedDate.year(), selectedDate.month(), selectedDate.day(), 0, 0, 0
        );
        
        for (const auto& lesson : allLessons) {
            // Проверяем, что занятие в выбранный день и еще не прошло
            if (DateTimeUtils::isSameDay(lesson.timeSlot.getStartTime(), selectedTimePoint) &&
                lesson.timeSlot.getStartTime() > std::chrono::system_clock::now()) {
                filteredLessons.push_back(lesson);
            }
        }
        
        if (filteredLessons.empty()) {
            updateStatus("ℹ️ На выбранную дату нет доступных занятий", false);
            return;
        }
        
        // Сортируем занятия по времени начала
        std::sort(filteredLessons.begin(), filteredLessons.end(),
            [](const LessonResponseDTO& a, const LessonResponseDTO& b) {
                return a.timeSlot.getStartTime() < b.timeSlot.getStartTime();
            });
        
        // Заголовки таблицы
        lessonsTable_->elementAt(0, 0)->addNew<Wt::WText>("<strong>Занятие</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        lessonsTable_->elementAt(0, 1)->addNew<Wt::WText>("<strong>Время</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        lessonsTable_->elementAt(0, 2)->addNew<Wt::WText>("<strong>Тип</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        lessonsTable_->elementAt(0, 3)->addNew<Wt::WText>("<strong>Уровень</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        lessonsTable_->elementAt(0, 4)->addNew<Wt::WText>("<strong>Места</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        lessonsTable_->elementAt(0, 5)->addNew<Wt::WText>("<strong>Цена</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        lessonsTable_->elementAt(0, 6)->addNew<Wt::WText>("<strong>Действия</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        int row = 1;
        UUID currentClientId = app_->getCurrentClientId();
        
        for (const auto& lesson : filteredLessons) {
            // Название занятия
            auto nameCell = lessonsTable_->elementAt(row, 0);
            nameCell->addNew<Wt::WText>(lesson.name);
            nameCell->setStyleClass("cell-lesson-name");
            
            // Время с учетом часового пояса филиала
            std::string timeStr = DateTimeUtils::formatTimeSlotWithOffset(
                lesson.timeSlot.getStartTime(), 
                lesson.timeSlot.getDurationMinutes(),
                branchTimezoneOffset
            );
            lessonsTable_->elementAt(row, 1)->addNew<Wt::WText>(timeStr);
            
            // Тип занятия
            std::string typeStr;
            switch (lesson.type) {
                case LessonType::OPEN_CLASS: typeStr = "Открытый класс"; break;
                case LessonType::SPECIAL_COURSE: typeStr = "Курс"; break;
                case LessonType::INDIVIDUAL: typeStr = "Индивидуальное"; break;
                case LessonType::MASTERCLASS: typeStr = "Мастер-класс"; break;
                default: typeStr = "Занятие";
            }
            lessonsTable_->elementAt(row, 2)->addNew<Wt::WText>(typeStr);
            
            // Уровень сложности
            std::string levelStr;
            switch (lesson.difficulty) {
                case DifficultyLevel::BEGINNER: levelStr = "Начинающий"; break;
                case DifficultyLevel::INTERMEDIATE: levelStr = "Средний"; break;
                case DifficultyLevel::ADVANCED: levelStr = "Продвинутый"; break;
                case DifficultyLevel::ALL_LEVELS: levelStr = "Все уровни"; break;
                default: levelStr = "Начинающий";
            }
            lessonsTable_->elementAt(row, 3)->addNew<Wt::WText>(levelStr);
            
            // Свободные места
            int freeSpots = lesson.maxParticipants - lesson.currentParticipants;
            std::string spotsStr = std::to_string(freeSpots) + "/" + std::to_string(lesson.maxParticipants);
            auto spotsCell = lessonsTable_->elementAt(row, 4);
            spotsCell->addNew<Wt::WText>(spotsStr);
            if (freeSpots <= 0) {
                spotsCell->setStyleClass("cell-no-spots");
            } else if (freeSpots <= 3) {
                spotsCell->setStyleClass("cell-few-spots");
            }
            
            // Цена
            std::string priceStr = std::to_string(static_cast<int>(lesson.price)) + " руб.";
            lessonsTable_->elementAt(row, 5)->addNew<Wt::WText>(priceStr);
            
            // Действия
            auto actionsCell = lessonsTable_->elementAt(row, 6);
            actionsCell->setStyleClass("cell-actions");
            
            bool canEnroll = app_->getLessonController()->canClientEnroll(currentClientId, lesson.lessonId);
            
            if (canEnroll && freeSpots > 0) {
                auto enrollBtn = actionsCell->addNew<Wt::WPushButton>("✅ Записаться");
                enrollBtn->setStyleClass("btn-enroll");
                enrollBtn->clicked().connect([this, lesson]() {
                    handleEnroll(lesson.lessonId);
                });
            } else if (freeSpots <= 0) {
                actionsCell->addNew<Wt::WText>("❌ Нет мест");
            } else {
                actionsCell->addNew<Wt::WText>("✅ Записан");
            }
            
            row++;
        }
        
        std::string statusMessage = "✅ Найдено занятий на " + 
                                   selectedDate.toString("dd.MM.yyyy").toUTF8() + 
                                   ": " + std::to_string(filteredLessons.size());
        updateStatus(statusMessage, false);
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка поиска занятий: " + std::string(e.what()), true);
    }
}

void LessonScheduleWidget::handleEnroll(const UUID& lessonId) {
    try {
        UUID clientId = app_->getCurrentClientId();
        auto response = app_->getLessonController()->enrollInLesson(clientId, lessonId);
        
        updateStatus("✅ Вы успешно записались на занятие!", false);
        
        // Обновляем таблицу
        handleSearch();
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка записи на занятие: " + std::string(e.what()), true);
    }
}

void LessonScheduleWidget::updateStatus(const std::string& message, bool isError) {
    statusText_->setText(message);
    if (isError) {
        statusText_->removeStyleClass("status-success");
        statusText_->addStyleClass("status-error");
    } else {
        statusText_->removeStyleClass("status-error");
        statusText_->addStyleClass("status-success");
    }
}

void LessonScheduleWidget::loadBranches() {
    try {
        branches_ = app_->getLessonController()->getBranches();
        
        branchComboBox_->clear();
        branchComboBox_->addItem("-- Выберите филиал --");
        branchIdMap_.clear();
        
        for (size_t i = 0; i < branches_.size(); ++i) {
            const auto& branch = branches_[i];
            int index = branchComboBox_->count(); // Текущий индекс после добавления
            branchComboBox_->addItem(branch.getName());
            // Сохраняем mapping между индексом в комбобоксе и UUID филиала
            branchIdMap_[index] = branch.getId();
        }
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка загрузки филиалов", true);
    }
}