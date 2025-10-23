#include "AdminDashboard.hpp"
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WCssStyleSheet.h>

AdminDashboard::AdminDashboard(WebApplication* app) 
    : app_(app),
      dashboardView_(nullptr),
      clientsView_(nullptr),
      bookingsView_(nullptr),
      lessonsView_(nullptr),
      hallsView_(nullptr),
      trainersView_(nullptr),
      subscriptionsView_(nullptr),
      reviewsView_(nullptr) {
    
    setupUI();
}

void AdminDashboard::setupUI() {
    setStyleClass("admin-dashboard");
    
    // Заголовок
    auto titleContainer = addNew<Wt::WContainerWidget>();
    titleContainer->setStyleClass("dashboard-header");
    
    auto title = titleContainer->addNew<Wt::WText>("<h1>🛠️ Admin Panel</h1>");
    title->setStyleClass("dashboard-title");
    
    auto subtitle = titleContainer->addNew<Wt::WText>(
        "<p class='dashboard-subtitle'>Dance Studio Management System</p>"
    );
    
    // Контейнер для меню и контента
    auto contentContainer = addNew<Wt::WContainerWidget>();
    contentContainer->setStyleClass("admin-content-container");
    
    // Меню администратора
    auto menuContainer = contentContainer->addNew<Wt::WContainerWidget>();
    menuContainer->setStyleClass("admin-menu-container");
    
    adminMenu_ = menuContainer->addNew<Wt::WMenu>();
    adminMenu_->setStyleClass("nav nav-pills flex-column");
    
    // Стек для контента
    contentStack_ = contentContainer->addNew<Wt::WStackedWidget>();
    contentStack_->setStyleClass("admin-content-stack");
    
    // Создаем все представления
    createViews();
    setupAdminMenu();
    
    // Показываем панель управления по умолчанию
    showDashboard();
}

void AdminDashboard::refresh() {
    std::cout << "🔄 AdminDashboard::refresh() called" << std::endl;
    // Принудительно обновляем отображение
    if (app_ && app_->getUserSession()) {
        std::cout << "✅ Refreshing admin dashboard for user: " << app_->getUserSession()->getUserName() << std::endl;
    }
}

void AdminDashboard::createViews() {
    dashboardView_ = contentStack_->addNew<Wt::WContainerWidget>();
    clientsView_ = contentStack_->addNew<Wt::WContainerWidget>();
    bookingsView_ = contentStack_->addNew<Wt::WContainerWidget>();
    lessonsView_ = contentStack_->addNew<Wt::WContainerWidget>();
    hallsView_ = contentStack_->addNew<Wt::WContainerWidget>();
    trainersView_ = contentStack_->addNew<Wt::WContainerWidget>();
    subscriptionsView_ = contentStack_->addNew<Wt::WContainerWidget>();
    reviewsView_ = contentStack_->addNew<Wt::WContainerWidget>();
    
    // Инициализируем представления
    setupDashboardView();
    setupClientsView();
    setupBookingsView();
    setupLessonsView();
    setupHallsView();
    setupTrainersView();
    setupSubscriptionsView();
    setupReviewsView();
}

void AdminDashboard::setupAdminMenu() {
    // Добавляем пункты меню (используем английский текст для избежания проблем с кодировкой)
    auto dashboardItem = adminMenu_->addItem("📊 Dashboard", std::make_unique<Wt::WText>(""));
    auto clientsItem = adminMenu_->addItem("👥 Clients", std::make_unique<Wt::WText>(""));
    auto bookingsItem = adminMenu_->addItem("🏟️ Bookings", std::make_unique<Wt::WText>(""));
    auto lessonsItem = adminMenu_->addItem("🎓 Lessons", std::make_unique<Wt::WText>(""));
    auto hallsItem = adminMenu_->addItem("💃 Halls", std::make_unique<Wt::WText>(""));
    auto trainersItem = adminMenu_->addItem("👨‍🏫 Trainers", std::make_unique<Wt::WText>(""));
    auto subscriptionsItem = adminMenu_->addItem("🎫 Subscriptions", std::make_unique<Wt::WText>(""));
    auto reviewsItem = adminMenu_->addItem("⭐ Reviews", std::make_unique<Wt::WText>(""));
    
    // Обработчики для меню
    dashboardItem->clicked().connect([this]() { showDashboard(); });
    clientsItem->clicked().connect([this]() { showClients(); });
    bookingsItem->clicked().connect([this]() { showBookings(); });
    lessonsItem->clicked().connect([this]() { showLessons(); });
    hallsItem->clicked().connect([this]() { showHalls(); });
    trainersItem->clicked().connect([this]() { showTrainers(); });
    subscriptionsItem->clicked().connect([this]() { showSubscriptions(); });
    reviewsItem->clicked().connect([this]() { showReviews(); });
}

