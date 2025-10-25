#include "MyEnrollmentsWidget.hpp"
#include "../WebApplication.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <Wt/WMessageBox.h>
#include <iostream>

MyEnrollmentsWidget::MyEnrollmentsWidget(WebApplication* app) 
    : app_(app),
      enrollmentsTable_(nullptr),
      statusText_(nullptr) {
    
    setupUI();
}

void MyEnrollmentsWidget::setupUI() {
    setStyleClass("my-enrollments-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("lesson-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("lesson-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>📋 Мои записи на занятия</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Кнопка обновления
    auto refreshBtn = header->addNew<Wt::WPushButton>("🔄 Обновить");
    refreshBtn->setStyleClass("btn-refresh");
    refreshBtn->clicked().connect([this]() {
        loadEnrollments();
    });
    
    // Таблица записей
    auto content = card->addNew<Wt::WContainerWidget>();
    content->setStyleClass("enrollment-list-content");
    
    enrollmentsTable_ = content->addNew<Wt::WTable>();
    enrollmentsTable_->setStyleClass("enrollment-table");
    
    // Статус
    statusText_ = content->addNew<Wt::WText>();
    statusText_->setStyleClass("lesson-status");
    
    // Загружаем записи
    loadEnrollments();
}

void MyEnrollmentsWidget::loadEnrollments() {
    try {
        // Очищаем таблицу
        enrollmentsTable_->clear();
        
        // Получаем записи текущего пользователя
        UUID clientId = app_->getCurrentClientId();
        auto enrollments = app_->getLessonController()->getClientEnrollments(clientId);
        
        // ФИЛЬТРАЦИЯ: оставляем только будущие занятия
        std::vector<EnrollmentResponseDTO> futureEnrollments;
        auto now = std::chrono::system_clock::now();
        
        for (const auto& enrollment : enrollments) {
            // Получаем информацию о занятии
            auto lesson = app_->getLessonController()->getLesson(enrollment.lessonId);
            
            // Проверяем, что занятие в будущем и статус "REGISTERED"
            if (lesson.timeSlot.getStartTime() > now && enrollment.status == "REGISTERED") {
                futureEnrollments.push_back(enrollment);
            }
        }
        
        if (futureEnrollments.empty()) {
            auto noEnrollmentsRow = enrollmentsTable_->elementAt(0, 0);
            noEnrollmentsRow->setColumnSpan(6);
            noEnrollmentsRow->addNew<Wt::WText>("<div style='text-align: center; padding: 2rem; color: #6c757d;'>У вас нет активных записей на будущие занятия</div>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
            return;
        }
        
        // Заголовки таблицы
        enrollmentsTable_->elementAt(0, 0)->addNew<Wt::WText>("<strong>Занятие</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        enrollmentsTable_->elementAt(0, 1)->addNew<Wt::WText>("<strong>Дата и время занятия</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        enrollmentsTable_->elementAt(0, 2)->addNew<Wt::WText>("<strong>Дата записи</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        enrollmentsTable_->elementAt(0, 3)->addNew<Wt::WText>("<strong>Статус</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        enrollmentsTable_->elementAt(0, 4)->addNew<Wt::WText>("<strong>Действия</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        int row = 1;
        for (const auto& enrollment : futureEnrollments) {
            // Получаем информацию о занятии
            auto lesson = app_->getLessonController()->getLesson(enrollment.lessonId);
            
            // Название занятия
            enrollmentsTable_->elementAt(row, 0)->addNew<Wt::WText>(lesson.name);
            
            // Дата и время занятия
            std::string lessonDateTime = DateTimeUtils::formatDateTime(lesson.timeSlot.getStartTime());
            enrollmentsTable_->elementAt(row, 1)->addNew<Wt::WText>(lessonDateTime);
            
            // Дата записи
            enrollmentsTable_->elementAt(row, 2)->addNew<Wt::WText>(enrollment.enrollmentDate);
            
            // Статус - всегда "Активная" для будущих занятий
            auto statusCell = enrollmentsTable_->elementAt(row, 3);
            auto statusText = statusCell->addNew<Wt::WText>("✅ Активная");
            statusCell->setStyleClass("status-registered");
            
            // Действия - всегда можно отменить для будущих занятий
            auto actionsCell = enrollmentsTable_->elementAt(row, 4);
            actionsCell->setStyleClass("cell-actions");
            
            auto cancelBtn = actionsCell->addNew<Wt::WPushButton>("❌ Отменить");
            cancelBtn->setStyleClass("btn-cancel");
            cancelBtn->clicked().connect([this, enrollment]() {
                handleCancelEnrollment(enrollment.enrollmentId);
            });
            
            row++;
        }
        
        updateStatus("✅ Загружено активных записей: " + std::to_string(futureEnrollments.size()), false);
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка загрузки записей: " + std::string(e.what()), true);
    }
}

void MyEnrollmentsWidget::handleCancelEnrollment(const UUID& enrollmentId) {
    try {
        // Диалог подтверждения
        auto dialog = addNew<Wt::WDialog>("Подтверждение отмены");
        dialog->setModal(true);
        dialog->setStyleClass("cancel-dialog");
        
        auto content = dialog->contents()->addNew<Wt::WContainerWidget>();
        content->setStyleClass("dialog-content");
        
        auto message = content->addNew<Wt::WText>(
            "<h3 style='text-align: center; margin-bottom: 1rem;'>Отмена записи на занятие</h3>"
            "<p style='text-align: center; color: #666;'>Вы уверены, что хотите отменить запись на занятие?</p>"
        );
        message->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        auto buttonContainer = dialog->footer()->addNew<Wt::WContainerWidget>();
        buttonContainer->setStyleClass("dialog-buttons");
        
        auto cancelBtn = buttonContainer->addNew<Wt::WPushButton>("Нет, оставить");
        cancelBtn->setStyleClass("btn-dialog-secondary");
        cancelBtn->clicked().connect([dialog]() {
            dialog->reject();
        });
        
        auto confirmBtn = buttonContainer->addNew<Wt::WPushButton>("Да, отменить");
        confirmBtn->setStyleClass("btn-dialog-primary");
        confirmBtn->clicked().connect([this, dialog, enrollmentId]() {
            dialog->accept();
            
            UUID clientId = app_->getCurrentClientId();
            auto response = app_->getLessonController()->cancelEnrollment(enrollmentId, clientId);
            
            updateStatus("✅ Запись на занятие отменена!", false);
            
            // Перезагружаем список
            loadEnrollments();
        });
        
        dialog->finished().connect([this, dialog](Wt::DialogCode code) {
            removeChild(dialog);
        });
        
        dialog->show();
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при отмене записи: " + std::string(e.what()), true);
    }
}

void MyEnrollmentsWidget::updateStatus(const std::string& message, bool isError) {
    statusText_->setText(message);
    if (isError) {
        statusText_->removeStyleClass("status-success");
        statusText_->addStyleClass("status-error");
    } else {
        statusText_->removeStyleClass("status-error");
        statusText_->addStyleClass("status-success");
    }
}