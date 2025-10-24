cmake_minimum_required(VERSION 3.16)
project(PPO_labs)

set(CMAKE_CXX_STANDARD 17)

# Создаем отдельную директорию для артефактов сборки
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Поиск зависимостей
find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)

# Поиск libpqxx
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

# Поиск библиотеки uuid для Linux
if(UNIX AND NOT APPLE)
    find_library(UUID_LIBRARY uuid)
    if(UUID_LIBRARY)
        message(STATUS "Found UUID library: ${UUID_LIBRARY}")
    else()
        message(FATAL_ERROR "UUID library not found. On Ubuntu/Debian, install with: sudo apt-get install uuid-dev")
    endif()
endif()

# Указываем корневую директорию исходников
set(SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Компонент 1: Бизнес-логика (чистый C++, без зависимостей от БД)
add_library(BookingCore STATIC
    ${SOURCE_ROOT}/models/TimeSlot.cpp
    ${SOURCE_ROOT}/models/Booking.cpp
    ${SOURCE_ROOT}/models/Client.cpp
    ${SOURCE_ROOT}/models/DanceHall.cpp
    ${SOURCE_ROOT}/models/Branch.cpp
    ${SOURCE_ROOT}/models/Studio.cpp
    ${SOURCE_ROOT}/models/Trainer.cpp
    ${SOURCE_ROOT}/models/Lesson.cpp
    ${SOURCE_ROOT}/models/Enrollment.cpp
    ${SOURCE_ROOT}/models/Review.cpp
    ${SOURCE_ROOT}/models/Subscription.cpp
    ${SOURCE_ROOT}/models/SubscriptionType.cpp
    ${SOURCE_ROOT}/models/Schedule.cpp
    ${SOURCE_ROOT}/types/uuid.cpp
    ${SOURCE_ROOT}/types/enums.cpp
    ${SOURCE_ROOT}/services/BookingService.cpp
    ${SOURCE_ROOT}/services/LessonService.cpp
    ${SOURCE_ROOT}/services/AuthService.cpp
    ${SOURCE_ROOT}/services/SubscriptionService.cpp
    ${SOURCE_ROOT}/services/ReviewService.cpp
    ${SOURCE_ROOT}/services/ScheduleService.cpp
    ${SOURCE_ROOT}/services/EnrollmentService.cpp
    ${SOURCE_ROOT}/dtos/BookingDTO.cpp
    ${SOURCE_ROOT}/dtos/LessonDTO.cpp
    ${SOURCE_ROOT}/dtos/SubscriptionDTO.cpp
    ${SOURCE_ROOT}/dtos/ReviewDTO.cpp
    ${SOURCE_ROOT}/dtos/AuthDTO.cpp
    ${SOURCE_ROOT}/dtos/EnrollmentDTO.cpp
    ${SOURCE_ROOT}/core/Config.cpp
    ${SOURCE_ROOT}/core/Logger.cpp
    ${SOURCE_ROOT}/data/DateTimeUtils.cpp
)

# Для Linux линкуем библиотеку uuid
if(UNIX AND NOT APPLE)
    target_link_libraries(BookingCore PRIVATE ${UUID_LIBRARY})
endif()

# Используем PRIVATE вместо PUBLIC для include directories
target_include_directories(BookingCore PRIVATE 
    ${SOURCE_ROOT}
)

# Компонент 2: Доступ к данным
add_library(DataAccess STATIC
    ${SOURCE_ROOT}/data/DatabaseConnection.cpp
    ${SOURCE_ROOT}/data/ResilientDatabaseConnection.cpp
    ${SOURCE_ROOT}/data/QueryFactory.cpp
    ${SOURCE_ROOT}/data/SqlQueryBuilder.cpp
    ${SOURCE_ROOT}/data/PostgreSQLRepositoryFactory.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLClientRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLDanceHallRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLBookingRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLSubscriptionRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLSubscriptionTypeRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLStudioRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLBranchRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLTrainerRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLLessonRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLReviewRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLEnrollmentRepository.cpp
)

target_include_directories(DataAccess PRIVATE 
    ${SOURCE_ROOT}
)

target_link_libraries(DataAccess PRIVATE BookingCore ${LIBPQXX_LIBRARIES})

# Компонент 3: Технологический UI
add_executable(TechUI
    ${SOURCE_ROOT}/tech_ui/TechUI.cpp
    ${SOURCE_ROOT}/tech_ui/TechUIManagers.cpp
    ${SOURCE_ROOT}/tech_ui/InputHandlers.cpp
    ${SOURCE_ROOT}/main.cpp
)

target_include_directories(TechUI PRIVATE ${SOURCE_ROOT})
target_link_libraries(TechUI PRIVATE BookingCore DataAccess)

# Ручная настройка Wt - используем прямое указание путей
set(WT_LIBRARIES /usr/local/lib/libwthttp.so /usr/local/lib/libwt.so)
set(WT_INCLUDE_DIRS /usr/local/include)

message(STATUS "Wt libraries: ${WT_LIBRARIES}")
message(STATUS "Wt include dirs: ${WT_INCLUDE_DIRS}")

# Компонент 4: Web UI
add_library(WebUIComponent
    ${SOURCE_ROOT}/web_ui/WebApplication.cpp
    ${SOURCE_ROOT}/web_ui/models/UserSession.cpp
    ${SOURCE_ROOT}/web_ui/controllers/AuthController.cpp
    ${SOURCE_ROOT}/web_ui/controllers/BookingController.cpp
    ${SOURCE_ROOT}/web_ui/views/LoginWidget.cpp
    ${SOURCE_ROOT}/web_ui/views/RegistrationWidget.cpp
    ${SOURCE_ROOT}/web_ui/views/ClientDashboard.cpp
    ${SOURCE_ROOT}/web_ui/views/BookingView.cpp
    ${SOURCE_ROOT}/web_ui/views/BookingCreateWidget.cpp
    ${SOURCE_ROOT}/web_ui/views/BookingListWidget.cpp
)

target_include_directories(WebUIComponent PRIVATE 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/web_ui
    ${SOURCE_ROOT}/web_ui/controllers
    ${SOURCE_ROOT}/web_ui/models
    ${SOURCE_ROOT}/web_ui/views
)

# Веб-приложение
add_executable(DanceStudioWebUI
    ${SOURCE_ROOT}/web_main.cpp
)

target_include_directories(DanceStudioWebUI PRIVATE ${SOURCE_ROOT})
target_link_libraries(DanceStudioWebUI 
    WebUIComponent
    BookingCore
    DataAccess
    ${WT_LIBRARIES}
    ${LIBPQXX_LIBRARIES}
    pthread
)

# Создаем директорию styles в бинарной директории
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/styles)