void AdminDashboard::setupDashboardView() {
    dashboardView_->setStyleClass("dashboard-view");
    
    auto welcomeCard = dashboardView_->addNew<Wt::WContainerWidget>();
    welcomeCard->setStyleClass("card welcome-card");
    
    auto cardBody = welcomeCard->addNew<Wt::WContainerWidget>();
    cardBody->setStyleClass("card-body");
    
    cardBody->addNew<Wt::WText>(
        "<h2>Welcome to Admin Panel!</h2>"
        "<p class='lead'>Manage all aspects of the dance studio here.</p>"
    );
    
    // Статистика
    auto statsContainer = dashboardView_->addNew<Wt::WContainerWidget>();
    statsContainer->setStyleClass("stats-container");
    
    auto statsRow = statsContainer->addNew<Wt::WContainerWidget>();
    statsRow->setStyleClass("row stats-row");
    
    // Статистика на английском
    std::vector<std::pair<std::string, std::string>> stats = {
        {"👥", "Clients: 156"},
        {"🏟️", "Bookings: 23"},
        {"🎓", "Lessons: 45"},
        {"⭐", "Reviews: 12"}
    };
    
    for (const auto& stat : stats) {
        auto statCard = statsRow->addNew<Wt::WContainerWidget>();
        statCard->setStyleClass("col-md-3 stat-card");
        
        auto icon = statCard->addNew<Wt::WText>("<div class='stat-icon'>" + stat.first + "</div>");
        auto value = statCard->addNew<Wt::WText>("<div class='stat-value'>" + stat.second + "</div>");
    }
    
    // Быстрые действия
    auto actionsContainer = dashboardView_->addNew<Wt::WContainerWidget>();
    actionsContainer->setStyleClass("quick-actions-container");
    
    auto actionsTitle = actionsContainer->addNew<Wt::WText>("<h3>Quick Actions</h3>");
    
    auto actionsRow = actionsContainer->addNew<Wt::WContainerWidget>();
    actionsRow->setStyleClass("quick-actions-row");
    
    std::vector<std::pair<std::string, std::string>> actions = {
        {"➕ Create Lesson", "showLessons"},
        {"➕ Add Hall", "showHalls"},
        {"➕ New Trainer", "showTrainers"},
        {"👁️ Moderate Reviews", "showReviews"}
    };
    
    for (const auto& action : actions) {
        auto actionBtn = actionsRow->addNew<Wt::WPushButton>(action.first);
        actionBtn->setStyleClass("btn btn-outline-primary quick-action-btn");
        actionBtn->clicked().connect([this, action]() {
            if (action.second == "showLessons") showLessons();
            else if (action.second == "showHalls") showHalls();
            else if (action.second == "showTrainers") showTrainers();
            else if (action.second == "showReviews") showReviews();
        });
    }
}

