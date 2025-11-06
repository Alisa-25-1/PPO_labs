#ifndef MOCK_ENROLLMENT_REPOSITORY_HPP
#define MOCK_ENROLLMENT_REPOSITORY_HPP

#include <gmock/gmock.h>
#include "../../../repositories/IEnrollmentRepository.hpp"

class MockEnrollmentRepository : public IEnrollmentRepository {
public:
    MOCK_METHOD(std::optional<Enrollment>, findById, (const UUID&), (override));
    MOCK_METHOD(std::vector<Enrollment>, findByClientId, (const UUID&), (override));
    MOCK_METHOD(std::vector<Enrollment>, findByLessonId, (const UUID&), (override));
    MOCK_METHOD(std::optional<Enrollment>, findByClientAndLesson, (const UUID&, const UUID&), (override));
    MOCK_METHOD(int, countByLessonId, (const UUID&), (override));
    MOCK_METHOD(std::vector<Enrollment>, findAll, (), (override));
    MOCK_METHOD(bool, save, (const Enrollment&), (override));
    MOCK_METHOD(bool, update, (const Enrollment&), (override));
    MOCK_METHOD(bool, remove, (const UUID&), (override));
    MOCK_METHOD(bool, exists, (const UUID&), (override));
};

#endif // MOCK_ENROLLMENT_REPOSITORY_HPP