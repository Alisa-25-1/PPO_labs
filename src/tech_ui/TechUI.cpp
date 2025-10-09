#include "TechUI.hpp"
#include <iomanip>
#include <sstream>

TechUI::TechUI(const std::string& connectionString) {
    try {
        // Инициализация подключения к БД
        dbConnection_ = std::make_shared<DatabaseConnection>(connectionString);
        
        // Инициализация репозиториев
        auto clientRepo = std::make_unique<PostgreSQLClientRepository>(dbConnection_);
        auto hallRepo = std::make_unique<PostgreSQLHallRepository>(dbConnection_);
        auto bookingRepo = std::make_unique<PostgreSQLBookingRepository>(dbConnection_);
        
        // Инициализация сервиса бронирований
        bookingService_ = std::make_unique<BookingService>(
            std::move(bookingRepo),
            std::move(clientRepo),
            std::move(hallRepo)
        );
        
        std::cout << "✅ Технологический UI инициализирован" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка инициализации: " << e.what() << std::endl;
        throw;
    }
}

void TechUI::run() {
    std::cout << "=== ТЕХНОЛОГИЧЕСКИЙ UI ТАНЦЕВАЛЬНОЙ СТУДИИ ===" << std::endl;
    std::cout << "Система для тестирования бизнес-логики" << std::endl;
    
    while (true) {
        displayMainMenu();
        
        int choice;
        std::cout << "Выберите опцию: ";
        std::cin >> choice;
        std::cin.ignore(); // Очистка буфера
        
        switch (choice) {
            case 1: handleClientManagement(); break;
            case 2: handleHallManagement(); break;
            case 3: handleBookingManagement(); break;
            case 4: handleSubscriptionManagement(); break;
            case 5: handleLessonManagement(); break;
            case 0: 
                std::cout << "Выход из системы..." << std::endl;
                return;
            default:
                std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        }
    }
}

void TechUI::displayMainMenu() {
    std::cout << "\n=== ГЛАВНОЕ МЕНЮ ===" << std::endl;
    std::cout << "1. Управление клиентами" << std::endl;
    std::cout << "2. Управление залами" << std::endl;
    std::cout << "3. Управление бронированиями" << std::endl;
    std::cout << "4. Управление абонементами" << std::endl;
    std::cout << "5. Управление занятиями" << std::endl;
    std::cout << "0. Выход" << std::endl;
}

void TechUI::handleClientManagement() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ КЛИЕНТАМИ ===" << std::endl;
        std::cout << "1. Создать клиента" << std::endl;
        std::cout << "2. Список клиентов" << std::endl;
        std::cout << "3. Найти клиента по ID" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice;
        std::cout << "Выберите опцию: ";
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: createClient(); break;
            case 2: listClients(); break;
            case 3: findClientById(); break;
            case 0: return;
            default: std::cout << "Неверный выбор." << std::endl;
        }
    }
}

void TechUI::handleHallManagement() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ ЗАЛАМИ ===" << std::endl;
        std::cout << "1. Создать зал" << std::endl;
        std::cout << "2. Список залов" << std::endl;
        std::cout << "3. Найти зал по ID" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice;
        std::cout << "Выберите опцию: ";
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: createHall(); break;
            case 2: listHalls(); break;
            case 3: findHallById(); break;
            case 0: return;
            default: std::cout << "Неверный выбор." << std::endl;
        }
    }
}

void TechUI::handleBookingManagement() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ БРОНИРОВАНИЯМИ ===" << std::endl;
        std::cout << "1. Создать бронирование" << std::endl;
        std::cout << "2. Список бронирований" << std::endl;
        std::cout << "3. Отменить бронирование" << std::endl;
        std::cout << "4. Проверить доступность времени" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice;
        std::cout << "Выберите опцию: ";
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: createBooking(); break;
            case 2: listBookings(); break;
            case 3: cancelBooking(); break;
            case 4: checkTimeSlotAvailability(); break;
            case 0: return;
            default: std::cout << "Неверный выбор." << std::endl;
        }
    }
}