void AdminDashboard::setupClientsView() {
    clientsView_->setStyleClass("admin-section");
    
    auto header = clientsView_->addNew<Wt::WContainerWidget>();
    header->setStyleClass("section-header");
    
    header->addNew<Wt::WText>("<h2>👥 Client Management</h2>");
    
    // Поиск и фильтры
    auto filtersContainer = clientsView_->addNew<Wt::WContainerWidget>();
    filtersContainer->setStyleClass("filters-container");
    
    auto searchContainer = filtersContainer->addNew<Wt::WContainerWidget>();
    searchContainer->setStyleClass("form-group");
    
    auto searchLabel = searchContainer->addNew<Wt::WLabel>("Search client:");
    clientSearchEdit_ = searchContainer->addNew<Wt::WLineEdit>();
    clientSearchEdit_->setPlaceholderText("Enter email or client name...");
    clientSearchEdit_->setStyleClass("form-control");
    
    auto searchBtn = searchContainer->addNew<Wt::WPushButton>("🔍 Search");
    searchBtn->setStyleClass("btn btn-primary");
    searchBtn->clicked().connect(this, &AdminDashboard::handleClientSearch);
    
    // Таблица клиентов
    clientsTable_ = clientsView_->addNew<Wt::WTable>();
    clientsTable_->setStyleClass("table table-striped admin-table");
    clientsTable_->setHeaderCount(1);
    
    // Заголовки таблицы
    std::vector<std::string> headers = {"ID", "Name", "Email", "Phone", "Registration Date", "Status", "Actions"};
    setupTableHeaders(clientsTable_, headers);
    
    // Загружаем данные
    loadClientsData();
}

void AdminDashboard::setupBookingsView() {
    bookingsView_->setStyleClass("admin-section");
    
    auto header = bookingsView_->addNew<Wt::WContainerWidget>();
    header->setStyleClass("section-header");
    
    header->addNew<Wt::WText>("<h2>🏟️ Booking Management</h2>");
    
    // Фильтры
    auto filtersContainer = bookingsView_->addNew<Wt::WContainerWidget>();
    filtersContainer->setStyleClass("filters-container");
    
    auto filterContainer = filtersContainer->addNew<Wt::WContainerWidget>();
    filterContainer->setStyleClass("form-group");
    
    auto filterLabel = filterContainer->addNew<Wt::WLabel>("Booking status:");
    bookingFilterComboBox_ = filterContainer->addNew<Wt::WComboBox>();
    bookingFilterComboBox_->addItem("All bookings");
    bookingFilterComboBox_->addItem("Confirmed");
    bookingFilterComboBox_->addItem("Pending");
    bookingFilterComboBox_->addItem("Cancelled");
    bookingFilterComboBox_->setStyleClass("form-control");
    
    auto filterBtn = filterContainer->addNew<Wt::WPushButton>("🔍 Apply filter");
    filterBtn->setStyleClass("btn btn-primary");
    filterBtn->clicked().connect(this, &AdminDashboard::handleBookingFilter);
    
    // Таблица бронирований
    bookingsTable_ = bookingsView_->addNew<Wt::WTable>();
    bookingsTable_->setStyleClass("table table-striped admin-table");
    bookingsTable_->setHeaderCount(1);
    
    std::vector<std::string> headers = {"ID", "Client", "Hall", "Time", "Purpose", "Status", "Actions"};
    setupTableHeaders(bookingsTable_, headers);
    
    loadBookingsData();
}

