#include "BookingListWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WTimer.h>
#include <iostream>

BookingListWidget::BookingListWidget(WebApplication* app) 
    : app_(app) {
    setupUI();
}

void BookingListWidget::setupUI() {
    setStyleClass("booking-list-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("booking-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("booking-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>📋 Мои бронирования</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Таблица бронирований
    auto content = card->addNew<Wt::WContainerWidget>();
    content->setStyleClass("booking-list-content");
    
    bookingsTable_ = content->addNew<Wt::WTable>();
    bookingsTable_->setStyleClass("booking-table");
    
    // Статус
    statusText_ = content->addNew<Wt::WText>();
    statusText_->setStyleClass("booking-status");
    
    // Загружаем бронирования
    loadBookings();
}

void BookingListWidget::loadBookings() {
    try {
        // Очищаем таблицу
        bookingsTable_->clear();
        
        // Заголовки таблицы
        bookingsTable_->elementAt(0, 0)->addNew<Wt::WText>("<strong>ID</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 1)->addNew<Wt::WText>("<strong>Зал</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 2)->addNew<Wt::WText>("<strong>Дата и время</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 3)->addNew<Wt::WText>("<strong>Цель</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 4)->addNew<Wt::WText>("<strong>Статус</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 5)->addNew<Wt::WText>("<strong>Действия</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        // Заглушка с тестовыми данными
        int row = 1;
        
        // Тестовое бронирование 1
        bookingsTable_->elementAt(row, 0)->addNew<Wt::WText>("BKG-001");
        bookingsTable_->elementAt(row, 1)->addNew<Wt::WText>("🏟️ Зал 1");
        bookingsTable_->elementAt(row, 2)->addNew<Wt::WText>("15.12.2023 10:00-11:00");
        bookingsTable_->elementAt(row, 3)->addNew<Wt::WText>("Репетиция танцевальной группы");
        bookingsTable_->elementAt(row, 4)->addNew<Wt::WText>("✅ Подтверждено");
        
        auto cancelBtn1 = bookingsTable_->elementAt(row, 5)->addNew<Wt::WPushButton>("❌ Отменить");
        cancelBtn1->setStyleClass("btn-cancel-small");
        cancelBtn1->clicked().connect([this]() {
            handleCancelBooking();
        });
        
        row++;
        
        // Тестовое бронирование 2
        bookingsTable_->elementAt(row, 0)->addNew<Wt::WText>("BKG-002");
        bookingsTable_->elementAt(row, 1)->addNew<Wt::WText>("🏟️ Зал 2");
        bookingsTable_->elementAt(row, 2)->addNew<Wt::WText>("16.12.2023 14:00-16:00");
        bookingsTable_->elementAt(row, 3)->addNew<Wt::WText>("Индивидуальная тренировка");
        bookingsTable_->elementAt(row, 4)->addNew<Wt::WText>("⏳ Ожидание");
        
        auto cancelBtn2 = bookingsTable_->elementAt(row, 5)->addNew<Wt::WPushButton>("❌ Отменить");
        cancelBtn2->setStyleClass("btn-cancel-small");
        cancelBtn2->clicked().connect([this]() {
            handleCancelBooking();
        });
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка загрузки бронирований: " + std::string(e.what()), true);
    }
}

void BookingListWidget::handleCancelBooking() {
    try {
        // Здесь должна быть реальная логика отмены бронирования
        // Пока используем заглушку
        
        updateStatus("✅ Бронирование отменено! (заглушка)", false);
        
        // Перезагружаем список через 1 секунду
        auto timer = std::make_shared<Wt::WTimer>();
        timer->setSingleShot(true);
        timer->setInterval(std::chrono::seconds(1));
        timer->timeout().connect([this, timer]() {
            loadBookings();
        });
        timer->start();
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при отмене бронирования: " + std::string(e.what()), true);
    }
}

void BookingListWidget::updateStatus(const std::string& message, bool isError) {
    statusText_->setText(message);
    if (isError) {
        statusText_->removeStyleClass("status-success");
        statusText_->addStyleClass("status-error");
    } else {
        statusText_->removeStyleClass("status-error");
        statusText_->addStyleClass("status-success");
    }
}