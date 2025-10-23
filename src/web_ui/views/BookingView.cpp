#include "BookingView.hpp"
#include "../WebApplication.hpp"
#include "BookingCreateWidget.hpp"
#include "BookingListWidget.hpp"
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <iostream>

BookingView::BookingView(WebApplication* app) 
    : app_(app) {
    setupUI();
}

void BookingView::setupUI() {
    setStyleClass("booking-view");
    
    // Заголовок
    auto header = addNew<Wt::WContainerWidget>();
    header->setStyleClass("booking-header");
    auto title = header->addNew<Wt::WText>("<h1>🏟️ Бронирование залов</h1>");
    title->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Навигационное меню
    auto nav = addNew<Wt::WContainerWidget>();
    nav->setStyleClass("booking-nav");
    
    auto createBtn = nav->addNew<Wt::WPushButton>("➕ Создать бронирование");
    createBtn->setStyleClass("btn-nav");
    createBtn->clicked().connect(this, &BookingView::showCreateBooking);
    
    auto listBtn = nav->addNew<Wt::WPushButton>("📋 Мои бронирования");
    listBtn->setStyleClass("btn-nav");
    listBtn->clicked().connect(this, &BookingView::showMyBookings);
    
    auto backBtn = nav->addNew<Wt::WPushButton>("← Назад в меню");
    backBtn->setStyleClass("btn-nav btn-back");
    backBtn->clicked().connect([this]() {
        // Вернуться в главное меню клиента
    });
    
    // Контент
    contentStack_ = addNew<Wt::WStackedWidget>();
    contentStack_->setStyleClass("booking-content");
    
    // Показываем меню по умолчанию
    showBookingMenu();
}

void BookingView::showBookingMenu() {
    // Можно добавить приветственный экран или статистику
    auto welcome = contentStack_->addNew<Wt::WContainerWidget>();
    welcome->setStyleClass("booking-welcome");
    welcome->addNew<Wt::WText>("<h2>Добро пожаловать в систему бронирования!</h2>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    welcome->addNew<Wt::WText>("<p>Выберите действие из меню выше</p>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    contentStack_->setCurrentWidget(welcome);
}

void BookingView::showCreateBooking() {
    auto createWidget = contentStack_->addNew<BookingCreateWidget>(app_);
    contentStack_->setCurrentWidget(createWidget);
}

void BookingView::showMyBookings() {
    auto listWidget = contentStack_->addNew<BookingListWidget>(app_);
    contentStack_->setCurrentWidget(listWidget);
}

void BookingView::showCancelBooking() {
    // Реализуем в BookingListWidget
    showMyBookings();
}