void AdminDashboard::setupLessonsView() {
    lessonsView_->setStyleClass("admin-section");
    
    auto header = lessonsView_->addNew<Wt::WContainerWidget>();
    header->setStyleClass("section-header");
    
    header->addNew<Wt::WText>("<h2>🎓 Lesson Management</h2>");
    
    // Форма создания занятия
    auto createForm = lessonsView_->addNew<Wt::WContainerWidget>();
    createForm->setStyleClass("card creation-form");
    
    auto formHeader = createForm->addNew<Wt::WContainerWidget>();
    formHeader->setStyleClass("card-header");
    formHeader->addNew<Wt::WText>("<h4>➕ Create New Lesson</h4>");
    
    auto formBody = createForm->addNew<Wt::WContainerWidget>();
    formBody->setStyleClass("card-body");
    
    // Поля формы
    auto nameContainer = formBody->addNew<Wt::WContainerWidget>();
    nameContainer->setStyleClass("form-group");
    nameContainer->addNew<Wt::WLabel>("Lesson name:");
    auto nameEdit = nameContainer->addNew<Wt::WLineEdit>();
    nameEdit->setPlaceholderText("Enter lesson name");
    nameEdit->setStyleClass("form-control");
    
    auto dateContainer = formBody->addNew<Wt::WContainerWidget>();
    dateContainer->setStyleClass("form-group");
    dateContainer->addNew<Wt::WLabel>("Date:");
    lessonDatePicker_ = dateContainer->addNew<Wt::WDatePicker>();
    lessonDatePicker_->setDate(Wt::WDate::currentDate());
    
    auto createBtn = formBody->addNew<Wt::WPushButton>("✅ Create Lesson");
    createBtn->setStyleClass("btn btn-success");
    createBtn->clicked().connect(this, &AdminDashboard::handleCreateLesson);
    
    // Таблица занятий
    lessonsTable_ = lessonsView_->addNew<Wt::WTable>();
    lessonsTable_->setStyleClass("table table-striped admin-table");
    lessonsTable_->setHeaderCount(1);
    
    std::vector<std::string> headers = {"ID", "Name", "Trainer", "Hall", "Time", "Participants", "Status", "Actions"};
    setupTableHeaders(lessonsTable_, headers);
    
    loadLessonsData();
}

void AdminDashboard::setupHallsView() {
    hallsView_->setStyleClass("admin-section");
    
    auto header = hallsView_->addNew<Wt::WContainerWidget>();
    header->setStyleClass("section-header");
    
    header->addNew<Wt::WText>("<h2>💃 Hall Management</h2>");
    
    // Форма создания зала
    auto createForm = hallsView_->addNew<Wt::WContainerWidget>();
    createForm->setStyleClass("card creation-form");
    
    auto formHeader = createForm->addNew<Wt::WContainerWidget>();
    formHeader->setStyleClass("card-header");
    formHeader->addNew<Wt::WText>("<h4>➕ Add New Hall</h4>");
    
    auto formBody = createForm->addNew<Wt::WContainerWidget>();
    formBody->setStyleClass("card-body");
    
    auto nameContainer = formBody->addNew<Wt::WContainerWidget>();
    nameContainer->setStyleClass("form-group");
    nameContainer->addNew<Wt::WLabel>("Hall name:");
    hallNameEdit_ = nameContainer->addNew<Wt::WLineEdit>();
    hallNameEdit_->setPlaceholderText("Enter hall name");
    hallNameEdit_->setStyleClass("form-control");
    
    auto createBtn = formBody->addNew<Wt::WPushButton>("✅ Add Hall");
    createBtn->setStyleClass("btn btn-success");
    createBtn->clicked().connect(this, &AdminDashboard::handleCreateHall);
    
    // Таблица залов
    hallsTable_ = hallsView_->addNew<Wt::WTable>();
    hallsTable_->setStyleClass("table table-striped admin-table");
    hallsTable_->setHeaderCount(1);
    
    std::vector<std::string> headers = {"ID", "Name", "Branch", "Capacity", "Equipment", "Status", "Actions"};
    setupTableHeaders(hallsTable_, headers);
    
    loadHallsData();
}

