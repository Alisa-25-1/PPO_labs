// #include <iostream>
// #include <memory>
// #include "data/DatabaseConnection.hpp"
// #include "repositories/impl/PostgreSQLClientRepository.hpp"
// #include "repositories/impl/PostgreSQLHallRepository.hpp" 
// #include "repositories/impl/PostgreSQLBookingRepository.hpp"
// #include "services/BookingService.hpp"

// int main() {
//     std::cout << "=== Dance Studio Booking System ===" << std::endl;
    
//     try {
//         // Создаем соединение с БД
//         auto dbConnection = std::make_shared<DatabaseConnection>(
//             "postgresql://dance_user:dance_password@localhost/dance_studio"
//         );
        
//         std::cout << "✅ Database connection: " << (dbConnection->isConnected() ? "OK" : "FAILED") << std::endl;
        
//         // Создаем репозитории
//         auto clientRepo = std::make_unique<PostgreSQLClientRepository>(dbConnection);
//         auto hallRepo = std::make_unique<PostgreSQLHallRepository>(dbConnection);
//         auto bookingRepo = std::make_unique<PostgreSQLBookingRepository>(dbConnection);
        
//         // Создаем сервис
//         BookingService bookingService(
//             std::move(bookingRepo),
//             std::move(clientRepo),
//             std::move(hallRepo)
//         );
        
//         std::cout << "✅ System initialized successfully!" << std::endl;
        
//         return 0;
        
//     } catch (const std::exception& e) {
//         std::cerr << "❌ Fatal error: " << e.what() << std::endl;
//         return 1;
//     }
// }