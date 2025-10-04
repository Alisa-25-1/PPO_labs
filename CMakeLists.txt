cmake_minimum_required(VERSION 3.16)
project(DanceStudioBooking VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Find required packages
find_package(GTest REQUIRED)

# Include directories
include_directories(src)

# Main library - ВКЛЮЧАЕМ ВСЕ .CPP ФАЙЛЫ
add_library(BookingCore
    src/models/TimeSlot.cpp
    src/models/Booking.cpp
    src/models/Client.cpp
    src/models/Hall.cpp
    src/services/BookingService.cpp
    src/dtos/BookingDTO.cpp      # <- ДОБАВЛЕНО
)

# Tests executable
add_executable(BookingTests
    src/tests/BookingServiceTest.cpp
)

# Link tests with libraries
target_link_libraries(BookingTests
    BookingCore
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    pthread
)

# Add test
add_test(NAME BookingTests COMMAND BookingTests)