void AdminDashboard::setupTrainersView() {
    trainersView_->setStyleClass("admin-section");
    
    auto header = trainersView_->addNew<Wt::WContainerWidget>();
    header->setStyleClass("section-header");
    
    header->addNew<Wt::WText>("<h2>👨‍🏫 Trainer Management</h2>");
    
    // Форма создания преподавателя
    auto createForm = trainersView_->addNew<Wt::WContainerWidget>();
    createForm->setStyleClass("card creation-form");
    
    auto formHeader = createForm->addNew<Wt::WContainerWidget>();
    formHeader->setStyleClass("card-header");
    formHeader->addNew<Wt::WText>("<h4>➕ Add Trainer</h4>");
    
    auto formBody = createForm->addNew<Wt::WContainerWidget>();
    formBody->setStyleClass("card-body");
    
    auto nameContainer = formBody->addNew<Wt::WContainerWidget>();
    nameContainer->setStyleClass("form-group");
    nameContainer->addNew<Wt::WLabel>("Trainer name:");
    trainerNameEdit_ = nameContainer->addNew<Wt::WLineEdit>();
    trainerNameEdit_->setPlaceholderText("Enter trainer full name");
    trainerNameEdit_->setStyleClass("form-control");
    
    auto createBtn = formBody->addNew<Wt::WPushButton>("✅ Add Trainer");
    createBtn->setStyleClass("btn btn-success");
    createBtn->clicked().connect(this, &AdminDashboard::handleCreateTrainer);
    
    // Таблица преподавателей
    trainersTable_ = trainersView_->addNew<Wt::WTable>();
    trainersTable_->setStyleClass("table table-striped admin-table");
    trainersTable_->setHeaderCount(1);
    
    std::vector<std::string> headers = {"ID", "Name", "Specialization", "Qualification", "Status", "Actions"};
    setupTableHeaders(trainersTable_, headers);
    
    loadTrainersData();
}

void AdminDashboard::setupSubscriptionsView() {
    subscriptionsView_->setStyleClass("admin-section");
    
    auto header = subscriptionsView_->addNew<Wt::WContainerWidget>();
    header->setStyleClass("section-header");
    
    header->addNew<Wt::WText>("<h2>🎫 Subscription Management</h2>");
    
    // Форма создания типа абонемента
    auto createForm = subscriptionsView_->addNew<Wt::WContainerWidget>();
    createForm->setStyleClass("card creation-form");
    
    auto formHeader = createForm->addNew<Wt::WContainerWidget>();
    formHeader->setStyleClass("card-header");
    formHeader->addNew<Wt::WText>("<h4>➕ Create Subscription Type</h4>");
    
    auto formBody = createForm->addNew<Wt::WContainerWidget>();
    formBody->setStyleClass("card-body");
    
    auto typeContainer = formBody->addNew<Wt::WContainerWidget>();
    typeContainer->setStyleClass("form-group");
    typeContainer->addNew<Wt::WLabel>("Subscription type:");
    subscriptionTypeComboBox_ = typeContainer->addNew<Wt::WComboBox>();
    subscriptionTypeComboBox_->addItem("Basic");
    subscriptionTypeComboBox_->addItem("Standard");
    subscriptionTypeComboBox_->addItem("Premium");
    subscriptionTypeComboBox_->setStyleClass("form-control");
    
    auto createBtn = formBody->addNew<Wt::WPushButton>("✅ Create Type");
    createBtn->setStyleClass("btn btn-success");
    createBtn->clicked().connect(this, &AdminDashboard::handleCreateSubscriptionType);
    
    // Таблица типов абонементов
    subscriptionsTable_ = subscriptionsView_->addNew<Wt::WTable>();
    subscriptionsTable_->setStyleClass("table table-striped admin-table");
    subscriptionsTable_->setHeaderCount(1);
    
    std::vector<std::string> headers = {"ID", "Name", "Validity", "Visits", "Price", "Status", "Actions"};
    setupTableHeaders(subscriptionsTable_, headers);
    
    loadSubscriptionsData();
}

