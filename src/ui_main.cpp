// #include <iostream>
// #include <memory>
// #include "tech_ui/TechUI.hpp"

// int main() {
//     std::cout << "=== Dance Studio Booking System ===" << std::endl;
    
//     try {
//         // Создаем технологический UI
//         TechUI techUI("postgresql://dance_user:dance_password@localhost/dance_studio");
        
//         // Запускаем основной цикл
//         techUI.run();
        
//         return 0;
        
//     } catch (const std::exception& e) {
//         std::cerr << "❌ Fatal error: " << e.what() << std::endl;
//         return 1;
//     }
// }