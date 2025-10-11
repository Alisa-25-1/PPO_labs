#include "EnrollmentDTO.hpp"
#include <sstream>
#include <iomanip>

bool EnrollmentRequestDTO::validate() const {
    return !clientId.isNull() && clientId.isValid() &&
           !lessonId.isNull() && lessonId.isValid();
}