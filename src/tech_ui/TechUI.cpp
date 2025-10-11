#include "TechUI.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

TechUI::TechUI(const std::string& connectionString) {
    try {
        managers_ = std::make_unique<TechUIManagers>(connectionString);
        std::cout << "✅ Технологический UI инициализирован" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка инициализации TechUI: " << e.what() << std::endl;
        throw;
    }
}

void TechUI::run() {
    std::cout << "=== ТЕХНОЛОГИЧЕСКИЙ UI ТАНЦЕВАЛЬНОЙ СТУДИИ ===" << std::endl;
    std::cout << "Система для тестирования бизнес-логики" << std::endl;
    
    while (true) {
        displayMainMenu();
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 2);
        
        switch (choice) {
            case 1: handleClientMode(); break;
            case 2: handleAdminMode(); break;
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
    std::cout << "1. Режим клиента" << std::endl;
    std::cout << "2. Режим администратора" << std::endl;
    std::cout << "0. Выход" << std::endl;
}

void TechUI::handleClientMode() {
    if (!isClientLoggedIn()) {
        handleClientAuth();
    }
    
    if (isClientLoggedIn()) {
        handleClientMenu();
    }
}

void TechUI::handleClientAuth() {
    while (true) {
        std::cout << "\n=== АВТОРИЗАЦИЯ КЛИЕНТА ===" << std::endl;
        std::cout << "1. Регистрация" << std::endl;
        std::cout << "2. Вход" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 2);
        
        switch (choice) {
            case 1: registerClient(); break;
            case 2: loginClient(); break;
            case 0: return;
        }
        
        if (isClientLoggedIn()) {
            break;
        }
    }
}

void TechUI::registerClient() {
    try {
        std::cout << "\n--- РЕГИСТРАЦИЯ КЛИЕНТА ---" << std::endl;
        
        std::string name = InputHandlers::readString("Введите ФИО: ");
        std::string email = InputHandlers::readEmail("Введите email: ");
        std::string phone = InputHandlers::readPhone("Введите телефон: ");
        std::string password = InputHandlers::readPassword("Введите пароль: ");
        
        // Используем конструктор для регистрации
        AuthRequestDTO request(name, email, phone, password);
        
        // Предварительная проверка данных
        if (name.empty() || email.empty() || phone.empty() || password.empty()) {
            std::cerr << "❌ Все поля должны быть заполнены" << std::endl;
            return;
        }
        
        auto response = managers_->getAuthService()->registerClient(request);
        
        currentClientId_ = response.clientId;
        std::cout << "✅ Регистрация успешна! ID клиента: " << currentClientId_.toString() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка регистрации: " << e.what() << std::endl;
    }
}

void TechUI::loginClient() {
    try {
        std::cout << "\n--- ВХОД КЛИЕНТА ---" << std::endl;
        
        std::string email = InputHandlers::readEmail("Введите email: ");
        std::string password = InputHandlers::readPassword("Введите пароль: ");
        
        // Проверяем, что поля не пустые
        if (email.empty() || password.empty()) {
            std::cerr << "❌ Email и пароль не могут быть пустыми" << std::endl;
            return;
        }
        
        // Используем конструктор для входа
        AuthRequestDTO request(email, password);
        auto response = managers_->getAuthService()->login(request);
        
        currentClientId_ = response.clientId;
        std::cout << "✅ Вход успешен! Добро пожаловать, " << response.name << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка входа: " << e.what() << std::endl;
    }
}

void TechUI::handleClientMenu() {
    while (isClientLoggedIn()) {
        std::cout << "\n=== МЕНЮ КЛИЕНТА ===" << std::endl;
        std::cout << "1. Бронирование залов" << std::endl;
        std::cout << "2. Запись на занятия" << std::endl;
        std::cout << "3. Абонементы" << std::endl;
        std::cout << "4. Отзывы" << std::endl;
        std::cout << "0. Выход" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 4);
        
        switch (choice) {
            case 1: handleClientBookings(); break;
            case 2: handleClientLessons(); break;
            case 3: handleClientSubscriptions(); break;
            case 4: handleClientReviews(); break;
            case 0: 
                currentClientId_ = UUID();
                return;
        }
    }
}

