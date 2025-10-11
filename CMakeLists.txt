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

# Обновляем основную библиотеку бизнес-логики
add_library(BookingCore
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
    ${SOURCE_ROOT}/tech_ui/InputHandlers.cpp
    ${SOURCE_ROOT}/tech_ui/TechUIManagers.cpp
)

# Для Linux линкуем библиотеку uuid
if(UNIX AND NOT APPLE)
    target_link_libraries(BookingCore PRIVATE ${UUID_LIBRARY})
endif()

# Явно указываем пути включения для BookingCore
target_include_directories(BookingCore PUBLIC 
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/models
    ${SOURCE_ROOT}/types
    ${SOURCE_ROOT}/services
    ${SOURCE_ROOT}/dtos
    ${SOURCE_ROOT}/repositories
    ${SOURCE_ROOT}/tech_ui
)

add_library(DataAccess
    ${SOURCE_ROOT}/data/DatabaseConnection.cpp
    ${SOURCE_ROOT}/data/ResilientDatabaseConnection.cpp
    ${SOURCE_ROOT}/data/DateTimeUtils.cpp
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

# Технологический UI 
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