void AdminDashboard::setupReviewsView() {
    reviewsView_->setStyleClass("admin-section");
    
    auto header = reviewsView_->addNew<Wt::WContainerWidget>();
    header->setStyleClass("section-header");
    
    header->addNew<Wt::WText>("<h2>⭐ Review Moderation</h2>");
    
    // Фильтры
    auto filtersContainer = reviewsView_->addNew<Wt::WContainerWidget>();
    filtersContainer->setStyleClass("filters-container");
    
    auto filterContainer = filtersContainer->addNew<Wt::WContainerWidget>();
    filterContainer->setStyleClass("form-group");
    
    auto filterLabel = filterContainer->addNew<Wt::WLabel>("Review status:");
    reviewStatusComboBox_ = filterContainer->addNew<Wt::WComboBox>();
    reviewStatusComboBox_->addItem("All reviews");
    reviewStatusComboBox_->addItem("Pending");
    reviewStatusComboBox_->addItem("Approved");
    reviewStatusComboBox_->addItem("Rejected");
    reviewStatusComboBox_->setStyleClass("form-control");
    
    // Таблица отзывов
    reviewsTable_ = reviewsView_->addNew<Wt::WTable>();
    reviewsTable_->setStyleClass("table table-striped admin-table");
    reviewsTable_->setHeaderCount(1);
    
    std::vector<std::string> headers = {"ID", "Client", "Lesson", "Rating", "Comment", "Date", "Status", "Actions"};
    setupTableHeaders(reviewsTable_, headers);
    
    loadReviewsData();
}

// Navigation methods
void AdminDashboard::showDashboard() {
    contentStack_->setCurrentWidget(dashboardView_);
}

void AdminDashboard::showClients() {
    contentStack_->setCurrentWidget(clientsView_);
    loadClientsData();
}

void AdminDashboard::showBookings() {
    contentStack_->setCurrentWidget(bookingsView_);
    loadBookingsData();
}

void AdminDashboard::showLessons() {
    contentStack_->setCurrentWidget(lessonsView_);
    loadLessonsData();
}

void AdminDashboard::showHalls() {
    contentStack_->setCurrentWidget(hallsView_);
    loadHallsData();
}

void AdminDashboard::showTrainers() {
    contentStack_->setCurrentWidget(trainersView_);
    loadTrainersData();
}

void AdminDashboard::showSubscriptions() {
    contentStack_->setCurrentWidget(subscriptionsView_);
    loadSubscriptionsData();
}

void AdminDashboard::showReviews() {
    contentStack_->setCurrentWidget(reviewsView_);
    loadReviewsData();
}

// Data loading methods
void AdminDashboard::loadClientsData() {
    // Используем структуру вместо кортежа для избежания проблем
    struct ClientData {
        std::string id;
        std::string name;
        std::string email;
        std::string phone;
        std::string regDate;
        std::string status;
    };
    
    std::vector<ClientData> testClients = {
        {"CL001", "Ivan Ivanov", "ivan@mail.com", "+7 (123) 456-7890", "2023-01-15", "Active"},
        {"CL002", "Maria Petrova", "maria@mail.com", "+7 (123) 456-7891", "2023-02-20", "Active"},
        {"CL003", "Alexey Sidorov", "alex@mail.com", "+7 (123) 456-7892", "2023-03-10", "Inactive"}
    };
    
    int row = 1;
    for (const auto& client : testClients) {
        clientsTable_->elementAt(row, 0)->addNew<Wt::WText>(client.id);
        clientsTable_->elementAt(row, 1)->addNew<Wt::WText>(client.name);
        clientsTable_->elementAt(row, 2)->addNew<Wt::WText>(client.email);
        clientsTable_->elementAt(row, 3)->addNew<Wt::WText>(client.phone);
        clientsTable_->elementAt(row, 4)->addNew<Wt::WText>(client.regDate);
        
        auto statusText = clientsTable_->elementAt(row, 5)->addNew<Wt::WText>(client.status);
        if (client.status == "Active") {
            statusText->setStyleClass("status-active");
        } else {
            statusText->setStyleClass("status-inactive");
        }
        
        // Кнопки действий
        auto actionsContainer = clientsTable_->elementAt(row, 6)->addNew<Wt::WContainerWidget>();
        auto editBtn = actionsContainer->addNew<Wt::WPushButton>("✏️");
        editBtn->setStyleClass("btn btn-sm btn-outline-primary");
        editBtn->setToolTip("Edit");
        
        auto deleteBtn = actionsContainer->addNew<Wt::WPushButton>("🗑️");
        deleteBtn->setStyleClass("btn btn-sm btn-outline-danger");
        deleteBtn->setToolTip("Delete");
        
        row++;
    }
}

