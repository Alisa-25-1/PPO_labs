#include "EnrollmentHistoryWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WPushButton.h>

EnrollmentHistoryWidget::EnrollmentHistoryWidget(WebApplication* app) 
    : app_(app),
      historyTable_(nullptr),
      statusText_(nullptr) {
    
    setupUI();
}

void EnrollmentHistoryWidget::setupUI() {
    setStyleClass("enrollment-history-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("lesson-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("lesson-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>📊 История посещений</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Кнопка обновления
    auto refreshBtn = header->addNew<Wt::WPushButton>("🔄 Обновить");
    refreshBtn->setStyleClass("btn-refresh");
    refreshBtn->clicked().connect([this]() {
        loadHistory();
    });
    
    // Таблица истории
    auto content = card->addNew<Wt::WContainerWidget>();
    content->setStyleClass("history-list-content");
    
    historyTable_ = content->addNew<Wt::WTable>();
    historyTable_->setStyleClass("history-table");
    
    // Статус
    statusText_ = content->addNew<Wt::WText>();
    statusText_->setStyleClass("lesson-status");
    
    // Загружаем историю
    loadHistory();
}

void EnrollmentHistoryWidget::loadHistory() {
    try {
        // Очищаем таблицу
        historyTable_->clear();
        
        // Получаем все записи пользователя (включая исторические)
        UUID clientId = app_->getCurrentClientId();
        auto enrollments = app_->getLessonController()->getClientEnrollments(clientId);
        
        if (enrollments.empty()) {
            auto noHistoryRow = historyTable_->elementAt(0, 0);
            noHistoryRow->setColumnSpan(4);
            noHistoryRow->addNew<Wt::WText>("<div style='text-align: center; padding: 2rem; color: #6c757d;'>История посещений пуста</div>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
            return;
        }
        
        // Заголовки таблицы
        historyTable_->elementAt(0, 0)->addNew<Wt::WText>("<strong>Занятие</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        historyTable_->elementAt(0, 1)->addNew<Wt::WText>("<strong>Дата занятия</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        historyTable_->elementAt(0, 2)->addNew<Wt::WText>("<strong>Статус посещения</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        historyTable_->elementAt(0, 3)->addNew<Wt::WText>("<strong>Дата записи</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        int row = 1;
        for (const auto& enrollment : enrollments) {
            // Получаем информацию о занятии
            auto lesson = app_->getLessonController()->getLesson(enrollment.lessonId);
            
            // Название занятия
            historyTable_->elementAt(row, 0)->addNew<Wt::WText>(lesson.name);
            
            // Дата занятия - используем formatDateTime вместо formatDate
            std::string lessonDate = DateTimeUtils::formatDateTime(lesson.timeSlot.getStartTime());
            historyTable_->elementAt(row, 1)->addNew<Wt::WText>(lessonDate);
            
            // Статус посещения
            auto statusCell = historyTable_->elementAt(row, 2);
            std::string statusDisplay;
            std::string statusClass;
            
            if (enrollment.status == "ATTENDED") {
                statusDisplay = "✅ Посещено";
                statusClass = "status-attended";
            } else if (enrollment.status == "MISSED") {
                statusDisplay = "❌ Пропущено";
                statusClass = "status-missed";
            } else if (enrollment.status == "CANCELLED") {
                statusDisplay = "🚫 Отменено";
                statusClass = "status-cancelled";
            } else {
                statusDisplay = "⏳ Запланировано";
                statusClass = "status-registered";
            }
            
            auto statusText = statusCell->addNew<Wt::WText>(statusDisplay);
            statusCell->setStyleClass(statusClass);
            
            // Дата записи
            historyTable_->elementAt(row, 3)->addNew<Wt::WText>(enrollment.enrollmentDate);
            
            row++;
        }
        
        updateStatus("✅ Загружено записей в истории: " + std::to_string(enrollments.size()), false);
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка загрузки истории: " + std::string(e.what()), true);
    }
}

void EnrollmentHistoryWidget::updateStatus(const std::string& message, bool isError) {
    statusText_->setText(message);
    if (isError) {
        statusText_->removeStyleClass("status-success");
        statusText_->addStyleClass("status-error");
    } else {
        statusText_->removeStyleClass("status-error");
        statusText_->addStyleClass("status-success");
    }
}