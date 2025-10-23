#include <Wt/WServer.h>
#include <iostream>
#include <filesystem>
#include "web_ui/WebApplication.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    try {
        // Создаем аргументы командной строки с docroot
        std::vector<const char*> args;
        args.push_back(argv[0]); // имя программы
        
        // Добавляем параметр docroot - указываем текущую директорию
        args.push_back("--docroot");
        args.push_back(".");
        
        // Опционально: устанавливаем порт
        args.push_back("--http-port");
        args.push_back("8080");
        
        // Опционально: адрес
        args.push_back("--http-address");
        args.push_back("0.0.0.0");
        
        // Создаем сервер с нашими аргументами
        Wt::WServer server(args.size(), const_cast<char**>(args.data()));
        
        server.addEntryPoint(Wt::EntryPointType::Application, 
            [](const Wt::WEnvironment& env) {
                return std::make_unique<WebApplication>(env);
            });
        
        std::cout << "🚀 Сервер запущен: http://localhost:8080" << std::endl;
        std::cout << "📁 DocRoot: " << fs::current_path().string() << std::endl;
        
        if (server.start()) {
            Wt::WServer::waitForShutdown();
            server.stop();
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "💥 Ошибка: " << e.what() << std::endl;
        return 1;
    }
}