# Копируем все CSS файлы в бинарную директорию
set(CSS_SOURCE_DIR ${SOURCE_ROOT}/web_ui/styles)

# Копируем main.css в корень
#configure_file(
#    ${CSS_SOURCE_DIR}/main.css
#    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main.css
#    COPYONLY
#)

# Копируем CSS файлы в подпапку styles
file(GLOB CSS_FILES 
    "${CSS_SOURCE_DIR}/main.css"
    "${CSS_SOURCE_DIR}/base.css"
    "${CSS_SOURCE_DIR}/auth.css" 
    "${CSS_SOURCE_DIR}/dashboard.css"
    "${CSS_SOURCE_DIR}/booking.css"
)

foreach(css_file ${CSS_FILES})
    get_filename_component(css_name ${css_file} NAME)
    configure_file(
        ${css_file}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/styles/${css_name}
        COPYONLY
    )
    message(STATUS "📁 CSS файл добавлен: styles/${css_name}")
endforeach()

# Установочные правила для библиотек (опционально, можно закомментировать если не нужны)
# install(TARGETS BookingCore DataAccess
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     RUNTIME DESTINATION bin
# )

# Модульные тесты
add_executable(BookingTests
    ${SOURCE_ROOT}/tests/unit/BookingServiceTest.cpp
)

target_include_directories(BookingTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(BookingTests 
    BookingCore 
    GTest::gtest 
    GTest::gtest_main
    GTest::gmock
)

add_executable(AuthServiceTests
    ${SOURCE_ROOT}/tests/unit/AuthServiceTest.cpp
)

target_include_directories(AuthServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(AuthServiceTests 
    BookingCore 
    GTest::gtest 
    GTest::gtest_main
    GTest::gmock
)

add_executable(LessonServiceTests
    ${SOURCE_ROOT}/tests/unit/LessonServiceTest.cpp
)

target_include_directories(LessonServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(LessonServiceTests 
    BookingCore 
    GTest::gtest 
    GTest::gtest_main
    GTest::gmock
)

add_executable(SubscriptionServiceTests
    ${SOURCE_ROOT}/tests/unit/SubscriptionServiceTest.cpp
)

target_include_directories(SubscriptionServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(SubscriptionServiceTests 
    BookingCore 
    GTest::gtest 
    GTest::gtest_main
    GTest::gmock
)

add_executable(ReviewServiceTests
    ${SOURCE_ROOT}/tests/unit/ReviewServiceTest.cpp
)

target_include_directories(ReviewServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(ReviewServiceTests 
    BookingCore 
    GTest::gtest 
    GTest::gtest_main
    GTest::gmock
)

# Интеграционные тесты
add_executable(IntegrationTests
    ${SOURCE_ROOT}/tests/integration/RepositoryIntegrationTests.cpp
)

target_include_directories(IntegrationTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(IntegrationTests 
    BookingCore 
    DataAccess 
    GTest::gtest 
    GTest::gtest_main
    ${LIBPQXX_LIBRARIES}
)