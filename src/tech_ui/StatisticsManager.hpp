#pragma once
#include "../services/StatisticsService.hpp"
#include "../tech_ui/InputHandlers.hpp"
#include <memory>

class StatisticsManager {
private:
    StatisticsService* statisticsService_; // Изменено на сырой указатель

public:
    explicit StatisticsManager(StatisticsService* statisticsService) // Изменено на сырой указатель
        : statisticsService_(statisticsService) {}
    
    void showMenu();
    void showStudioStats();
    void showClientStats(); 
    void showAllClientsStats();
    bool migrateHistoricalData();
};