void TechUI::handleClientBookings() {
    while (true) {
        std::cout << "\n=== БРОНИРОВАНИЯ ===" << std::endl;
        std::cout << "1. Создать бронирование" << std::endl;
        std::cout << "2. Мои бронирования" << std::endl;
        std::cout << "3. Отменить бронирование" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 3);
        
        switch (choice) {
            case 1: createBooking(); break;
            case 2: viewClientBookings(); break;
            case 3: cancelBooking(); break;
            case 0: return;
        }
    }
}

void TechUI::createBooking() {
    try {
        std::cout << "\n--- СОЗДАНИЕ БРОНИРОВАНИЯ ---" << std::endl;
        
        auto halls = managers_->getAvailableHalls();
        if (halls.empty()) {
            std::cout << "❌ Нет доступных залов." << std::endl;
            return;
        }
        
        std::cout << "Доступные залы:" << std::endl;
        for (const auto& hall : halls) {
            std::cout << "- " << hall.getName() << " (ID: " << hall.getId().toString() 
                      << ", Вместимость: " << hall.getCapacity() << ")" << std::endl;
        }
        
        UUID hallId = InputHandlers::readUUID("Введите ID зала: ");
        TimeSlot timeSlot = InputHandlers::readTimeSlot();
        std::string purpose = InputHandlers::readString("Цель бронирования: ");
        
        BookingRequestDTO request{currentClientId_, hallId, timeSlot, purpose};
        auto response = managers_->getBookingService()->createBooking(request);
        
        std::cout << "✅ Бронирование создано! ID: " << response.bookingId.toString() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка создания бронирования: " << e.what() << std::endl;
    }
}

void TechUI::viewClientBookings() {
    try {
        std::cout << "\n--- МОИ БРОНИРОВАНИЯ ---" << std::endl;
        
        auto bookings = managers_->getBookingService()->getClientBookings(currentClientId_);
        
        if (bookings.empty()) {
            std::cout << "У вас нет активных бронирований." << std::endl;
            return;
        }
        
        for (const auto& booking : bookings) {
            displayBooking(booking);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении бронирований: " << e.what() << std::endl;
    }
}

void TechUI::cancelBooking() {
    try {
        std::cout << "\n--- ОТМЕНА БРОНИРОВАНИЯ ---" << std::endl;
        
        UUID bookingId = InputHandlers::readUUID("Введите ID бронирования для отмены: ");
        
        auto response = managers_->getBookingService()->cancelBooking(bookingId, currentClientId_);
        
        std::cout << "✅ Бронирование отменено! Статус: " << response.status << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при отмене бронирования: " << e.what() << std::endl;
    }
}

void TechUI::handleClientLessons() {
    while (true) {
        std::cout << "\n=== ЗАНЯТИЯ ===" << std::endl;
        std::cout << "1. Расписание" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 1);
        
        switch (choice) {
            case 1: viewSchedule(); break;
            case 0: return;
        }
    }
}

void TechUI::viewSchedule() {
    try {
        std::cout << "\n--- РАСПИСАНИЕ ---" << std::endl;
        
        auto lessons = managers_->getUpcomingLessons(7);
        
        if (lessons.empty()) {
            std::cout << "Нет занятий на ближайшую неделю." << std::endl;
            return;
        }
        
        for (const auto& lesson : lessons) {
            displayLesson(lesson);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении расписания: " << e.what() << std::endl;
    }
}

void TechUI::handleClientSubscriptions() {
    while (true) {
        std::cout << "\n=== АБОНЕМЕНТЫ ===" << std::endl;
        std::cout << "1. Купить абонемент" << std::endl;
        std::cout << "2. Мои абонементы" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 2);
        
        switch (choice) {
            case 1: purchaseSubscription(); break;
            case 2: viewClientSubscriptions(); break;
            case 0: return;
        }
    }
}

