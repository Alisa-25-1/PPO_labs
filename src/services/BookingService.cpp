#include "BookingService.hpp"
#include <algorithm>

// Constructor
BookingService::BookingService(
    std::shared_ptr<IBookingRepository> bookingRepo,
    std::shared_ptr<IClientRepository> clientRepo,
    std::shared_ptr<IDanceHallRepository> hallRepo,
    std::shared_ptr<IBranchRepository> branchRepo
) : bookingRepository_(std::move(bookingRepo)),
    clientRepository_(std::move(clientRepo)),
    hallRepository_(std::move(hallRepo)),
    branchRepository_(std::move(branchRepo)) {}

// Validation methods
void BookingService::validateBookingRequest(const BookingRequestDTO& request) const {
    if (!request.validate()) {
        throw ValidationException("Invalid booking request data");
    }
    
    validateClient(request.clientId);
    validateDanceHall(request.hallId);
    validateTimeSlot(request.timeSlot);

    validateWorkingHours(request.hallId, request.timeSlot);
    
    if (!Booking::isValidPurpose(request.purpose)) {
        throw ValidationException("Invalid booking purpose");
    }
}

void BookingService::validateClient(const UUID& clientId) const {
    auto client = clientRepository_->findById(clientId);
    if (!client) {
        throw ValidationException("Client not found");
    }
    
    if (!client->isActive()) {
        throw BusinessRuleException("Client account is not active");
    }
}

void BookingService::validateDanceHall(const UUID& hallId) const {  // Исправлено
    if (!hallRepository_->exists(hallId)) {
        throw ValidationException("Dance hall not found");
    }
}

void BookingService::validateTimeSlot(const TimeSlot& timeSlot) const {
    if (!timeSlot.isValid()) {
        throw ValidationException("Invalid time slot");
    }
    
    if (!Booking::isFutureBooking(timeSlot)) {
        throw BusinessRuleException("Booking must be for future time");
    }
}

void BookingService::validateWorkingHours(const UUID& hallId, const TimeSlot& timeSlot) const {
    // Получаем филиал для зала
    auto branch = getBranchForHall(hallId);
    if (!branch) {
        throw ValidationException("Не удалось найти филиал для указанного зала");
    }
    
    // Получаем время работы филиала
    auto workingHours = branch->getWorkingHours();
    
    // Проверяем, что временной слот находится в пределах рабочего времени
    if (!isWithinWorkingHours(timeSlot, workingHours.openTime, workingHours.closeTime)) {
        std::string error = "Время бронирования выходит за пределы рабочего времени филиала. ";
        error += "Филиал работает с " + std::to_string(workingHours.openTime.count()) + 
                ":00 до " + std::to_string(workingHours.closeTime.count()) + ":00";
        throw BusinessRuleException(error);
    }
}

