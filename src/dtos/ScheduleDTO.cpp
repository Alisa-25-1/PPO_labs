#include "ScheduleDTO.hpp"

bool ScheduleRequestDTO::validate() const {
    return !branchId.isNull() && branchId.isValid() &&
           startDate < endDate;
}