void TechUI::purchaseSubscription() {
    try {
        std::cout << "\n--- ПОКУПКА АБОНЕМЕНТА ---" << std::endl;
        
        auto subscriptionTypes = managers_->getSubscriptionTypes();
        
        if (subscriptionTypes.empty()) {
            std::cout << "Нет доступных типов абонементов." << std::endl;
            return;
        }
        
        std::cout << "Доступные абонементы:" << std::endl;
        for (size_t i = 0; i < subscriptionTypes.size(); ++i) {
            const auto& type = subscriptionTypes[i];
            std::cout << (i + 1) << ". " << type.getName() << std::endl;
            std::cout << "   Описание: " << type.getDescription() << std::endl;
            std::cout << "   Срок: " << type.getValidityDays() << " дней" << std::endl;
            if (type.isUnlimited()) {
                std::cout << "   Посещений: безлимит" << std::endl;
            } else {
                std::cout << "   Посещений: " << type.getVisitCount() << std::endl;
            }
            std::cout << "   Цена: " << type.getPrice() << " руб." << std::endl;
            std::cout << "---" << std::endl;
        }
        
        int choice = InputHandlers::readInt("Выберите тип абонемента: ", 1, static_cast<int>(subscriptionTypes.size()));
        UUID subscriptionTypeId = subscriptionTypes[choice - 1].getId();
        
        SubscriptionRequestDTO request{currentClientId_, subscriptionTypeId};
        auto response = managers_->getSubscriptionService()->purchaseSubscription(request);
        
        std::cout << "✅ Абонемент приобретен! ID: " << response.subscriptionId.toString() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при покупке абонемента: " << e.what() << std::endl;
    }
}

void TechUI::viewClientSubscriptions() {
    try {
        std::cout << "\n--- МОИ АБОНЕМЕНТЫ ---" << std::endl;
        
        auto subscriptions = managers_->getSubscriptionService()->getClientSubscriptions(currentClientId_);
        
        if (subscriptions.empty()) {
            std::cout << "У вас нет активных абонементов." << std::endl;
            return;
        }
        
        for (const auto& subscription : subscriptions) {
            displaySubscription(subscription);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении абонементов: " << e.what() << std::endl;
    }
}

void TechUI::handleClientReviews() {
    while (true) {
        std::cout << "\n=== ОТЗЫВЫ ===" << std::endl;
        std::cout << "1. Оставить отзыв" << std::endl;
        std::cout << "2. Мои отзывы" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 2);
        
        switch (choice) {
            case 1: createReview(); break;
            case 2: viewClientReviews(); break;
            case 0: return;
        }
    }
}

void TechUI::createReview() {
    try {
        std::cout << "\n--- ОСТАВЛЕНИЕ ОТЗЫВА ---" << std::endl;
        
        UUID lessonId = InputHandlers::readUUID("Введите ID занятия: ");
        int rating = InputHandlers::readRating();
        std::string comment = InputHandlers::readString("Комментарий (необязательно): ", 1000);
        
        ReviewRequestDTO request{currentClientId_, lessonId, rating, comment};
        auto response = managers_->getReviewService()->createReview(request);
        
        std::cout << "✅ Отзыв отправлен на модерацию! ID: " << response.reviewId.toString() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при создании отзыва: " << e.what() << std::endl;
    }
}

void TechUI::viewClientReviews() {
    try {
        std::cout << "\n--- МОИ ОТЗЫВЫ ---" << std::endl;
        
        auto reviews = managers_->getReviewService()->getClientReviews(currentClientId_);
        
        if (reviews.empty()) {
            std::cout << "У вас нет оставленных отзывов." << std::endl;
            return;
        }
        
        for (const auto& review : reviews) {
            displayReview(review);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении отзывов: " << e.what() << std::endl;
    }
}

void TechUI::handleAdminMode() {
    handleAdminMenu();
}

void TechUI::handleAdminMenu() {
    while (true) {
        std::cout << "\n=== МЕНЮ АДМИНИСТРАТОРА ===" << std::endl;
        std::cout << "1. Управление клиентами" << std::endl;
        std::cout << "2. Управление залами" << std::endl;
        std::cout << "3. Управление бронированиями" << std::endl;
        std::cout << "4. Управление занятиями" << std::endl;
        std::cout << "5. Управление преподавателями" << std::endl;
        std::cout << "6. Управление абонементами" << std::endl;
        std::cout << "7. Модерация отзывов" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 7);
        
        switch (choice) {
            case 1: handleAdminClients(); break;
            case 2: handleAdminHalls(); break;
            case 3: handleAdminBookings(); break;
            case 4: handleAdminLessons(); break;
            case 5: handleAdminTrainers(); break;
            case 6: handleAdminSubscriptions(); break;
            case 7: handleAdminReviews(); break;
            case 0: return;
        }
    }
}

