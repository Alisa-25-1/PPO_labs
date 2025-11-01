#include "BookingListWidget.hpp"
#include "../WebApplication.hpp"
#include <Wt/WTimer.h>
#include <Wt/WMessageBox.h>
#include <iostream>
#include <iomanip>
#include <sstream>

BookingListWidget::BookingListWidget(WebApplication* app) 
    : app_(app),
      bookingsTable_(nullptr),
      statusText_(nullptr) {
    
    std::cout << "🔧 Создание BookingListWidget..." << std::endl;
    setupUI();
    std::cout << "✅ BookingListWidget создан" << std::endl;
}

void BookingListWidget::setupUI() {
    std::cout << "🔧 Настройка UI BookingListWidget..." << std::endl;
    
    setStyleClass("booking-list-widget");
    
    auto card = addNew<Wt::WContainerWidget>();
    card->setStyleClass("booking-card");
    
    // Заголовок
    auto header = card->addNew<Wt::WContainerWidget>();
    header->setStyleClass("booking-card-header");
    auto headerText = header->addNew<Wt::WText>("<h2>📋 Мои бронирования</h2>");
    headerText->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Кнопка обновления
    auto refreshBtn = header->addNew<Wt::WPushButton>("🔄 Обновить");
    refreshBtn->setStyleClass("btn-refresh");
    refreshBtn->clicked().connect([this]() {
        loadBookings();
    });
    
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
    
    std::cout << "✅ UI BookingListWidget настроен" << std::endl;
}