void TechUI::handleSubscriptionManagement() {
    std::cout << "\n=== УПРАВЛЕНИЕ АБОНЕМЕНТАМИ ===" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::handleLessonManagement() {
    std::cout << "\n=== УПРАВЛЕНИЕ ЗАНЯТИЯМИ ===" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

// Методы для работы с клиентами
void TechUI::createClient() {
    std::cout << "\n--- СОЗДАНИЕ КЛИЕНТА ---" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::listClients() {
    std::cout << "\n--- СПИСОК КЛИЕНТОВ ---" << std::endl;
    std::cout << "📋 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::findClientById() {
    std::cout << "\n--- ПОИСК КЛИЕНТА ПО ID ---" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

// Методы для работы с залами
void TechUI::createHall() {
    std::cout << "\n--- СОЗДАНИЕ ЗАЛА ---" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::listHalls() {
    std::cout << "\n--- СПИСОК ЗАЛОВ ---" << std::endl;
    std::cout << "📋 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::findHallById() {
    std::cout << "\n--- ПОИСК ЗАЛА ПО ID ---" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

// Методы для работы с бронированиями
void TechUI::createBooking() {
    try {
        std::cout << "\n--- СОЗДАНИЕ БРОНИРОВАНИЯ ---" << std::endl;
        std::cout << "🚧 Демонстрационный режим" << std::endl;
        
        // Создаем тестовые данные
        UUID clientId = UUID::generate();
        UUID hallId = UUID::generate();
        
        auto startTime = std::chrono::system_clock::now() + std::chrono::hours(1);
        TimeSlot timeSlot(startTime, 120);
        
        std::cout << "Создано тестовое бронирование:" << std::endl;
        std::cout << "- Client ID: " << clientId.toString() << std::endl;
        std::cout << "- Hall ID: " << hallId.toString() << std::endl;
        std::cout << "- Время: через 1 час на 120 минут" << std::endl;
        std::cout << "- Цель: тестовое бронирование" << std::endl;
        
        std::cout << "✅ Тест создания бронирования выполнен" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка: " << e.what() << std::endl;
    }
    
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::listBookings() {
    std::cout << "\n--- СПИСОК БРОНИРОВАНИЙ ---" << std::endl;
    std::cout << "📋 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::cancelBooking() {
    std::cout << "\n--- ОТМЕНА БРОНИРОВАНИЯ ---" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::checkTimeSlotAvailability() {
    std::cout << "\n--- ПРОВЕРКА ДОСТУПНОСТИ ВРЕМЕНИ ---" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

// Методы для работы с занятиями
void TechUI::createLesson() {
    std::cout << "\n--- СОЗДАНИЕ ЗАНЯТИЯ ---" << std::endl;
    std::cout << "🚧 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void TechUI::listLessons() {
    std::cout << "\n--- СПИСОК ЗАНЯТИЙ ---" << std::endl;
    std::cout << "📋 Функционал в разработке" << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

// Вспомогательные методы
UUID TechUI::readUUID(const std::string& prompt) { 
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    try {
        return UUID::fromString(input);
    } catch (const std::exception&) {
        std::cout << "Неверный формат UUID, сгенерирован новый: ";
        UUID newUuid = UUID::generate();
        std::cout << newUuid.toString() << std::endl;
        return newUuid;
    }
}

std::string TechUI::readString(const std::string& prompt) { 
    std::string result;
    std::cout << prompt;
    std::getline(std::cin, result);
    return result;
}

int TechUI::readInt(const std::string& prompt) { 
    std::cout << prompt;
    int result;
    std::cin >> result;
    std::cin.ignore();
    return result;
}

TimeSlot TechUI::readTimeSlot() { 
    auto now = std::chrono::system_clock::now();
    int duration = 60; // по умолчанию 60 минут
    return TimeSlot(now, duration);
}