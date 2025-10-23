#include "BookingCreateWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WBreak.h>
#include <Wt/WTimer.h>
#include <iostream>

BookingCreateWidget::BookingCreateWidget(WebApplication* app) 
    : app_(app) {
    setupUI();
}

void BookingCreateWidget::setupUI() {
    setStyleClass("booking-create-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("booking-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("booking-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>➕ Создать бронирование</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Форма
    auto form = card->addNew<Wt::WContainerWidget>();
    form->setStyleClass("booking-form");
    
    // Выбор зала
    auto hallGroup = form->addNew<Wt::WContainerWidget>();
    hallGroup->setStyleClass("form-group");
    
    auto hallLabel = hallGroup->addNew<Wt::WText>("<label class='form-label'>🏟️ Выберите зал</label>");
    hallLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    hallComboBox_ = hallGroup->addNew<Wt::WComboBox>();
    hallComboBox_->setStyleClass("form-input");
    hallComboBox_->changed().connect(this, &BookingCreateWidget::loadTimeSlots);
    
    // Дата
    auto dateGroup = form->addNew<Wt::WContainerWidget>();
    dateGroup->setStyleClass("form-group");
    
    auto dateLabel = dateGroup->addNew<Wt::WText>("<label class='form-label'>📅 Дата бронирования</label>");
    dateLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    dateEdit_ = dateGroup->addNew<Wt::WDateEdit>();
    dateEdit_->setStyleClass("form-input");
    dateEdit_->setDate(Wt::WDate::currentDate());
    dateEdit_->changed().connect(this, &BookingCreateWidget::loadTimeSlots);
    
    // Время
    auto timeGroup = form->addNew<Wt::WContainerWidget>();
    timeGroup->setStyleClass("form-group");
    
    auto timeLabel = timeGroup->addNew<Wt::WText>("<label class='form-label'>⏰ Время начала</label>");
    timeLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    timeComboBox_ = timeGroup->addNew<Wt::WComboBox>();
    timeComboBox_->setStyleClass("form-input");
    
    // Продолжительность
    auto durationGroup = form->addNew<Wt::WContainerWidget>();
    durationGroup->setStyleClass("form-group");
    
    auto durationLabel = durationGroup->addNew<Wt::WText>("<label class='form-label'>⏱️ Продолжительность</label>");
    durationLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    durationComboBox_ = durationGroup->addNew<Wt::WComboBox>();
    durationComboBox_->setStyleClass("form-input");
    durationComboBox_->addItem("1 час");
    durationComboBox_->addItem("2 часа");
    durationComboBox_->addItem("3 часа");
    durationComboBox_->addItem("4 часа");
    durationComboBox_->setCurrentIndex(0);
    
    // Цель бронирования
    auto purposeGroup = form->addNew<Wt::WContainerWidget>();
    purposeGroup->setStyleClass("form-group");
    
    auto purposeLabel = purposeGroup->addNew<Wt::WText>("<label class='form-label'>🎯 Цель бронирования</label>");
    purposeLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    purposeEdit_ = purposeGroup->addNew<Wt::WLineEdit>();
    purposeEdit_->setPlaceholderText("Например: Репетиция танцевальной группы");
    purposeEdit_->setStyleClass("form-input");
    
    // Кнопки
    auto buttonGroup = form->addNew<Wt::WContainerWidget>();
    buttonGroup->setStyleClass("booking-button-group");
    
    createButton_ = buttonGroup->addNew<Wt::WPushButton>("🚀 СОЗДАТЬ БРОНИРОВАНИЕ");
    createButton_->setStyleClass("btn-booking-create");
    createButton_->clicked().connect(this, &BookingCreateWidget::handleCreate);
    
    backButton_ = buttonGroup->addNew<Wt::WPushButton>("← Назад");
    backButton_->setStyleClass("btn-booking-back");
    backButton_->clicked().connect(this, &BookingCreateWidget::handleBack);
    
    // Статус
    statusText_ = form->addNew<Wt::WText>();
    statusText_->setStyleClass("booking-status");
    
    // Загружаем доступные залы
    loadAvailableHalls();
    loadTimeSlots();  // Загружаем временные слоты при инициализации
}

void BookingCreateWidget::handleCreate() {
    try {
        // Получаем выбранные значения
        int hallIndex = hallComboBox_->currentIndex();
        if (hallIndex < 0) {
            updateStatus("❌ Пожалуйста, выберите зал", true);
            return;
        }
        
        int timeIndex = timeComboBox_->currentIndex();
        if (timeIndex < 0) {
            updateStatus("❌ Пожалуйста, выберите время", true);
            return;
        }
        
        std::string purpose = purposeEdit_->text().toUTF8();
        if (purpose.empty()) {
            updateStatus("❌ Пожалуйста, укажите цель бронирования", true);
            return;
        }
        
        // Здесь должна быть реальная логика создания бронирования
        // Пока используем заглушку
        
        updateStatus("✅ Бронирование успешно создано! (заглушка)", false);
        
        // Очищаем форму
        purposeEdit_->setText("");
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при создании бронирования: " + std::string(e.what()), true);
    }
}

void BookingCreateWidget::handleBack() {
    // Возвращаемся к меню бронирований
    // В реальной реализации нужно уведомить родительский виджет
}

void BookingCreateWidget::updateStatus(const std::string& message, bool isError) {
    statusText_->setText(message);
    if (isError) {
        statusText_->removeStyleClass("status-success");
        statusText_->addStyleClass("status-error");
    } else {
        statusText_->removeStyleClass("status-error");
        statusText_->addStyleClass("status-success");
    }
}

void BookingCreateWidget::loadAvailableHalls() {
    try {
        // Загружаем доступные залы через контроллер
        // auto halls = app_->getBookingController()->getAvailableHalls();
        
        // Заглушка с тестовыми данными
        hallComboBox_->clear();
        hallComboBox_->addItem("🏟️ Зал 1 - Большой зал (вместимость: 50 чел.)");
        hallComboBox_->addItem("🏟️ Зал 2 - Малый зал (вместимость: 25 чел.)");
        hallComboBox_->addItem("🏟️ Зал 3 - Зеркальный зал (вместимость: 30 чел.)");
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки залов: " << e.what() << std::endl;
    }
}

void BookingCreateWidget::loadTimeSlots() {
    try {
        timeComboBox_->clear();
        
        // Заглушка с тестовыми временными слотами
        timeComboBox_->addItem("09:00 - 10:00");
        timeComboBox_->addItem("10:00 - 11:00");
        timeComboBox_->addItem("11:00 - 12:00");
        timeComboBox_->addItem("12:00 - 13:00");
        timeComboBox_->addItem("13:00 - 14:00");
        timeComboBox_->addItem("14:00 - 15:00");
        timeComboBox_->addItem("15:00 - 16:00");
        timeComboBox_->addItem("16:00 - 17:00");
        timeComboBox_->addItem("17:00 - 18:00");
        timeComboBox_->addItem("18:00 - 19:00");
        timeComboBox_->addItem("19:00 - 20:00");
        timeComboBox_->addItem("20:00 - 21:00");
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки временных слотов: " << e.what() << std::endl;
    }
}