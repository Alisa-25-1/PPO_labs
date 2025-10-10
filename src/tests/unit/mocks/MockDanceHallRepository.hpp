#pragma once
#include "../../../repositories/IDanceHallRepository.hpp"
#include <gmock/gmock.h>

class MockDanceHallRepository : public IDanceHallRepository {  
public:
    MOCK_METHOD(std::optional<DanceHall>, findById, (const UUID& id), (override));  
    MOCK_METHOD(std::vector<DanceHall>, findByBranchId, (const UUID& branchId), (override));
    MOCK_METHOD(bool, exists, (const UUID& id), (override));
    MOCK_METHOD(std::vector<DanceHall>, findAll, (), (override)); 
    MOCK_METHOD(bool, save, (const DanceHall& hall), (override));  
    MOCK_METHOD(bool, update, (const DanceHall& hall), (override));  
};