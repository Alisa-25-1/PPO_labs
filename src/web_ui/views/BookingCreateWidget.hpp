#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WDateEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "../../types/uuid.hpp"
#include "../../models/DanceHall.hpp"
#include "../../models/Branch.hpp" 
#include "../../dtos/BookingDTO.hpp"
#include "../../models/TimeSlot.hpp"
#include "../../data/DateTimeUtils.hpp"

class WebApplication;

class BookingCreateWidget : public Wt::WContainerWidget {
public:
    BookingCreateWidget(WebApplication* app);
    void setBackToListCallback(std::function<void()> callback);
    
private:
    WebApplication* app_;
    std::function<void()> onBackToList_;
    
    Wt::WComboBox* branchComboBox_;
    Wt::WComboBox* hallComboBox_;
    Wt::WDateEdit* dateEdit_;
    Wt::WComboBox* timeComboBox_;
    Wt::WComboBox* durationComboBox_;
    Wt::WLineEdit* purposeEdit_;
    Wt::WPushButton* createButton_;
    Wt::WPushButton* backButton_;
    Wt::WText* statusText_;
    std::chrono::minutes currentTimezoneOffset_;

    std::vector<Branch> branches_;
    std::vector<DanceHall> halls_;

    void setupUI();
    void handleCreate();
    void handleBack();
    void updateStatus(const std::string& message, bool isError = false);
    
    void loadAvailableBranches();
    void onBranchChanged();
    void loadHallsByBranch(const UUID& branchId);
    
    void loadAvailableTimeSlots();
    void updateAvailableDurations();
    
    // Вспомогательные методы для работы с бизнес-логикой
    std::vector<TimeSlot> getAvailableTimeSlotsFromService(const UUID& hallId, const Wt::WDate& date);
    bool isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot);
    BookingResponseDTO createBookingThroughService(const BookingRequestDTO& request);
    UUID getCurrentClientId();
    std::chrono::system_clock::time_point createDateTime(const Wt::WDate& date, int hours, int minutes);
    std::chrono::minutes getTimezoneOffsetForCurrentHall() const;
    std::string formatTimeWithOffset(const std::chrono::system_clock::time_point& timePoint) const;
};