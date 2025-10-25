#include "StatisticsManager.hpp"
#include <iomanip>
#include <iostream>

void StatisticsManager::showMenu() {
    while (true) {
        std::cout << "\n=== СТАТИСТИКА ===" << std::endl;
        std::cout << "1. Общая статистика студии" << std::endl;
        std::cout << "2. Статистика по клиенту" << std::endl;
        std::cout << "3. Статистика всех клиентов" << std::endl;
        std::cout << "4. Мигрировать исторические данные" << std::endl;
        std::cout << "0. Назад" << std::endl;
        
        int choice = InputHandlers::readInt("Выберите опцию: ", 0, 4);
        
        switch (choice) {
            case 1:
                showStudioStats();
                break;
            case 2:
                showClientStats();
                break;
            case 3:
                showAllClientsStats();
                break;
            case 4:
                migrateHistoricalData();
                break;
            case 0:
                return;
            default:
                std::cout << "❌ Неверный выбор" << std::endl;
        }
    }
}

void StatisticsManager::showStudioStats() {
    try {
        std::cout << "\n--- ОБЩАЯ СТАТИСТИКА СТУДИИ ---" << std::endl;
        
        auto stats = statisticsService_->getStudioStats(); // Используем через ->
        
        std::cout << "📊 ОБЩАЯ СТАТИСТИКА:" << std::endl;
        std::cout << "👥 Всего клиентов: " << stats.totalClients << std::endl;
        std::cout << "🎓 Всего занятий: " << stats.totalLessons << std::endl;
        std::cout << "🏟️ Всего бронирований: " << stats.totalBookings << std::endl;
        
        std::cout << "\n📈 СТАТИСТИКА ПОСЕЩАЕМОСТИ:" << std::endl;
        std::cout << "✅ Посещено занятий: " << stats.visitedLessons << std::endl;
        std::cout << "❌ Отменено занятий: " << stats.cancelledLessons << std::endl;
        std::cout << "🚫 Не явились на занятия: " << stats.noShowLessons << std::endl;
        
        std::cout << "✅ Использовано бронирований: " << stats.visitedBookings << std::endl;
        std::cout << "❌ Отменено бронирований: " << stats.cancelledBookings << std::endl;
        std::cout << "🚫 Не использовано бронирований: " << stats.noShowBookings << std::endl;
        
        std::cout << "\n⭐ ОБЩИЙ РЕЙТИНГ ПОСЕЩАЕМОСТИ: " 
                  << std::fixed << std::setprecision(1) << stats.overallAttendanceRate << "%" << std::endl;
        
        if (!stats.topClients.empty()) {
            std::cout << "\n🏆 ТОП-5 КЛИЕНТОВ:" << std::endl;
            for (size_t i = 0; i < stats.topClients.size(); ++i) {
                std::cout << (i + 1) << ". " << stats.topClients[i].first 
                          << " - " << stats.topClients[i].second << " посещений" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении статистики: " << e.what() << std::endl;
    }
}

void StatisticsManager::showClientStats() {
    try {
        std::cout << "\n--- СТАТИСТИКА КЛИЕНТА ---" << std::endl;
        
        UUID clientId = InputHandlers::readUUID("Введите ID клиента: ");
        auto stats = statisticsService_->getClientStats(clientId); // Используем через ->
        
        std::cout << "\n📊 СТАТИСТИКА КЛИЕНТА: " << stats.clientName << std::endl;
        std::cout << "🎓 ЗАНЯТИЯ:" << std::endl;
        std::cout << "   Всего записей: " << stats.totalLessons << std::endl;
        std::cout << "   ✅ Посещено: " << stats.visitedLessons << std::endl;
        std::cout << "   ❌ Отменено: " << stats.cancelledLessons << std::endl;
        std::cout << "   🚫 Пропущено: " << stats.noShowLessons << std::endl;
        
        std::cout << "🏟️ БРОНИРОВАНИЯ:" << std::endl;
        std::cout << "   Всего бронирований: " << stats.totalBookings << std::endl;
        std::cout << "   ✅ Использовано: " << stats.visitedBookings << std::endl;
        std::cout << "   ❌ Отменено: %" << stats.cancelledBookings << std::endl;
        std::cout << "   🚫 Не использовано: " << stats.noShowBookings << std::endl;
        
        std::cout << "⭐ РЕЙТИНГ ПОСЕЩАЕМОСТИ: " 
                  << std::fixed << std::setprecision(1) << stats.attendanceRate << "%" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении статистики клиента: " << e.what() << std::endl;
    }
}

void StatisticsManager::showAllClientsStats() {
    try {
        std::cout << "\n--- СТАТИСТИКА ВСЕХ КЛИЕНТОВ ---" << std::endl;
        
        auto allStats = statisticsService_->getAllClientsStats(); // Используем через ->
        
        if (allStats.empty()) {
            std::cout << "Нет данных для отображения." << std::endl;
            return;
        }
        
        std::cout << "📊 СТАТИСТИКА ПОСЕЩАЕМОСТИ КЛИЕНТОВ:" << std::endl;
        std::cout << "=============================================" << std::endl;
        
        for (const auto& stats : allStats) {
            std::cout << "👤 " << stats.clientName << " (ID: " << stats.clientId.toString() << ")" << std::endl;
            std::cout << "   🎓 Занятия: " << stats.visitedLessons << "/" << stats.totalLessons 
                      << " (" << std::fixed << std::setprecision(1) 
                      << (stats.totalLessons > 0 ? (static_cast<double>(stats.visitedLessons) / stats.totalLessons * 100) : 0)
                      << "%)" << std::endl;
            std::cout << "   🏟️ Бронирования: " << stats.visitedBookings << "/" << stats.totalBookings 
                      << " (" << std::fixed << std::setprecision(1) 
                      << (stats.totalBookings > 0 ? (static_cast<double>(stats.visitedBookings) / stats.totalBookings * 100) : 0)
                      << "%)" << std::endl;
            std::cout << "   ⭐ Общий рейтинг: " << std::fixed << std::setprecision(1) << stats.attendanceRate << "%" << std::endl;
            std::cout << "---------------------------------------------" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка при получении статистики: " << e.what() << std::endl;
    }
}

bool StatisticsManager::migrateHistoricalData() {
    std::cout << "\n--- МИГРАЦИЯ ИСТОРИЧЕСКИХ ДАННЫХ ---" << std::endl;
    std::cout << "⚠️  Эта операция перенесет существующие бронирования и записи в систему посещаемости." << std::endl;
    std::cout << "Продолжить? (y/n): ";
    
    if (InputHandlers::getConfirmation()) {
        if (statisticsService_->migrateExistingData()) { // Используем через ->
            std::cout << "✅ Миграция завершена успешно!" << std::endl;
            std::cout << "📊 Теперь статистика будет учитывать все исторические данные." << std::endl;
            return true;
        } else {
            std::cout << "❌ Ошибка при миграции данных" << std::endl;
            return false;
        }
    }
    return false;
}