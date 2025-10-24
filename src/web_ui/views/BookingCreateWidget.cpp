#include "BookingCreateWidget.hpp"
#include "../WebApplication.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <Wt/WBreak.h>
#include <Wt/WTimer.h>
#include <Wt/WMessageBox.h>
#include <iostream>
#include <sstream>
#include <iomanip>

BookingCreateWidget::BookingCreateWidget(WebApplication* app) 
    : app_(app),
      hallComboBox_(nullptr),
      dateEdit_(nullptr),
      timeComboBox_(nullptr),
      durationComboBox_(nullptr),
      purposeEdit_(nullptr),
      createButton_(nullptr),
      backButton_(nullptr),
      statusText_(nullptr) {
    
    std::cout << "🔧 Создание BookingCreateWidget..." << std::endl;
    setupUI();
    std::cout << "✅ BookingCreateWidget создан" << std::endl;
}

void BookingCreateWidget::setupUI() {
    std::cout << "🔧 Настройка UI BookingCreateWidget..." << std::endl;
    
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
    hallComboBox_->changed().connect(this, &BookingCreateWidget::loadAvailableTimeSlots);
    
    // Дата
    auto dateGroup = form->addNew<Wt::WContainerWidget>();
    dateGroup->setStyleClass("form-group");
    
    auto dateLabel = dateGroup->addNew<Wt::WText>("<label class='form-label'>📅 Дата бронирования</label>");
    dateLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    dateEdit_ = dateGroup->addNew<Wt::WDateEdit>();
    dateEdit_->setStyleClass("form-input");
    dateEdit_->setDate(Wt::WDate::currentDate());
    
    // Устанавливаем ограничения по дате (максимум +30 дней)
    Wt::WDate maxDate = Wt::WDate::currentDate().addDays(30);
    dateEdit_->setTop(maxDate);
    dateEdit_->setBottom(Wt::WDate::currentDate());
    
    dateEdit_->changed().connect(this, &BookingCreateWidget::loadAvailableTimeSlots);
    
    // Время начала
    auto timeGroup = form->addNew<Wt::WContainerWidget>();
    timeGroup->setStyleClass("form-group");
    
    auto timeLabel = timeGroup->addNew<Wt::WText>("<label class='form-label'>⏰ Время начала</label>");
    timeLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    timeComboBox_ = timeGroup->addNew<Wt::WComboBox>();
    timeComboBox_->setStyleClass("form-input");
    timeComboBox_->addItem("-- Выберите время --");
    timeComboBox_->setCurrentIndex(0);
    
    // Подключаем сигнал для обновления продолжительностей при выборе времени
    timeComboBox_->changed().connect(this, &BookingCreateWidget::updateAvailableDurations);
    
    // Продолжительность - УБИРАЕМ ПОВТОРНОЕ ОБЪЯВЛЕНИЕ
    auto durationGroup = form->addNew<Wt::WContainerWidget>();
    durationGroup->setStyleClass("form-group");
    
    auto durationLabel = durationGroup->addNew<Wt::WText>("<label class='form-label'>⏱️ Продолжительность</label>");
    durationLabel->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    durationComboBox_ = durationGroup->addNew<Wt::WComboBox>();
    durationComboBox_->setStyleClass("form-input");
    durationComboBox_->addItem("-- Сначала выберите время --");
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
    
    backButton_ = buttonGroup->addNew<Wt::WPushButton>("← Назад к списку");
    backButton_->setStyleClass("btn-booking-back");
    backButton_->clicked().connect(this, &BookingCreateWidget::handleBack);
    
    // Статус
    statusText_ = form->addNew<Wt::WText>();
    statusText_->setStyleClass("booking-status");
    
    // Загружаем доступные залы
    loadAvailableHalls();
    
    std::cout << "✅ UI BookingCreateWidget настроен" << std::endl;
}