void AdminDashboard::loadBookingsData() {
    // Заглушка для бронирований
    struct BookingData {
        std::string id;
        std::string client;
        std::string hall;
        std::string time;
        std::string purpose;
        std::string status;
    };
    
    std::vector<BookingData> testBookings = {
        {"BK001", "Ivan Ivanov", "Hall 1", "2023-12-01 10:00-11:00", "Group rehearsal", "Confirmed"},
        {"BK002", "Maria Petrova", "Hall 2", "2023-12-02 14:00-15:00", "Individual training", "Pending"}
    };
    
    int row = 1;
    for (const auto& booking : testBookings) {
        bookingsTable_->elementAt(row, 0)->addNew<Wt::WText>(booking.id);
        bookingsTable_->elementAt(row, 1)->addNew<Wt::WText>(booking.client);
        bookingsTable_->elementAt(row, 2)->addNew<Wt::WText>(booking.hall);
        bookingsTable_->elementAt(row, 3)->addNew<Wt::WText>(booking.time);
        bookingsTable_->elementAt(row, 4)->addNew<Wt::WText>(booking.purpose);
        
        auto statusText = bookingsTable_->elementAt(row, 5)->addNew<Wt::WText>(booking.status);
        if (booking.status == "Confirmed") {
            statusText->setStyleClass("status-confirmed");
        } else {
            statusText->setStyleClass("status-pending");
        }
        
        // Кнопки действий
        auto actionsContainer = bookingsTable_->elementAt(row, 6)->addNew<Wt::WContainerWidget>();
        auto editBtn = actionsContainer->addNew<Wt::WPushButton>("✏️");
        editBtn->setStyleClass("btn btn-sm btn-outline-primary");
        editBtn->setToolTip("Edit");
        
        auto deleteBtn = actionsContainer->addNew<Wt::WPushButton>("🗑️");
        deleteBtn->setStyleClass("btn btn-sm btn-outline-danger");
        deleteBtn->setToolTip("Delete");
        
        row++;
    }
}

void AdminDashboard::loadLessonsData() {
    // Заглушка для занятий
    struct LessonData {
        std::string id;
        std::string name;
        std::string trainer;
        std::string hall;
        std::string time;
        std::string participants;
        std::string status;
    };
    
    std::vector<LessonData> testLessons = {
        {"LS001", "Salsa Beginner", "Anna Ivanova", "Hall 1", "2023-12-01 10:00", "15/20", "Active"},
        {"LS002", "Bachata Intermediate", "Petr Sidorov", "Hall 2", "2023-12-02 11:00", "12/15", "Active"}
    };
    
    int row = 1;
    for (const auto& lesson : testLessons) {
        lessonsTable_->elementAt(row, 0)->addNew<Wt::WText>(lesson.id);
        lessonsTable_->elementAt(row, 1)->addNew<Wt::WText>(lesson.name);
        lessonsTable_->elementAt(row, 2)->addNew<Wt::WText>(lesson.trainer);
        lessonsTable_->elementAt(row, 3)->addNew<Wt::WText>(lesson.hall);
        lessonsTable_->elementAt(row, 4)->addNew<Wt::WText>(lesson.time);
        lessonsTable_->elementAt(row, 5)->addNew<Wt::WText>(lesson.participants);
        
        auto statusText = lessonsTable_->elementAt(row, 6)->addNew<Wt::WText>(lesson.status);
        statusText->setStyleClass("status-active");
        
        // Кнопки действий
        auto actionsContainer = lessonsTable_->elementAt(row, 7)->addNew<Wt::WContainerWidget>();
        auto editBtn = actionsContainer->addNew<Wt::WPushButton>("✏️");
        editBtn->setStyleClass("btn btn-sm btn-outline-primary");
        editBtn->setToolTip("Edit");
        
        auto deleteBtn = actionsContainer->addNew<Wt::WPushButton>("🗑️");
        deleteBtn->setStyleClass("btn btn-sm btn-outline-danger");
        deleteBtn->setToolTip("Delete");
        
        row++;
    }
}