void TechUI::handleAdminClients() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ КЛИЕНТАМИ ===" << std::endl;
        std::cout << "1. Найти клиента по ID" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 1);
        
        switch (choice) {
            case 1: findClientById(); break;
            case 0: return;
        }
    }
}

void TechUI::findClientById() {
    try {
        std::cout << "\n--- ПОИСК КЛИЕНТА ПО ID ---" << std::endl;
        
        UUID clientId = InputHandlers::readUUID("Введите ID клиента: ");
        
        auto client = managers_->getClientRepo()->findById(clientId);
        if (client) {
            displayClient(*client);
        } else {
            std::cout << "❌ Клиент не найден." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при поиске клиента: " << e.what() << std::endl;
    }
}

void TechUI::handleAdminHalls() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ ЗАЛАМИ ===" << std::endl;
        std::cout << "1. Создать зал" << std::endl;
        std::cout << "2. Список всех залов" << std::endl;
        std::cout << "3. Найти зал по ID" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 3);
        
        switch (choice) {
            case 1: createHall(); break;
            case 2: listAllHalls(); break;
            case 3: findHallById(); break;
            case 0: return;
        }
    }
}

void TechUI::createHall() {
    try {
        std::cout << "\n--- СОЗДАНИЕ ЗАЛА ---" << std::endl;
        
        std::string name = InputHandlers::readString("Название зала: ");
        std::string description = InputHandlers::readString("Описание: ", 1000);
        int capacity = InputHandlers::readInt("Вместимость: ", 1, 500);
        std::string floorType = InputHandlers::readString("Тип покрытия: ");
        std::string equipment = InputHandlers::readString("Оборудование: ", 500);
        
        UUID hallId = UUID::generate();
        UUID branchId = UUID::fromString("22222222-2222-2222-2222-222222222222");
        
        DanceHall hall(hallId, name, capacity, branchId);
        hall.setDescription(description);
        hall.setFloorType(floorType);
        hall.setEquipment(equipment);
        
        bool success = managers_->getHallRepo()->save(hall);
        if (success) {
            std::cout << "✅ Зал успешно создан! ID: " << hallId.toString() << std::endl;
        } else {
            std::cout << "❌ Ошибка при создании зала" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при создании зала: " << e.what() << std::endl;
    }
}

void TechUI::listAllHalls() {
    try {
        std::cout << "\n--- ВСЕ ЗАЛЫ ---" << std::endl;
        
        auto halls = managers_->getAvailableHalls();
        
        if (halls.empty()) {
            std::cout << "Нет доступных залов." << std::endl;
            return;
        }
        
        for (const auto& hall : halls) {
            displayHall(hall);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении залов: " << e.what() << std::endl;
    }
}

void TechUI::findHallById() {
    try {
        std::cout << "\n--- ПОИСК ЗАЛА ПО ID ---" << std::endl;
        
        UUID hallId = InputHandlers::readUUID("Введите ID зала: ");
        
        auto hall = managers_->getHallRepo()->findById(hallId);
        if (hall) {
            displayHall(*hall);
        } else {
            std::cout << "❌ Зал не найден." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при поиске зала: " << e.what() << std::endl;
    }
}

void TechUI::handleAdminBookings() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ БРОНИРОВАНИЯМИ ===" << std::endl;
        std::cout << "1. Бронирования по залу" << std::endl;
        std::cout << "2. Бронирования по клиенту" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 2);
        
        switch (choice) {
            case 1: viewHallBookings(); break;
            case 2: viewClientBookingsAdmin(); break;
            case 0: return;
        }
    }
}

