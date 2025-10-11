#include "ReviewDTO.hpp"

bool ReviewRequestDTO::validate() const {
    return !clientId.isNull() && clientId.isValid() &&
           !lessonId.isNull() && lessonId.isValid() &&
           rating >= 1 && rating <= 5 &&
           comment.length() <= 1000;
}