void BookingListWidget::loadBookings() {
    try {
        // Очищаем таблицу
        bookingsTable_->clear();
        
        // Заголовки таблицы
        bookingsTable_->elementAt(0, 0)->addNew<Wt::WText>("<strong>Зал</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 1)->addNew<Wt::WText>("<strong>Дата и время</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 2)->addNew<Wt::WText>("<strong>Продолжительность</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 3)->addNew<Wt::WText>("<strong>Цель</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 4)->addNew<Wt::WText>("<strong>Статус</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        bookingsTable_->elementAt(0, 5)->addNew<Wt::WText>("<strong>Действия</strong>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        // Устанавливаем ширины столбцов
        bookingsTable_->setWidth("100%");
        
        // Получаем бронирования текущего пользователя
        auto bookings = getClientBookingsFromService();
        
        if (bookings.empty()) {
            auto noBookingsRow = bookingsTable_->elementAt(1, 0);
            noBookingsRow->setColumnSpan(6);
            noBookingsRow->addNew<Wt::WText>("<div style='text-align: center; padding: 2rem; color: #6c757d;'>У вас пока нет бронирований</div>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
            return;
        }
        
        int row = 1;
        for (const auto& booking : bookings) {
            // Название зала (убираем ID)
            std::string hallName = getHallNameById(booking.hallId);
            auto hallCell = bookingsTable_->elementAt(row, 0)->addNew<Wt::WText>(hallName);
            hallCell->setStyleClass("cell-hall-name");
            
            // Дата и время
            std::string datetimeStr = formatDateTime(booking.timeSlot.getStartTime(), booking.hallId);
            bookingsTable_->elementAt(row, 1)->addNew<Wt::WText>(datetimeStr);
            
            // Продолжительность
            std::string durationStr = std::to_string(booking.timeSlot.getDurationMinutes() / 60) + " ч";
            bookingsTable_->elementAt(row, 2)->addNew<Wt::WText>(durationStr);
            
            // Цель с ограничением длины
            std::string purpose = booking.purpose;
            if (purpose.length() > 30) {
                purpose = purpose.substr(0, 27) + "...";
            }
            auto purposeText = bookingsTable_->elementAt(row, 3)->addNew<Wt::WText>(purpose);
            purposeText->setToolTip(booking.purpose); // полный текст в тултипе
            purposeText->setStyleClass("cell-purpose");
            
            // Статус с цветовой индикацией
            auto statusCell = bookingsTable_->elementAt(row, 4);
            auto statusText = statusCell->addNew<Wt::WText>(getStatusDisplayName(booking.status));
            statusCell->setStyleClass(getStatusStyleClass(booking.status));
            
            // Действия - компактная кнопка
            auto actionsCell = bookingsTable_->elementAt(row, 5);
            actionsCell->setStyleClass("cell-actions");
            
            // Кнопка отмены показывается только для активных бронирований
            if (booking.status == "CONFIRMED" || booking.status == "PENDING") {
                auto cancelBtn = actionsCell->addNew<Wt::WPushButton>("❌");
                cancelBtn->setStyleClass("btn-cancel-compact");
                cancelBtn->setToolTip("Отменить бронирование");
                cancelBtn->clicked().connect([this, booking]() {
                    handleCancelBooking(booking.bookingId);
                });
            } else {
                actionsCell->addNew<Wt::WText>("—");
            }
            
            row++;
        }
        
        updateStatus("✅ Загружено бронирований: " + std::to_string(bookings.size()), false);
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка загрузки бронирований: " + std::string(e.what()), true);
    }
}

// Добавьте вспомогательные методы для статусов
std::string BookingListWidget::getStatusDisplayName(const std::string& status) {
    static std::map<std::string, std::string> statusMap = {
        {"CONFIRMED", "Подтверждено"},
        {"PENDING", "Ожидание"},
        {"CANCELLED", "Отменено"},
        {"COMPLETED", "Завершено"}
    };
    
    auto it = statusMap.find(status);
    return it != statusMap.end() ? it->second : status;
}

std::string BookingListWidget::getStatusStyleClass(const std::string& status) {
    static std::map<std::string, std::string> styleMap = {
        {"CONFIRMED", "status-confirmed"},
        {"PENDING", "status-pending"},
        {"CANCELLED", "status-cancelled"},
        {"COMPLETED", "status-completed"}
    };
    
    auto it = styleMap.find(status);
    return it != styleMap.end() ? it->second : "";
}

void BookingListWidget::handleCancelBooking(const UUID& bookingId) {
    try {
        // Создаем кастомный диалог вместо стандартного MessageBox
        auto dialog = addNew<Wt::WDialog>("Подтверждение отмены");
        dialog->setModal(true);
        dialog->setStyleClass("cancel-dialog");
        
        // Содержимое диалога
        auto content = dialog->contents()->addNew<Wt::WContainerWidget>();
        content->setStyleClass("dialog-content");
        
        // Иконка и текст
        auto icon = content->addNew<Wt::WText>("<div style='text-align: center; font-size: 3rem; margin-bottom: 1rem;'>⚠️</div>");
        icon->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        auto message = content->addNew<Wt::WText>(
            "<h3 style='text-align: center; margin-bottom: 1rem;'>Отмена бронирования</h3>"
            "<p style='text-align: center; color: #666;'>Вы уверены, что хотите отменить это бронирование?</p>"
        );
        message->setTextFormat(Wt::TextFormat::UnsafeXHTML);
        
        // Кнопки
        auto buttonContainer = dialog->footer()->addNew<Wt::WContainerWidget>();
        buttonContainer->setStyleClass("dialog-buttons");
        
        auto cancelBtn = buttonContainer->addNew<Wt::WPushButton>("Нет, оставить");
        cancelBtn->setStyleClass("btn-dialog-secondary");
        cancelBtn->clicked().connect([dialog]() {
            dialog->reject();
        });
        
        auto confirmBtn = buttonContainer->addNew<Wt::WPushButton>("Да, отменить");
        confirmBtn->setStyleClass("btn-dialog-primary");
        confirmBtn->clicked().connect([this, dialog, bookingId]() {
            dialog->accept();
            performCancelBooking(bookingId);
        });
        
        // Обработка закрытия диалога
        dialog->finished().connect([this, dialog](Wt::DialogCode code) {
            removeChild(dialog);
        });
        
        dialog->show();
        
    } catch (const std::exception& e) {
        updateStatus("❌ Ошибка при отмене бронирования: " + std::string(e.what()), true);
    }
}

void BookingListWidget::performCancelBooking(const UUID& bookingId) {
    try {
        // Вызываем сервис для отмены бронирования
        bool success = cancelBookingThroughService(bookingId);
        
        if (success) {
            updateStatus("✅ Бронирование успешно отменено!", false);
            
            // Перезагружаем список через 1 секунду
            auto timer = std::make_shared<Wt::WTimer>();
            timer->setSingleShot(true);
            timer->setInterval(std::chrono::seconds(1));
            timer->timeout().connect([this, timer]() {
                loadBookings();
            });
            timer->start();
        } else {
            updateStatus("❌ Не удалось отменить бронирование", true);
        }
        
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

std::vector<BookingResponseDTO> BookingListWidget::getClientBookingsFromService() {
    try {
        return app_->getBookingController()->getClientBookings(getCurrentClientId());
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения бронирований: " << e.what() << std::endl;
        return {};
    }
}

bool BookingListWidget::cancelBookingThroughService(const UUID& bookingId) {
    try {
        auto response = app_->getBookingController()->cancelBooking(bookingId, getCurrentClientId());
        return response.status == "CANCELLED";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка отмены бронирования: " << e.what() << std::endl;
        return false;
    }
}

UUID BookingListWidget::getCurrentClientId() {
    // Используем реальный ID клиента из сессии
    return app_->getCurrentClientId();
}

std::string BookingListWidget::getHallNameById(const UUID& hallId) {
    try {
        return app_->getBookingController()->getHallName(hallId);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения названия зала: " << e.what() << std::endl;
        return "Неизвестный зал";
    }
}

std::string BookingListWidget::formatDateTime(const std::chrono::system_clock::time_point& timePoint, const UUID& hallId) {
    try {
        auto offset = app_->getBookingController()->getTimezoneOffsetForHall(hallId);
        return DateTimeUtils::formatDateTimeWithOffset(timePoint, offset);
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка получения часового пояса для зала: " << e.what() << std::endl;
        return DateTimeUtils::formatDateTime(timePoint);
    }
}