void BookingCreateWidget::handleCreate() {
    try {
        // Проверяем обязательные поля
        if (hallComboBox_->currentIndex() <= 0) {
            updateStatus("❌ Пожалуйста, выберите зал", true);
            return;
        }
        
        if (timeComboBox_->currentIndex() <= 0) {
            updateStatus("❌ Пожалуйста, выберите время начала", true);
            return;
        }
        
        if (durationComboBox_->currentIndex() <= 0) {
            updateStatus("❌ Пожалуйста, выберите продолжительность", true);
            return;
        }
        
        std::string purpose = purposeEdit_->text().toUTF8();
        if (purpose.empty()) {
            updateStatus("❌ Пожалуйста, укажите цель бронирования", true);
            return;
        }
        
        if (purpose.length() < 3) {
            updateStatus("❌ Цель бронирования должна содержать не менее 3 символов", true);
            return;
        }
        
        // Получаем выбранные значения
        int hallIndex = hallComboBox_->currentIndex() - 1; // -1 потому что первый элемент "Выберите зал"
        auto halls = getAvailableHallsFromDB();
        if (hallIndex < 0 || hallIndex >= halls.size()) {
            updateStatus("❌ Ошибка: неверный выбор зала", true);
            return;
        }
        
        UUID hallId = halls[hallIndex].getId();
        Wt::WDate selectedDate = dateEdit_->date();
        std::string timeStr = timeComboBox_->currentText().toUTF8();
        
        // Парсим время (формат "HH:MM")
        int hours, minutes;
        if (sscanf(timeStr.c_str(), "%d:%d", &hours, &minutes) != 2) {
            updateStatus("❌ Ошибка формата времени", true);
            return;
        }
        
        // Создаем временную точку начала
        auto startTime = createDateTime(selectedDate, hours, minutes);
        
        // Получаем продолжительность из выбранного индекса
        int durationIndex = durationComboBox_->currentIndex();
        int durationMinutes = 0;
        
        // Определяем продолжительность в минутах на основе выбранного индекса
        switch (durationIndex) {
            case 1: durationMinutes = 60; break;   // 1 час
            case 2: durationMinutes = 120; break;  // 2 часа
            case 3: durationMinutes = 180; break;  // 3 часа
            case 4: durationMinutes = 240; break;  // 4 часа
            default: 
                updateStatus("❌ Неверная продолжительность", true);
                return;
        }
        
        // Создаем TimeSlot
        TimeSlot timeSlot(startTime, durationMinutes);
        
        // Проверяем доступность слота
        if (!isTimeSlotAvailable(hallId, timeSlot)) {
            updateStatus("❌ Выбранное время уже занято. Пожалуйста, выберите другое время.", true);
            return;
        }
        
        // Получаем ID текущего клиента
        UUID clientId = getCurrentClientId();
        
        // Создаем DTO запроса
        BookingRequestDTO request;
        request.clientId = clientId;
        request.hallId = hallId;
        request.timeSlot = timeSlot;
        request.purpose = purpose;
        
        // Вызываем сервис для создания бронирования
        auto response = createBookingThroughService(request);
        
        updateStatus("✅ Бронирование успешно создано! ID: " + response.bookingId.toString(), false);
        
        // Очищаем форму
        purposeEdit_->setText("");
        timeComboBox_->setCurrentIndex(0);
        durationComboBox_->setCurrentIndex(0);
        
        // Автоматическое обновление доступных слотов
        loadAvailableTimeSlots();
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при создании бронирования: " + std::string(e.what()), true);
    }
}

void BookingCreateWidget::handleBack() {
    // Вызываем колбэк для возврата к списку бронирований
    if (onBackToList_) {
        onBackToList_();
    }
}

void BookingCreateWidget::setBackToListCallback(std::function<void()> callback) {
    onBackToList_ = std::move(callback);
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
        auto halls = getAvailableHallsFromDB();
        
        hallComboBox_->clear();
        hallComboBox_->addItem("-- Выберите зал --");
        
        for (const auto& hall : halls) {
            std::string hallInfo = "🏟️ " + hall.getName() + 
                                 " (Вместимость: " + std::to_string(hall.getCapacity()) + " чел.)" +
                                 " - " + hall.getDescription();
            hallComboBox_->addItem(hallInfo);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки залов: " << e.what() << std::endl;
        updateStatus("❌ Ошибка загрузки списка залов", true);
    }
}

