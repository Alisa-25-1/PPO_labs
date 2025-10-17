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

# Поиск библиотеки uuid для Linux
if(UNIX AND NOT APPLE)
    find_library(UUID_LIBRARY uuid)
    if(UUID_LIBRARY)
        message(STATUS "Found UUID library: ${UUID_LIBRARY}")
    else()
        message(FATAL_ERROR "UUID library not found. On Ubuntu/Debian, install with: sudo apt-get install uuid-dev")
    endif()
endif()

# Компонент Core (типы, утилиты, логирование, конфигурация)
add_library(CoreComponent
    ${SOURCE_ROOT}/types/uuid.cpp
    ${SOURCE_ROOT}/types/enums.cpp
    ${SOURCE_ROOT}/core/Logger.cpp
    ${SOURCE_ROOT}/core/Config.cpp
)

# Компонент Models (все модели данных)
add_library(ModelsComponent
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
)

# Компонент DTOs
add_library(DTOsComponent
    ${SOURCE_ROOT}/dtos/BookingDTO.cpp
    ${SOURCE_ROOT}/dtos/LessonDTO.cpp
    ${SOURCE_ROOT}/dtos/SubscriptionDTO.cpp
    ${SOURCE_ROOT}/dtos/ReviewDTO.cpp     
    ${SOURCE_ROOT}/dtos/AuthDTO.cpp        
    ${SOURCE_ROOT}/dtos/EnrollmentDTO.cpp  
)

# Компонент Services (бизнес-логика)
add_library(ServicesComponent
    ${SOURCE_ROOT}/services/BookingService.cpp
    ${SOURCE_ROOT}/services/LessonService.cpp
    ${SOURCE_ROOT}/services/AuthService.cpp
    ${SOURCE_ROOT}/services/SubscriptionService.cpp
    ${SOURCE_ROOT}/services/ReviewService.cpp
    ${SOURCE_ROOT}/services/ScheduleService.cpp
    ${SOURCE_ROOT}/services/EnrollmentService.cpp
)

# Компонент DataAccess
add_library(DataAccessComponent
    ${SOURCE_ROOT}/data/DatabaseConnection.cpp
    ${SOURCE_ROOT}/data/ResilientDatabaseConnection.cpp
    ${SOURCE_ROOT}/data/DateTimeUtils.cpp
    ${SOURCE_ROOT}/data/QueryFactory.cpp
    ${SOURCE_ROOT}/data/SqlQueryBuilder.cpp

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

# Компонент TechUI
add_library(TechUIComponent
    ${SOURCE_ROOT}/tech_ui/InputHandlers.cpp
    ${SOURCE_ROOT}/tech_ui/TechUIManagers.cpp
    ${SOURCE_ROOT}/tech_ui/TechUI.cpp
)

# Настройка путей включения для всех компонентов
target_include_directories(CoreComponent PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/core
    ${SOURCE_ROOT}/types
)

target_include_directories(ModelsComponent PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/models
    ${SOURCE_ROOT}/types
)

target_include_directories(DTOsComponent PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/dtos
    ${SOURCE_ROOT}/models
)

target_include_directories(ServicesComponent PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/services
    ${SOURCE_ROOT}/models
    ${SOURCE_ROOT}/dtos
    ${SOURCE_ROOT}/repositories
)

target_include_directories(DataAccessComponent PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/data
    ${SOURCE_ROOT}/repositories
    ${SOURCE_ROOT}/repositories/impl
    ${LIBPQXX_INCLUDE_DIRS}
)

target_include_directories(TechUIComponent PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/tech_ui
    ${SOURCE_ROOT}/services
    ${SOURCE_ROOT}/dtos
    ${SOURCE_ROOT}/models
)

# Настройка зависимостей между компонентами
target_link_libraries(ModelsComponent PUBLIC CoreComponent)
target_link_libraries(DTOsComponent PUBLIC ModelsComponent)
target_link_libraries(ServicesComponent PUBLIC DTOsComponent ModelsComponent)
target_link_libraries(DataAccessComponent PUBLIC ServicesComponent DTOsComponent ModelsComponent)
target_link_libraries(TechUIComponent PUBLIC ServicesComponent DataAccessComponent DTOsComponent ModelsComponent CoreComponent)

# Для Linux линкуем библиотеку uuid
if(UNIX AND NOT APPLE)
    target_link_libraries(CoreComponent PRIVATE ${UUID_LIBRARY})
    target_link_libraries(ModelsComponent PRIVATE ${UUID_LIBRARY})
endif()

target_link_libraries(DataAccessComponent PRIVATE ${LIBPQXX_LIBRARIES})

# Основное приложение
add_executable(DanceStudioTechUI
    ${SOURCE_ROOT}/main.cpp
    ${SOURCE_ROOT}/core/Logger.cpp
    ${SOURCE_ROOT}/core/Config.cpp
)

target_link_libraries(DanceStudioTechUI 
    TechUIComponent
    ServicesComponent 
    DataAccessComponent 
    DTOsComponent
    ModelsComponent
    CoreComponent
    ${LIBPQXX_LIBRARIES}
)

# Установка конфигурационных файлов
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config.json
    ${CMAKE_CURRENT_BINARY_DIR}/config/config.json
    COPYONLY
)