void AdminDashboard::loadHallsData() {
    // Заглушка для залов
    struct HallData {
        std::string id;
        std::string name;
        std::string branch;
        std::string capacity;
        std::string equipment;
        std::string status;
    };
    
    std::vector<HallData> testHalls = {
        {"HL001", "Main Hall", "Central", "30", "Mirrors, Sound System", "Available"},
        {"HL002", "Small Studio", "Central", "15", "Mirrors", "Available"}
    };
    
    int row = 1;
    for (const auto& hall : testHalls) {
        hallsTable_->elementAt(row, 0)->addNew<Wt::WText>(hall.id);
        hallsTable_->elementAt(row, 1)->addNew<Wt::WText>(hall.name);
        hallsTable_->elementAt(row, 2)->addNew<Wt::WText>(hall.branch);
        hallsTable_->elementAt(row, 3)->addNew<Wt::WText>(hall.capacity);
        hallsTable_->elementAt(row, 4)->addNew<Wt::WText>(hall.equipment);
        
        auto statusText = hallsTable_->elementAt(row, 5)->addNew<Wt::WText>(hall.status);
        statusText->setStyleClass("status-active");
        
        // Кнопки действий
        auto actionsContainer = hallsTable_->elementAt(row, 6)->addNew<Wt::WContainerWidget>();
        auto editBtn = actionsContainer->addNew<Wt::WPushButton>("✏️");
        editBtn->setStyleClass("btn btn-sm btn-outline-primary");
        editBtn->setToolTip("Edit");
        
        auto deleteBtn = actionsContainer->addNew<Wt::WPushButton>("🗑️");
        deleteBtn->setStyleClass("btn btn-sm btn-outline-danger");
        deleteBtn->setToolTip("Delete");
        
        row++;
    }
}

void AdminDashboard::loadTrainersData() {
    // Заглушка для преподавателей
}

void AdminDashboard::loadSubscriptionsData() {
    // Заглушка для абонементов
}

void AdminDashboard::loadReviewsData() {
    // Заглушка для отзывов
}

// Action handlers
void AdminDashboard::handleClientSearch() {
    std::string searchTerm = clientSearchEdit_->text().toUTF8();
    // Здесь будет реальная логика поиска
    showStatusMessage("Client search: " + searchTerm);
}

void AdminDashboard::handleBookingFilter() {
    // Логика фильтрации бронирований
}

void AdminDashboard::handleCreateLesson() {
    showStatusMessage("Creating new lesson...");
}

void AdminDashboard::handleCreateHall() {
    std::string hallName = hallNameEdit_->text().toUTF8();
    if (!hallName.empty()) {
        showStatusMessage("Hall '" + hallName + "' created successfully!", false);
        hallNameEdit_->setText("");
    } else {
        showStatusMessage("Enter hall name", true);
    }
}

void AdminDashboard::handleCreateTrainer() {
    std::string trainerName = trainerNameEdit_->text().toUTF8();
    if (!trainerName.empty()) {
        showStatusMessage("Trainer '" + trainerName + "' added successfully!", false);
        trainerNameEdit_->setText("");
    } else {
        showStatusMessage("Enter trainer name", true);
    }
}

void AdminDashboard::handleCreateSubscriptionType() {
    showStatusMessage("Creating subscription type...");
}

void AdminDashboard::handleReviewModeration(const UUID& reviewId, bool approve) {
    // Логика модерации отзывов
}

// Utility methods
void AdminDashboard::setupTableHeaders(Wt::WTable* table, const std::vector<std::string>& headers) {
    table->clear();
    for (size_t i = 0; i < headers.size(); ++i) {
        table->elementAt(0, i)->addNew<Wt::WText>(headers[i]);
        table->elementAt(0, i)->setStyleClass("table-header");
    }
}

void AdminDashboard::showStatusMessage(const std::string& message, bool isError) {
    // Здесь можно добавить отображение статусных сообщений
    std::cout << "AdminDashboard: " << message << std::endl;
}