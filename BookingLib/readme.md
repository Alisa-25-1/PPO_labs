# BookingLib - Dance Studio Management Library

## Overview

Professional C++ library for dance studio management systems. Provides complete business logic and data access components for managing clients, bookings, lessons, subscriptions, and reviews.

**Version:** 1.0.0  
**License:** Proprietary  
**Required:** C++17, PostgreSQL 12+

## Features

- Studio Management - Multi-branch studio support
- Client Management - Registration, authentication, profiles
- Booking System - Room reservations with conflict detection
- Lesson Management - Group/individual classes with enrollment
- Scheduling - Real-time schedule conflict resolution
- Subscription System - Flexible membership plans
- Review System - Rating and feedback with moderation
- PostgreSQL Support - Robust data persistence

## Installation

### Method 1: Pre-built Package (Recommended)

1. Download the latest release package:
```bash
wget https://example.com/BookingLib-1.0.0-Linux-x64.tar.gz
```

2. Extract to system directory:
```bash
sudo tar -xzf BookingLib-1.0.0-Linux-x64.tar.gz -C /usr/local/
```

3. Verify installation:
```bash
ls /usr/local/include/booking_core/
ls /usr/local/lib/libBookingCore.a
```

### Method 2: Build from Source

```bash
git clone <repository-url>
cd PPO_labs
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
```

## CMake Integration
Add to your CMakeLists.txt:

```C
cmake_minimum_required(VERSION 3.16)
project(MyDanceApp)

# Find BookingLib package
find_package(BookingLib REQUIRED)

# Create your executable
add_executable(my_app main.cpp)

# Link with BookingLib components
target_link_libraries(my_app 
    PRIVATE 
    BookingLib::BookingCore 
    BookingLib::DataAccess
)

# Include directories are handled automatically
# C++17 is required
target_compile_features(my_app PRIVATE cxx_std_17)
```


## Database Setup
### 1. Install PostgreSQL
```bash
# Ubuntu/Debian
sudo apt-get install postgresql postgresql-contrib

# CentOS/RHEL
sudo yum install postgresql-server postgresql-contrib
```

### 2. Create Database and User
```sql
CREATE DATABASE dance_studio;
CREATE USER dance_user WITH PASSWORD 'secure_password';
GRANT ALL PRIVILEGES ON DATABASE dance_studio TO dance_user;
```

### 3. Initialize Schema
Execute the provided SQL script:
```bash
psql -d dance_studio -U dance_user -f /usr/local/share/BookingLib/init_database.sql
```


## Quick Start

### Basic Configuration

Create config.json:
```json
{
    "database": {
        "connection_string": "postgresql://dance_user:secure_password@localhost/dance_studio",
        "max_connections": 10,
        "connection_timeout_seconds": 30
    },
    "business_logic": {
        "max_booking_days_ahead": 30,
        "lesson_cancellation_hours": 24,
        "max_participants_per_lesson": 50
    }
}
```

### Basic Usage Example
```C
#include <iostream>
#include <data_access/PostgreSQLRepositoryFactory.hpp>
#include <booking_core/services/AuthService.hpp>
#include <booking_core/services/BookingService.hpp>
#include <booking_core/core/Config.hpp>
#include <booking_core/core/Logger.hpp>

int main() {
    try {
        // Initialize configuration and logger
        Config::getInstance().load("config.json");
        Logger::getInstance().info("Application started");
        
        // Create repository factory
        PostgreSQLRepositoryFactory factory(
            Config::getInstance().getDatabaseConnectionString()
        );
        
        // Test connection
        if (!factory.testConnection()) {
            throw std::runtime_error("Database connection failed");
        }
        
        // Create repositories
        auto clientRepo = factory.createClientRepository();
        auto bookingRepo = factory.createBookingRepository();
        auto hallRepo = factory.createDanceHallRepository();
        auto branchRepo = factory.createBranchRepository();
        
        // Initialize services
        AuthService authService(clientRepo);
        BookingService bookingService(bookingRepo, clientRepo, hallRepo, branchRepo);
        
        // Register a new client
        AuthRequestDTO regRequest{
            "Anna Smith",
            "anna.smith@example.com",
            "+1234567890",
            "secure_password123"
        };
        
        AuthResponseDTO authResponse = authService.registerClient(regRequest);
        std::cout << "Client registered: " << authResponse.name 
                  << " (ID: " << authResponse.clientId.toString() << ")" << std::endl;
        
        Logger::getInstance().info("Application completed successfully");
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        Logger::getInstance().error("Application failed: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
```

## Core Components
### Services

- AuthService - Client registration, authentication, password management
- BookingService - Room reservation management with conflict detection
- LessonService - Class scheduling and management
- EnrollmentService - Student registration for classes
- SubscriptionService - Membership plan management
- ReviewService - Rating and feedback system
- ScheduleService - Timetable and availability management

### Data Types

- UUID - Universal unique identifier with validation
- TimeSlot - Time intervals with conflict detection
- DTO Objects - Data transfer objects for API communication

### Repository Interfaces

All data access is done through abstract interfaces:
    
- IClientRepository
- IBookingRepository
- ILessonRepository
- ISubscriptionRepository
- etc.

## Error Handling

The library uses exception-based error handling:

```C
try {
    auto booking = bookingService.createBooking(bookingRequest);
} catch (const ValidationException& e) {
    std::cerr << "Validation error: " << e.what() << std::endl;
} catch (const BookingException& e) {
    std::cerr << "Booking error: " << e.what() << std::endl;
} catch (const DataAccessException& e) {
    std::cerr << "Database error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
}
```

## Configuration
### Database Configuration

```json
{
    "database": {
        "connection_string": "postgresql://user:pass@host:port/database",
        "max_connections": 10,
        "connection_timeout_seconds": 30
    }
}
```

### Business Rules

```json
{
    "business_logic": {
        "max_booking_days_ahead": 30,
        "lesson_cancellation_hours": 24,
        "max_participants_per_lesson": 50,
        "default_lesson_duration_minutes": 60
    }
}
```

### Logging Configuration
```json
{
    "logging": {
        "level": "INFO",
        "file_path": "logs/application.log",
        "max_file_size_mb": 10,
        "backup_count": 5
    }
}
```



В CMakeLists.txt: 
cmake_minimum_required(VERSION 3.16)
project(MyDanceApp)

# Ищем вашу библиотеку
find_package(BookingLib REQUIRED)  # ← CMake автоматически находит BookingLibConfig.cmake

add_executable(my_app main.cpp)

# Используем ваши библиотеки
target_link_libraries(my_app 
    PRIVATE 
    BookingLib::BookingCore    # ← Бизнес-логика
    BookingLib::DataAccess     # ← Доступ к данным
)

В коде 
#include <data_access/PostgreSQLRepositoryFactory.hpp>
#include <booking_core/services/AuthService.hpp>

int main() {
    // Создаем фабрику - единственный класс, который нужно знать
    PostgreSQLRepositoryFactory factory("postgresql://user:pass@localhost/db");
    
    // Получаем репозитории через фабрику
    auto clientRepo = factory.createClientRepository();
    
    // Создаем сервисы
    AuthService authService(clientRepo);
    
    // Используем бизнес-логику
    AuthRequestDTO request{"John", "john@test.com", "123456", "pass"};
    auto response = authService.registerClient(request);
    
    return 0;
}