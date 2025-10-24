#include "BookingView.hpp"
#include "../WebApplication.hpp"
#include "BookingCreateWidget.hpp"
#include "BookingListWidget.hpp"
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <iostream>

BookingView::BookingView(WebApplication* app) 
    : app_(app),
      contentStack_(nullptr),
      menuWidget_(nullptr),
      createWidget_(nullptr),
      listWidget_(nullptr) {
    
    std::cout << "🔧 Создание BookingView..." << std::endl;
    setupUI();
    std::cout << "✅ BookingView создан" << std::endl;
}

void BookingView::setupUI() {
    std::cout << "🔧 Настройка UI BookingView..." << std::endl;
    
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
        app_->showDashboard();
    });
    
    // Контент
    contentStack_ = addNew<Wt::WStackedWidget>();
    contentStack_->setStyleClass("booking-content");
    
    std::cout << "🔧 Создание виджетов BookingView..." << std::endl;
    
    // Создаем виджеты заранее
    menuWidget_ = contentStack_->addNew<Wt::WContainerWidget>();
    createWidget_ = contentStack_->addNew<BookingCreateWidget>(app_);
    listWidget_ = contentStack_->addNew<BookingListWidget>(app_);

     // Настраиваем колбэк для возврата к списку
    createWidget_->setBackToListCallback([this]() {
        showMyBookings();
    });
    
    std::cout << "🔧 Настройка меню BookingView..." << std::endl;
    
    // Настраиваем меню
    menuWidget_->setStyleClass("booking-welcome");
    menuWidget_->addNew<Wt::WText>("<h2>Добро пожаловать в систему бронирования!</h2>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    menuWidget_->addNew<Wt::WText>("<p>Выберите действие из меню выше</p>")->setTextFormat(Wt::TextFormat::UnsafeXHTML);
    
    // Показываем меню по умолчанию
    showBookingMenu();
    
    std::cout << "✅ UI BookingView настроен" << std::endl;
}

void BookingView::showBookingMenu() {
    std::cout << "🔄 Показываем меню бронирований" << std::endl;
    if (contentStack_ && menuWidget_) {
        contentStack_->setCurrentWidget(menuWidget_);
    }
}

void BookingView::showCreateBooking() {
    std::cout << "🔄 Показываем создание бронирования" << std::endl;
    if (contentStack_ && createWidget_) {
        contentStack_->setCurrentWidget(createWidget_);
    }
}

void BookingView::showMyBookings() {
    std::cout << "🔄 Показываем список бронирований" << std::endl;
    if (contentStack_ && listWidget_) {
        // Обновляем список бронирований при каждом показе
        listWidget_->loadBookings();
        contentStack_->setCurrentWidget(listWidget_);
    }
}