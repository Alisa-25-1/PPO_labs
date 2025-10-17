#include <iostream>
#include "core/Logger.hpp"

int main() {
    std::cout << "🧪 ТЕСТ ЛОГГЕРА" << std::endl;
    
    try {
        std::cout << "1. Получение экземпляра Logger..." << std::endl;
        auto& logger = Logger::getInstance();
        
        std::cout << "2. Инициализация Logger..." << std::endl;
        logger.initialize("test.log", LogLevel::DEBUG);
        
        std::cout << "3. Тест записи сообщений..." << std::endl;
        logger.debug("Тестовое debug сообщение", "Test");
        logger.info("Тестовое info сообщение", "Test");
        logger.warning("Тестовое warning сообщение", "Test");
        logger.error("Тестовое error сообщение", "Test");
        
        std::cout << "4. Завершение..." << std::endl;
        std::cout << "✅ Тест завершен успешно!" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Тест завершен с ошибкой: " << e.what() << std::endl;
        return 1;
    }
}