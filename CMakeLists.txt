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


# Компонент 1: Бизнес-логика (чистый C++, без зависимостей от БД)
add_library(BookingCore STATIC
    src/models/TimeSlot.cpp
    src/models/Booking.cpp
    src/models/Client.cpp
    src/models/DanceHall.cpp
    src/models/Branch.cpp
    src/models/Studio.cpp
    src/models/Trainer.cpp
    src/models/Lesson.cpp
    src/models/Enrollment.cpp
    src/models/Review.cpp
    src/models/Subscription.cpp
    src/models/SubscriptionType.cpp
    src/models/Schedule.cpp
    src/types/uuid.cpp
    src/types/enums.cpp
    src/services/BookingService.cpp
    src/services/LessonService.cpp
    src/services/AuthService.cpp
    src/services/SubscriptionService.cpp
    src/services/ReviewService.cpp
    src/services/ScheduleService.cpp
    src/services/EnrollmentService.cpp
    src/dtos/BookingDTO.cpp
    src/dtos/LessonDTO.cpp
    src/dtos/SubscriptionDTO.cpp
    src/dtos/ReviewDTO.cpp
    src/dtos/AuthDTO.cpp
    src/dtos/EnrollmentDTO.cpp
    src/core/Config.cpp
    src/core/Logger.cpp
)

# Для Linux линкуем библиотеку uuid
if(UNIX AND NOT APPLE)
    target_link_libraries(BookingCore PRIVATE ${UUID_LIBRARY})
endif()

# Публичные заголовки бизнес-логики
target_include_directories(BookingCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dtos
    ${CMAKE_CURRENT_SOURCE_DIR}/src/repositories  # только интерфейсы!
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

# Компонент 2: Доступ к данным
add_library(DataAccess STATIC
    src/data/DatabaseConnection.cpp
    src/data/ResilientDatabaseConnection.cpp
    src/data/DateTimeUtils.cpp
    src/data/QueryFactory.cpp
    src/data/SqlQueryBuilder.cpp
    src/data/PostgreSQLRepositoryFactory.cpp
    src/repositories/impl/PostgreSQLClientRepository.cpp
    src/repositories/impl/PostgreSQLDanceHallRepository.cpp
    src/repositories/impl/PostgreSQLBookingRepository.cpp
    src/repositories/impl/PostgreSQLSubscriptionRepository.cpp
    src/repositories/impl/PostgreSQLSubscriptionTypeRepository.cpp
    src/repositories/impl/PostgreSQLStudioRepository.cpp
    src/repositories/impl/PostgreSQLBranchRepository.cpp
    src/repositories/impl/PostgreSQLTrainerRepository.cpp
    src/repositories/impl/PostgreSQLLessonRepository.cpp
    src/repositories/impl/PostgreSQLReviewRepository.cpp
    src/repositories/impl/PostgreSQLEnrollmentRepository.cpp
)

target_include_directories(DataAccess PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data
    ${CMAKE_CURRENT_SOURCE_DIR}/src/repositories/impl
)

target_link_libraries(DataAccess PUBLIC BookingCore ${LIBPQXX_LIBRARIES})

# Компонент 3: Технологический UI
add_executable(TechUI
    src/tech_ui/TechUI.cpp
    src/tech_ui/TechUIManagers.cpp
    src/tech_ui/InputHandlers.cpp
    src/main.cpp
)

target_include_directories(TechUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(TechUI PRIVATE BookingCore DataAccess)

# Установочные правила для библиотек
install(TARGETS BookingCore DataAccess
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Указываем корневую директорию исходников
set(SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Ручная настройка Wt - используем прямое указание путей
set(WT_LIBRARIES /usr/local/lib/libwthttp.so /usr/local/lib/libwt.so)
set(WT_INCLUDE_DIRS /usr/local/include)

message(STATUS "Wt libraries: ${WT_LIBRARIES}")
message(STATUS "Wt include dirs: ${WT_INCLUDE_DIRS}")

# Компонент 4: Web UI
add_library(WebUIComponent
    src/web_ui/WebApplication.cpp
    src/web_ui/models/UserSession.cpp
    src/web_ui/controllers/AuthController.cpp
    src/web_ui/controllers/BookingController.cpp
    src/web_ui/views/LoginWidget.cpp
    src/web_ui/views/RegistrationWidget.cpp
    src/web_ui/views/ClientDashboard.cpp
    src/web_ui/views/BookingView.cpp
    src/web_ui/views/BookingCreateWidget.cpp
    src/web_ui/views/BookingListWidget.cpp
)

target_include_directories(WebUIComponent PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/web_ui
    ${SOURCE_ROOT}/web_ui/controllers
    ${SOURCE_ROOT}/web_ui/models
    ${SOURCE_ROOT}/web_ui/views
)

# Веб-приложение
add_executable(DanceStudioWebUI
    src/web_main.cpp
)

target_link_libraries(DanceStudioWebUI 
    WebUIComponent
    BookingCore
    DataAccess
    ${WT_LIBRARIES}
    ${LIBPQXX_LIBRARIES}
    pthread
)

# Правила установки для дистрибутива
install(DIRECTORY src/ DESTINATION include
    FILES_MATCHING 
    PATTERN "*.hpp"
    PATTERN "tests" EXCLUDE
    PATTERN "tech_ui" EXCLUDE
    PATTERN "web_ui" EXCLUDE
    PATTERN "main.cpp" EXCLUDE
    # Скрываем реализации репозиториев от пользователей
    PATTERN "repositories/impl" EXCLUDE
)

install(TARGETS BookingCore DataAccess
    EXPORT BookingLibTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Создание конфигурационных файлов для find_package
install(EXPORT BookingLibTargets
    FILE BookingLibTargets.cmake
    NAMESPACE BookingLib::
    DESTINATION lib/cmake/BookingLib
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/BookingLibConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/BookingLibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/BookingLibConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/BookingLibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/BookingLibConfigVersion.cmake
    DESTINATION lib/cmake/BookingLib
)

# Создание пакета
set(CPACK_PACKAGE_NAME "BookingLib")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "Dance Studio")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional Dance Studio Management Library")
set(CPACK_GENERATOR "TGZ")
include(CPack)

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