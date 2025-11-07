#include <iostream>
#include <cstdlib>

int main() {
    std::cout << "🧪 Проверка подключения к MongoDB..." << std::endl;
    
    // Простая проверка через системную команду
    int result = system("mongosh --eval \"db.adminCommand('ismaster')\" --quiet");
    
    if (result == 0) {
        std::cout << "✅ MongoDB работает корректно" << std::endl;
        return 0;
    } else {
        std::cout << "❌ Проблемы с подключением к MongoDB" << std::endl;
        std::cout << "💡 Проверьте: sudo systemctl status mongod" << std::endl;
        return 1;
    }
}