void TechUI::viewHallBookings() {
    try {
        std::cout << "\n--- БРОНИРОВАНИЯ ПО ЗАЛУ ---" << std::endl;
        
        UUID hallId = InputHandlers::readUUID("Введите ID зала: ");
        
        auto bookings = managers_->getBookingService()->getDanceHallBookings(hallId);
        
        if (bookings.empty()) {
            std::cout << "Нет бронирований для этого зала." << std::endl;
            return;
        }
        
        for (const auto& booking : bookings) {
            displayBooking(booking);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении бронирований: " << e.what() << std::endl;
    }
}

void TechUI::viewClientBookingsAdmin() {
    try {
        std::cout << "\n--- БРОНИРОВАНИЯ КЛИЕНТА ---" << std::endl;
        
        UUID clientId = InputHandlers::readUUID("Введите ID клиента: ");
        
        auto bookings = managers_->getBookingService()->getClientBookings(clientId);
        
        if (bookings.empty()) {
            std::cout << "У клиента нет бронирований." << std::endl;
            return;
        }
        
        for (const auto& booking : bookings) {
            displayBooking(booking);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении бронирований: " << e.what() << std::endl;
    }
}

void TechUI::handleAdminLessons() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ ЗАНЯТИЯМИ ===" << std::endl;
        std::cout << "1. Создать занятие" << std::endl;
        std::cout << "2. Все занятия" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 2);
        
        switch (choice) {
            case 1: createLesson(); break;
            case 2: listAllLessons(); break;
            case 0: return;
        }
    }
}

void TechUI::createLesson() {
    try {
        std::cout << "\n--- СОЗДАНИЕ ЗАНЯТИЯ ---" << std::endl;
        
        auto trainers = managers_->getActiveTrainers();
        if (trainers.empty()) {
            std::cout << "❌ Нет доступных преподавателей." << std::endl;
            return;
        }
        
        std::cout << "Доступные преподаватели:" << std::endl;
        for (const auto& trainer : trainers) {
            std::cout << "- " << trainer.getName() << " (ID: " << trainer.getId().toString() << ")" << std::endl;
        }
        
        auto halls = managers_->getAvailableHalls();
        if (halls.empty()) {
            std::cout << "❌ Нет доступных залов." << std::endl;
            return;
        }
        
        std::cout << "Доступные залы:" << std::endl;
        for (const auto& hall : halls) {
            std::cout << "- " << hall.getName() << " (ID: " << hall.getId().toString() 
                      << ", Вместимость: " << hall.getCapacity() << ")" << std::endl;
        }
        
        std::string name = InputHandlers::readString("Название занятия: ");
        std::string description = InputHandlers::readString("Описание: ", 1000);
        LessonType type = InputHandlers::readLessonType();
        DifficultyLevel difficulty = InputHandlers::readDifficultyLevel();
        auto startTime = InputHandlers::readDateTime("Время начала занятия");
        int duration = InputHandlers::readInt("Продолжительность (минут): ", 30, 240);
        int maxParticipants = InputHandlers::readInt("Макс. участников: ", 1, 100);
        double price = InputHandlers::readDouble("Цена: ", 0.0);
        UUID trainerId = InputHandlers::readUUID("ID преподавателя: ");
        UUID hallId = InputHandlers::readUUID("ID зала: ");
        
        LessonRequestDTO request;
        request.trainerId = trainerId;
        request.hallId = hallId;
        request.startTime = startTime;
        request.durationMinutes = duration;
        request.type = type;
        request.name = name;
        request.description = description;
        request.difficulty = difficulty;
        request.maxParticipants = maxParticipants;
        request.price = price;
        
        auto response = managers_->getLessonService()->createLesson(request);
        
        std::cout << "✅ Занятие создано! ID: " << response.lessonId.toString() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при создании занятия: " << e.what() << std::endl;
    }
}

