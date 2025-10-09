cmake_minimum_required(VERSION 3.16)
project(PPO_labs)

set(CMAKE_CXX_STANDARD 17)

# Поиск зависимостей
find_package(GTest REQUIRED)
find_package(PkgConfig REQUIRED)

# Поиск libpqxx
pkg_check_modules(LIBPQXX REQUIRED libpqxx)

# Указываем корневую директорию исходников
set(SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Основная библиотека бизнес-логики
add_library(BookingCore
    ${SOURCE_ROOT}/models/TimeSlot.cpp
    ${SOURCE_ROOT}/models/Booking.cpp
    ${SOURCE_ROOT}/models/Client.cpp
    ${SOURCE_ROOT}/models/Hall.cpp
    ${SOURCE_ROOT}/types/uuid.cpp
    ${SOURCE_ROOT}/services/BookingService.cpp
    ${SOURCE_ROOT}/dtos/BookingDTO.cpp
)

# Явно указываем пути включения для BookingCore
target_include_directories(BookingCore PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/models
    ${SOURCE_ROOT}/types
    ${SOURCE_ROOT}/services
    ${SOURCE_ROOT}/dtos
    ${SOURCE_ROOT}/repositories
)

# Библиотека доступа к данным
add_library(DataAccess
    ${SOURCE_ROOT}/data/DatabaseConnection.cpp
    ${SOURCE_ROOT}/data/DateTimeUtils.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLClientRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLHallRepository.cpp
    ${SOURCE_ROOT}/repositories/impl/PostgreSQLBookingRepository.cpp
)

# Явно указываем пути включения для DataAccess
target_include_directories(DataAccess PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/data
    ${SOURCE_ROOT}/repositories/impl
    ${LIBPQXX_INCLUDE_DIRS}
)

target_link_libraries(DataAccess PRIVATE ${LIBPQXX_LIBRARIES})
target_link_libraries(DataAccess PRIVATE BookingCore)

# Модульные тесты
add_executable(BookingTests
    ${SOURCE_ROOT}/tests/BookingServiceTest.cpp
)

target_include_directories(BookingTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(BookingTests 
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

# Технологический UI - ЕДИНСТВЕННОЕ основное приложение
add_executable(TechUI
    ${SOURCE_ROOT}/main.cpp
    ${SOURCE_ROOT}/tech_ui/TechUI.cpp
)

target_include_directories(TechUI PRIVATE ${SOURCE_ROOT})
target_link_libraries(TechUI 
    BookingCore 
    DataAccess 
    ${LIBPQXX_LIBRARIES}
)
