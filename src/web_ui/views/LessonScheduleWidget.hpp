#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include <Wt/WDateEdit.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "../../types/uuid.hpp"
#include "../../models/Branch.hpp"
#include "../../dtos/LessonDTO.hpp"
#include <vector>
#include <map>

class WebApplication;

class LessonScheduleWidget : public Wt::WContainerWidget {
public:
    LessonScheduleWidget(WebApplication* app);
    
private:
    WebApplication* app_;
    
    Wt::WComboBox* branchComboBox_;
    Wt::WDateEdit* dateEdit_;
    Wt::WPushButton* searchButton_;
    Wt::WTable* lessonsTable_;
    Wt::WText* statusText_;

    // Храним mapping между индексами комбобокса и UUID филиалов
    std::map<int, UUID> branchIdMap_;
    std::vector<Branch> branches_;

    void setupUI();
    void handleSearch();
    void handleEnroll(const UUID& lessonId);
    void updateStatus(const std::string& message, bool isError = false);
    void loadBranches();
    void displayLessons(const std::vector<LessonResponseDTO>& lessons);
};