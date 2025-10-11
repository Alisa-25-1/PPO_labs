#include <iostream>
#include <memory>
#include "tech_ui/TechUI.hpp"

int main() {
    try {
        std::string connectionString = "postgresql://dance_user:dance_password@localhost/dance_studio";
        
        std::cout << "🚀 Запуск технологического UI танцевальной студии" << std::endl;
        std::cout << "📡 Подключение к БД: " << connectionString << std::endl;
        
        TechUI techUI(connectionString);
        techUI.run();
        
        std::cout << "👋 Завершение работы технологического UI" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
}