#ifndef MOCKBRANCHREPOSITORY_HPP
#define MOCKBRANCHREPOSITORY_HPP

#include <gmock/gmock.h>
#include "../../../repositories/IBranchRepository.hpp"

class MockBranchRepository : public IBranchRepository {
public:
    MOCK_METHOD(std::optional<Branch>, findById, (const UUID& id), (override));
    MOCK_METHOD(std::vector<Branch>, findByStudioId, (const UUID& studioId), (override));
    MOCK_METHOD(std::vector<Branch>, findAll, (), (override));
    MOCK_METHOD(bool, save, (const Branch& branch), (override));
    MOCK_METHOD(bool, update, (const Branch& branch), (override));
    MOCK_METHOD(bool, remove, (const UUID& id), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
};

#endif // MOCKBRANCHREPOSITORY_HPP