std::optional<Branch> BookingService::getBranchForHall(const UUID& hallId) const {
    try {
        // Получаем зал
        auto hall = hallRepository_->findById(hallId);
        if (!hall) {
            return std::nullopt;
        }
        
        // Получаем филиал зала
        return branchRepository_->findById(hall->getBranchId());
    } catch (const std::exception& e) {
        // Логируем ошибку, но не прерываем выполнение
        std::cerr << "Ошибка при получении филиала для зала: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool BookingService::isWithinWorkingHours(const TimeSlot& timeSlot, 
                                        const std::chrono::hours& openTime, 
                                        const std::chrono::hours& closeTime) const {
    return DateTimeUtils::isTimeInRange(timeSlot.getStartTime(), openTime, closeTime) &&
           DateTimeUtils::isTimeInRange(timeSlot.getEndTime(), openTime, closeTime);
}


void BookingService::checkBookingConflicts(const UUID& hallId, const TimeSlot& timeSlot, const UUID& excludeBookingId) const {
    auto conflictingBookings = bookingRepository_->findConflictingBookings(hallId, timeSlot);
    
    if (!excludeBookingId.isNull()) {
        conflictingBookings.erase(
            std::remove_if(conflictingBookings.begin(), conflictingBookings.end(),
                [excludeBookingId](const Booking& booking) {
                    return booking.getId() == excludeBookingId;
                }),
            conflictingBookings.end()
        );
    }
    
    if (!conflictingBookings.empty()) {
        throw BookingConflictException("Time slot conflicts with existing booking");
    }
}

BookingResponseDTO BookingService::createBooking(const BookingRequestDTO& request) {

    validateBookingRequest(request);
    
    if (!canClientBook(request.clientId)) {
        throw BusinessRuleException("Client cannot create new booking");
    }
    
    checkBookingConflicts(request.hallId, request.timeSlot);
    
    UUID newId = UUID::generate();
    Booking booking(newId, request.clientId, request.hallId, request.timeSlot, request.purpose);
    booking.confirm();
    
    if (!bookingRepository_->save(booking)) {
        throw BookingException("Failed to save booking");
    }
    
    return BookingResponseDTO(booking);
}

BookingResponseDTO BookingService::cancelBooking(const UUID& bookingId, const UUID& clientId) {
    auto booking = bookingRepository_->findById(bookingId);
    if (!booking) {
        throw BookingNotFoundException("Booking not found");
    }
    
    if (booking->getClientId() != clientId) {
        throw BusinessRuleException("Client can only cancel their own bookings");
    }
    
    if (booking->isCancelled()) {
        throw BusinessRuleException("Booking is already cancelled");
    }
    
    if (booking->isCompleted()) {
        throw BusinessRuleException("Cannot cancel completed booking");
    }
    
    booking->cancel();
    
    if (!bookingRepository_->update(*booking)) {
        throw BookingException("Failed to cancel booking");
    }
    
    return BookingResponseDTO(*booking);
}

BookingResponseDTO BookingService::getBooking(const UUID& bookingId) {
    auto booking = bookingRepository_->findById(bookingId);
    if (!booking) {
        throw BookingNotFoundException("Booking not found");
    }
    
    return BookingResponseDTO(*booking);
}

std::vector<BookingResponseDTO> BookingService::getClientBookings(const UUID& clientId) {
    validateClient(clientId);
    
    auto bookings = bookingRepository_->findByClientId(clientId);
    std::vector<BookingResponseDTO> result;
    
    for (const auto& booking : bookings) {
        result.push_back(BookingResponseDTO(booking));
    }
    
    return result;
}

std::vector<BookingResponseDTO> BookingService::getDanceHallBookings(const UUID& hallId) {  
    validateDanceHall(hallId);  
    
    auto bookings = bookingRepository_->findByHallId(hallId);
    std::vector<BookingResponseDTO> result;
    
    for (const auto& booking : bookings) {
        result.push_back(BookingResponseDTO(booking));
    }
    
    return result;
}

bool BookingService::isTimeSlotAvailable(const UUID& hallId, const TimeSlot& timeSlot) const {
    // Проверяем существование зала
    if (!hallRepository_->exists(hallId)) {
        return false;
    }
    
    // Проверяем конфликты
    try {
        checkBookingConflicts(hallId, timeSlot);
        return true;
    } catch (const BookingConflictException&) {
        return false;
    }
}

// Business rules
bool BookingService::canClientBook(const UUID& clientId) const {
    // Business rule: client can have maximum 3 active bookings
    return getClientActiveBookingsCount(clientId) < 3;
}

int BookingService::getClientActiveBookingsCount(const UUID& clientId) const {
    auto bookings = bookingRepository_->findByClientId(clientId);
    int activeCount = 0;
    
    for (const auto& booking : bookings) {
        if (booking.isActive()) {
            activeCount++;
        }
    }
    
    return activeCount;
}

// Добавляем метод для получения всех залов
std::vector<DanceHall> BookingService::getAllHalls() const {
    try {
        return hallRepository_->findAll();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения списка залов: " << e.what() << std::endl;
        return {};
    }
}

// Добавляем метод для получения зала по ID
std::optional<DanceHall> BookingService::getHallById(const UUID& hallId) const {
    try {
        return hallRepository_->findById(hallId);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения зала: " << e.what() << std::endl;
        return std::nullopt;
    }
}

// Новый метод для получения максимальной доступной продолжительности
std::vector<int> BookingService::getAvailableDurations(const UUID& hallId, 
                                                      const std::chrono::system_clock::time_point& startTime) const {
    try {
        std::cout << "⏱️ Расчет доступных продолжительностей для зала " << hallId.toString() 
                  << " в " << DateTimeUtils::formatTime(startTime) << std::endl; // Используем DateTimeUtils
        
        validateDanceHall(hallId);
        
        // Получаем все бронирования для зала
        auto bookings = bookingRepository_->findByHallId(hallId);
        
        // Фильтруем бронирования на выбранный день с помощью DateTimeUtils
        auto dayBookings = filterBookingsByDate(bookings, startTime);
        
        // Возможные продолжительности в минутах
        std::vector<int> possibleDurations = {60, 120, 180, 240}; // 1, 2, 3, 4 часа
        std::vector<int> availableDurations;
        
        // Проверяем каждую продолжительность
        for (int duration : possibleDurations) {
            TimeSlot proposedSlot(startTime, duration);
            
            // Проверяем, что слот не конфликтует с существующими бронированиями
            bool isAvailable = true;
            for (const auto& booking : dayBookings) {
                if (booking.getTimeSlot().overlapsWith(proposedSlot) && 
                    !booking.isCancelled() && !booking.isCompleted()) {
                    isAvailable = false;
                    break;
                }
            }
            
            // Дополнительная проверка через isTimeSlotAvailable
            if (isAvailable && !isTimeSlotAvailable(hallId, proposedSlot)) {
                isAvailable = false;
            }
            
            // Проверяем рабочие часы с помощью DateTimeUtils
            if (isAvailable) {
                auto branch = getBranchForHall(hallId);
                if (branch) {
                    auto workingHours = branch->getWorkingHours();
                    if (!DateTimeUtils::isTimeInRange(proposedSlot.getEndTime(), workingHours.openTime, workingHours.closeTime)) {
                        isAvailable = false;
                    }
                }
            }
            
            if (isAvailable) {
                availableDurations.push_back(duration);
            }
        }
        
        std::cout << "✅ Доступные продолжительности: ";
        for (int dur : availableDurations) {
            std::cout << dur/60 << "ч ";
        }
        std::cout << std::endl;
        
        return availableDurations;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка расчета продолжительностей: " << e.what() << std::endl;
        return {};
    }
}

std::vector<TimeSlot> BookingService::getAvailableTimeSlots(const UUID& hallId, 
                                                           const std::chrono::system_clock::time_point& date) const {
    try {
        validateDanceHall(hallId);
        
        // Получаем все бронирования для зала
        auto bookings = bookingRepository_->findByHallId(hallId);
        
        // Фильтруем бронирования по дате с помощью DateTimeUtils
        auto dayBookings = filterBookingsByDate(bookings, date);
        
        // Получаем рабочие часы филиала
        auto branch = getBranchForHall(hallId);
        if (!branch) {
            throw ValidationException("Не удалось определить рабочие часы зала");
        }
        
        auto workingHours = branch->getWorkingHours();
        
        // Генерируем доступные слоты с учетом максимальной продолжительности
        return generateAvailableSlotsWithDuration(date, workingHours.openTime, workingHours.closeTime, dayBookings, hallId);
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка получения доступных слотов: " << e.what() << std::endl;
        return {};
    }
}

// Вспомогательный метод для фильтрации бронирований по дате
std::vector<Booking> BookingService::filterBookingsByDate(const std::vector<Booking>& bookings, 
                                                         const std::chrono::system_clock::time_point& date) const {
    std::vector<Booking> result;
    
    for (const auto& booking : bookings) {
        if (DateTimeUtils::isSameDay(booking.getTimeSlot().getStartTime(), date)) {
            result.push_back(booking);
        }
    }
    
    return result;
}

// Вспомогательный метод для генерации доступных слотов
std::vector<TimeSlot> BookingService::generateAvailableSlotsWithDuration(
    const std::chrono::system_clock::time_point& date,
    const std::chrono::hours& openTime,
    const std::chrono::hours& closeTime,
    const std::vector<Booking>& existingBookings,
    const UUID& hallId) const {
    
    std::vector<TimeSlot> availableSlots;
    
    auto base_time_t = std::chrono::system_clock::to_time_t(date);
    std::tm base_tm = *std::localtime(&base_time_t);
    
    int startHour = openTime.count();
    int endHour = closeTime.count();
    
    for (int hour = startHour; hour < endHour; hour++) {
        for (int minute = 0; minute < 60; minute += 60) {
            base_tm.tm_hour = hour;
            base_tm.tm_min = minute;
            base_tm.tm_sec = 0;
            
            auto slotStart = std::chrono::system_clock::from_time_t(std::mktime(&base_tm));
            
            // Проверяем доступные продолжительности для этого времени начала
            auto availableDurations = getAvailableDurations(hallId, slotStart);
            
            if (!availableDurations.empty()) {
                // Создаем слот с минимальной доступной продолжительностью
                int minDuration = *std::min_element(availableDurations.begin(), availableDurations.end());
                TimeSlot slot(slotStart, minDuration);
                availableSlots.push_back(slot);
            }
        }
    }
    
    return availableSlots;
}