void BookingCreateWidget::loadAvailableTimeSlots() {
    try {
        // Сбрасываем комбобокс времени
        timeComboBox_->clear();
        timeComboBox_->addItem("-- Выберите время --");
        
        // Сбрасываем комбобокс продолжительности
        durationComboBox_->clear();
        durationComboBox_->addItem("-- Сначала выберите время --");
        
        // Проверяем, выбран ли зал
        if (hallComboBox_->currentIndex() <= 0) {
            return;
        }
        
        // Получаем выбранный зал
        int hallIndex = hallComboBox_->currentIndex() - 1;
        auto halls = getAvailableHallsFromDB();
        if (hallIndex < 0 || hallIndex >= halls.size()) {
            return;
        }
        
        UUID hallId = halls[hallIndex].getId();
        Wt::WDate selectedDate = dateEdit_->date();
        
        // Получаем доступные временные слоты для выбранной даты
        auto availableSlots = getAvailableTimeSlotsFromService(hallId, selectedDate);
        
        // Заполняем комбобокс доступными временами с информацией о минимальной продолжительности
        for (const auto& slot : availableSlots) {
            auto timePoint = slot.getStartTime();
            
            // Используем DateTimeUtils для форматирования времени
            std::string timeStr = DateTimeUtils::formatTime(timePoint);
            
            timeComboBox_->addItem(timeStr);
        }
        
        if (availableSlots.empty()) {
            timeComboBox_->addItem("❌ Нет доступных слотов на эту дату");
            timeComboBox_->setCurrentIndex(1);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки временных слотов: " << e.what() << std::endl;
        timeComboBox_->addItem("❌ Ошибка загрузки");
    }
}

void BookingCreateWidget::updateAvailableDurations() {
    try {
        // Сбрасываем комбобокс продолжительности
        durationComboBox_->clear();
        durationComboBox_->addItem("-- Выберите продолжительность --");
        
        // Проверяем, выбраны ли все необходимые поля
        if (hallComboBox_->currentIndex() <= 0 || timeComboBox_->currentIndex() <= 0) {
            return;
        }
        
        // Получаем выбранный зал
        int hallIndex = hallComboBox_->currentIndex() - 1;
        auto halls = getAvailableHallsFromDB();
        if (hallIndex < 0 || hallIndex >= halls.size()) {
            return;
        }
        
        UUID hallId = halls[hallIndex].getId();
        Wt::WDate selectedDate = dateEdit_->date();
        std::string timeStr = timeComboBox_->currentText().toUTF8();
        
        // Извлекаем только время (игнорируем часть в скобках)
        size_t bracketPos = timeStr.find('(');
        if (bracketPos != std::string::npos) {
            timeStr = timeStr.substr(0, bracketPos);
            // Убираем пробелы в конце
            while (!timeStr.empty() && std::isspace(timeStr.back())) {
                timeStr.pop_back();
            }
        }
        
        // Парсим время (формат "HH:MM")
        int hours, minutes;
        if (sscanf(timeStr.c_str(), "%d:%d", &hours, &minutes) != 2) {
            return;
        }
        
        // Создаем временную точку начала
        auto startTime = createDateTime(selectedDate, hours, minutes);
        
        // Получаем доступные продолжительности через контроллер
        auto availableDurations = app_->getBookingController()->getAvailableDurations(hallId, startTime);
        
        // Заполняем комбобокс доступными продолжительностями
        for (int duration : availableDurations) {
            std::string durationText;
            if (duration == 60) {
                durationText = "1 час (60 минут)";
            } else if (duration == 120) {
                durationText = "2 часа (120 минут)";
            } else if (duration == 180) {
                durationText = "3 часа (180 минут)";
            } else if (duration == 240) {
                durationText = "4 часа (240 минут)";
            } else {
                durationText = std::to_string(duration/60) + " часа (" + std::to_string(duration) + " минут)";
            }
            durationComboBox_->addItem(durationText);
        }
        
        if (availableDurations.empty()) {
            durationComboBox_->addItem("❌ Нет доступных продолжительностей");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки продолжительностей: " << e.what() << std::endl;
        durationComboBox_->addItem("❌ Ошибка загрузки");
    }
}

// Вспомогательные методы для работы с бизнес-логикой
std::vector<DanceHall> BookingCreateWidget::getAvailableHallsFromDB() {
    try {
        // Используем BookingController для получения реальных данных
        return app_->getBookingController()->getAvailableHalls();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения залов: " << e.what() << std::endl;
        // Возвращаем пустой список в случае ошибки
        return {};
    }
}

std::vector<TimeSlot> BookingCreateWidget::getAvailableTimeSlotsFromService(const UUID& hallId, const Wt::WDate& date) {
    try {
        auto timePoint = createDateTime(date, 0, 0);
        return app_->getBookingController()->getAvailableTimeSlots(hallId, timePoint);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения временных слотов: " << e.what() << std::endl;
        return {};
    }
}

bool BookingCreateWidget::isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) {
    try {
        return app_->getBookingController()->isTimeSlotAvailable(hallId, timeSlot);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка проверки доступности слота: " << e.what() << std::endl;
        return false;
    }
}

BookingResponseDTO BookingCreateWidget::createBookingThroughService(const BookingRequestDTO& request) {
    try {
        return app_->getBookingController()->createBooking(request);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка создания бронирования: " << e.what() << std::endl;
        throw; // Пробрасываем исключение дальше
    }
}

UUID BookingCreateWidget::getCurrentClientId() {
    // Используем реальный ID клиента из сессии
    return app_->getCurrentClientId();
}

std::chrono::system_clock::time_point BookingCreateWidget::createDateTime(const Wt::WDate& date, int hours, int minutes) {
    return DateTimeUtils::createDateTime(
        date.year(), date.month(), date.day(), 
        hours, minutes, 0
    );
}