void TechUI::listAllLessons() {
    try {
        std::cout << "\n--- ВСЕ ЗАНЯТИЯ ---" << std::endl;
        
        auto lessons = managers_->getUpcomingLessons(30);
        
        if (lessons.empty()) {
            std::cout << "Нет занятий на ближайший месяц." << std::endl;
            return;
        }
        
        for (const auto& lesson : lessons) {
            displayLesson(lesson);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении занятий: " << e.what() << std::endl;
    }
}

void TechUI::handleAdminTrainers() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ ПРЕПОДАВАТЕЛЯМИ ===" << std::endl;
        std::cout << "1. Создать преподавателя" << std::endl;
        std::cout << "2. Все преподаватели" << std::endl;
        std::cout << "3. Найти преподавателя по ID" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 3);
        
        switch (choice) {
            case 1: createTrainer(); break;
            case 2: listAllTrainers(); break;
            case 3: findTrainerById(); break;
            case 0: return;
        }
    }
}

void TechUI::createTrainer() {
    try {
        std::cout << "\n--- СОЗДАНИЕ ПРЕПОДАВАТЕЛЯ ---" << std::endl;
        
        std::string name = InputHandlers::readString("Имя преподавателя: ");
        std::string biography = InputHandlers::readString("Биография: ", 2000);
        std::string qualification = InputHandlers::readString("Уровень квалификации: ");
        
        std::vector<std::string> specializations;
        std::cout << "Введите специализации (пустая строка для завершения):" << std::endl;
        while (true) {
            std::string spec = InputHandlers::readString("Специализация: ");
            if (spec.empty()) break;
            specializations.push_back(spec);
        }
        
        UUID trainerId = UUID::generate();
        Trainer trainer(trainerId, name, specializations);
        trainer.setBiography(biography);
        trainer.setQualificationLevel(qualification);
        trainer.setActive(true);
        
        bool success = managers_->getTrainerRepo()->save(trainer);
        if (success) {
            std::cout << "✅ Преподаватель создан! ID: " << trainerId.toString() << std::endl;
        } else {
            std::cout << "❌ Ошибка при создании преподавателя" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при создании преподавателя: " << e.what() << std::endl;
    }
}

void TechUI::listAllTrainers() {
    try {
        std::cout << "\n--- ВСЕ ПРЕПОДАВАТЕЛИ ---" << std::endl;
        
        auto trainers = managers_->getActiveTrainers();
        
        if (trainers.empty()) {
            std::cout << "Нет преподавателей." << std::endl;
            return;
        }
        
        for (const auto& trainer : trainers) {
            displayTrainer(trainer);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении преподавателей: " << e.what() << std::endl;
    }
}

void TechUI::findTrainerById() {
    try {
        std::cout << "\n--- ПОИСК ПРЕПОДАВАТЕЛЯ ПО ID ---" << std::endl;
        
        UUID trainerId = InputHandlers::readUUID("Введите ID преподавателя: ");
        
        auto trainer = managers_->getTrainerRepo()->findById(trainerId);
        if (trainer) {
            displayTrainer(*trainer);
        } else {
            std::cout << "❌ Преподаватель не найден." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при поиске преподавателя: " << e.what() << std::endl;
    }
}

void TechUI::handleAdminSubscriptions() {
    while (true) {
        std::cout << "\n=== УПРАВЛЕНИЕ АБОНЕМЕНТАМИ ===" << std::endl;
        std::cout << "1. Абонементы клиента" << std::endl;
        std::cout << "2. Типы абонементов" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 2);
        
        switch (choice) {
            case 1: viewClientSubscriptionsAdmin(); break;
            case 2: viewSubscriptionTypes(); break;
            case 0: return;
        }
    }
}

void TechUI::viewClientSubscriptionsAdmin() {
    try {
        std::cout << "\n--- АБОНЕМЕНТЫ КЛИЕНТА ---" << std::endl;
        
        UUID clientId = InputHandlers::readUUID("Введите ID клиента: ");
        
        auto subscriptions = managers_->getSubscriptionService()->getClientSubscriptions(clientId);
        
        if (subscriptions.empty()) {
            std::cout << "У клиента нет абонементов." << std::endl;
            return;
        }
        
        for (const auto& subscription : subscriptions) {
            displaySubscription(subscription);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении абонементов: " << e.what() << std::endl;
    }
}

void TechUI::viewSubscriptionTypes() {
    try {
        std::cout << "\n--- ТИПЫ АБОНЕМЕНТОВ ---" << std::endl;
        
        auto types = managers_->getSubscriptionTypes();
        
        if (types.empty()) {
            std::cout << "Нет доступных типов абонементов." << std::endl;
            return;
        }
        
        for (const auto& type : types) {
            std::cout << "🎫 " << type.getName() << " (ID: " << type.getId().toString() << ")" << std::endl;
            std::cout << "   Описание: " << type.getDescription() << std::endl;
            std::cout << "   Срок: " << type.getValidityDays() << " дней" << std::endl;
            if (type.isUnlimited()) {
                std::cout << "   Посещений: безлимит" << std::endl;
            } else {
                std::cout << "   Посещений: " << type.getVisitCount() << std::endl;
            }
            std::cout << "   Цена: " << type.getPrice() << " руб." << std::endl;
            std::cout << "---" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении типов абонементов: " << e.what() << std::endl;
    }
}

void TechUI::handleAdminReviews() {
    while (true) {
        std::cout << "\n=== МОДЕРАЦИЯ ОТЗЫВОВ ===" << std::endl;
        std::cout << "1. Отзывы на модерации" << std::endl;
        std::cout << "2. Одобрить отзыв" << std::endl;
        std::cout << "3. Отклонить отзыв" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 3);
        
        switch (choice) {
            case 1: viewPendingReviews(); break;
            case 2: approveReview(); break;
            case 3: rejectReview(); break;
            case 0: return;
        }
    }
}

void TechUI::viewPendingReviews() {
    try {
        std::cout << "\n--- ОТЗЫВЫ НА МОДЕРАЦИИ ---" << std::endl;
        
        auto reviews = managers_->getReviewService()->getPendingReviews();
        
        if (reviews.empty()) {
            std::cout << "Нет отзывов на модерации." << std::endl;
            return;
        }
        
        for (const auto& review : reviews) {
            displayReview(review);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении отзывов: " << e.what() << std::endl;
    }
}

void TechUI::approveReview() {
    try {
        std::cout << "\n--- ОДОБРЕНИЕ ОТЗЫВА ---" << std::endl;
        
        UUID reviewId = InputHandlers::readUUID("Введите ID отзыва для одобрения: ");
        
        auto response = managers_->getReviewService()->approveReview(reviewId);
        
        std::cout << "✅ Отзыв одобрен! Статус: " << response.status << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при одобрении отзыва: " << e.what() << std::endl;
    }
}

void TechUI::rejectReview() {
    try {
        std::cout << "\n--- ОТКЛОНЕНИЕ ОТЗЫВА ---" << std::endl;
        
        UUID reviewId = InputHandlers::readUUID("Введите ID отзыва для отклонения: ");
        
        auto response = managers_->getReviewService()->rejectReview(reviewId);
        
        std::cout << "✅ Отзыв отклонен! Статус: " << response.status << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при отклонении отзыва: " << e.what() << std::endl;
    }
}

// Вспомогательные методы отображения
void TechUI::displayBooking(const BookingResponseDTO& booking) {
    std::cout << "📅 БРОНИРОВАНИЕ " << booking.bookingId.toString() << std::endl;
    std::cout << "   Зал: " << booking.hallId.toString() << std::endl;
    std::cout << "   Время: " << booking.timeSlot.toString() << std::endl;
    std::cout << "   Статус: " << booking.status << std::endl;
    std::cout << "   Цель: " << booking.purpose << std::endl;
    std::cout << "   Создано: " << booking.createdAt << std::endl;
    std::cout << "---" << std::endl;
}

void TechUI::displayLesson(const Lesson& lesson) {
    auto time_t = std::chrono::system_clock::to_time_t(lesson.getStartTime());
    std::tm* tm = std::localtime(&time_t);
    
    std::cout << "🎓 " << lesson.getName() << " (ID: " << lesson.getId().toString() << ")" << std::endl;
    std::cout << "   Тип: " << EnumUtils::lessonTypeToString(lesson.getType()) << std::endl;
    std::cout << "   Время: " << std::put_time(tm, "%d.%m.%Y %H:%M") << std::endl;
    std::cout << "   Продолжительность: " << lesson.getDurationMinutes() << " мин." << std::endl;
    std::cout << "   Уровень: " << EnumUtils::difficultyLevelToString(lesson.getDifficulty()) << std::endl;
    std::cout << "   Места: " << lesson.getCurrentParticipants() << "/" << lesson.getMaxParticipants() << std::endl;
    std::cout << "   Цена: " << lesson.getPrice() << " руб." << std::endl;
    std::cout << "   Статус: " << EnumUtils::lessonStatusToString(lesson.getStatus()) << std::endl;
    std::cout << "---" << std::endl;
}

void TechUI::displaySubscription(const SubscriptionResponseDTO& subscription) {
    std::cout << "🎫 АБОНЕМЕНТ " << subscription.subscriptionId.toString() << std::endl;
    std::cout << "   Тип: " << subscription.subscriptionTypeId.toString() << std::endl;
    std::cout << "   Период: " << subscription.startDate << " - " << subscription.endDate << std::endl;
    std::cout << "   Осталось посещений: " << subscription.remainingVisits << std::endl;
    std::cout << "   Статус: " << subscription.status << std::endl;
    std::cout << "   Куплен: " << subscription.purchaseDate << std::endl;
    std::cout << "---" << std::endl;
}

void TechUI::displayReview(const ReviewResponseDTO& review) {
    std::cout << "⭐ ОТЗЫВ " << review.reviewId.toString() << std::endl;
    std::cout << "   Занятие: " << review.lessonId.toString() << std::endl;
    std::cout << "   Рейтинг: " << review.rating << "/5" << std::endl;
    if (!review.comment.empty()) {
        std::cout << "   Комментарий: " << review.comment << std::endl;
    }
    std::cout << "   Дата: " << review.publicationDate << std::endl;
    std::cout << "   Статус: " << review.status << std::endl;
    std::cout << "---" << std::endl;
}

void TechUI::displayClient(const Client& client) {
    std::cout << "👤 КЛИЕНТ " << client.getId().toString() << std::endl;
    std::cout << "   Имя: " << client.getName() << std::endl;
    std::cout << "   Email: " << client.getEmail() << std::endl;
    std::cout << "   Телефон: " << client.getPhone() << std::endl;
    std::cout << "   Статус: " << EnumUtils::accountStatusToString(client.getStatus()) << std::endl;
    std::cout << "---" << std::endl;
}

void TechUI::displayHall(const DanceHall& hall) {
    std::cout << "🏟️ ЗАЛ " << hall.getName() << " (ID: " << hall.getId().toString() << ")" << std::endl;
    std::cout << "   Вместимость: " << hall.getCapacity() << " чел." << std::endl;
    std::cout << "   Покрытие: " << hall.getFloorType() << std::endl;
    std::cout << "   Оборудование: " << hall.getEquipment() << std::endl;
    std::cout << "   Описание: " << hall.getDescription() << std::endl;
    std::cout << "---" << std::endl;
}

void TechUI::displayTrainer(const Trainer& trainer) {
    std::cout << "👨‍🏫 ПРЕПОДАВАТЕЛЬ " << trainer.getName() << " (ID: " << trainer.getId().toString() << ")" << std::endl;
    std::cout << "   Уровень квалификации: " << trainer.getQualificationLevel() << std::endl;
    std::cout << "   Статус: " << (trainer.isActive() ? "Активен" : "Неактивен") << std::endl;
    
    auto specializations = trainer.getSpecializations();
    if (!specializations.empty()) {
        std::cout << "   Специализации: ";
        for (size_t i = 0; i < specializations.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << specializations[i];
        }
        std::cout << std::endl;
    }
    
    if (!trainer.getBiography().empty()) {
        std::cout << "   Биография: " << trainer.getBiography() << std::endl;
    }
    std::cout << "---" << std::endl;
}