# Создание директорий при сборке
add_custom_command(TARGET DanceStudioTechUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/logs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/config
    COMMENT "Creating output directories"
)

# Модульные тесты (остаются как были)
add_executable(BookingTests ${SOURCE_ROOT}/tests/unit/BookingServiceTest.cpp)
target_include_directories(BookingTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(BookingTests ServicesComponent DTOsComponent ModelsComponent CoreComponent GTest::gtest GTest::gtest_main GTest::gmock)

add_executable(AuthServiceTests ${SOURCE_ROOT}/tests/unit/AuthServiceTest.cpp)
target_include_directories(AuthServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(AuthServiceTests ServicesComponent DTOsComponent ModelsComponent CoreComponent GTest::gtest GTest::gtest_main GTest::gmock)

add_executable(LessonServiceTests ${SOURCE_ROOT}/tests/unit/LessonServiceTest.cpp)
target_include_directories(LessonServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(LessonServiceTests ServicesComponent DTOsComponent ModelsComponent CoreComponent GTest::gtest GTest::gtest_main GTest::gmock)

add_executable(SubscriptionServiceTests ${SOURCE_ROOT}/tests/unit/SubscriptionServiceTest.cpp)
target_include_directories(SubscriptionServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(SubscriptionServiceTests ServicesComponent DTOsComponent ModelsComponent CoreComponent GTest::gtest GTest::gtest_main GTest::gmock)

add_executable(ReviewServiceTests ${SOURCE_ROOT}/tests/unit/ReviewServiceTest.cpp)
target_include_directories(ReviewServiceTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(ReviewServiceTests ServicesComponent DTOsComponent ModelsComponent CoreComponent GTest::gtest GTest::gtest_main GTest::gmock)

# Интеграционные тесты
add_executable(IntegrationTests ${SOURCE_ROOT}/tests/integration/RepositoryIntegrationTests.cpp)
target_include_directories(IntegrationTests PRIVATE ${SOURCE_ROOT})
target_link_libraries(IntegrationTests DataAccessComponent ServicesComponent DTOsComponent ModelsComponent CoreComponent GTest::gtest GTest::gtest_main ${LIBPQXX_LIBRARIES})

# Установка цели для копирования конфигов
add_custom_target(CopyConfig ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/config
        ${CMAKE_CURRENT_BINARY_DIR}/config
    COMMENT "Copying configuration files"
)

add_dependencies(DanceStudioTechUI CopyConfig)

add_executable(TestLogger src/test_logger.cpp src/core/Logger.cpp src/core/Config.cpp)
target_include_directories(TestLogger PRIVATE ${SOURCE_ROOT})