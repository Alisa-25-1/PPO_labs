#pragma once
#include <memory>
#include "../repositories/IClientRepository.hpp"
#include "../repositories/IDanceHallRepository.hpp"
#include "../repositories/IBookingRepository.hpp"
#include "../repositories/ILessonRepository.hpp"
#include "../repositories/ITrainerRepository.hpp"
#include "../repositories/IEnrollmentRepository.hpp"
#include "../repositories/ISubscriptionRepository.hpp"
#include "../repositories/ISubscriptionTypeRepository.hpp"
#include "../repositories/IReviewRepository.hpp"
#include "../repositories/IBranchRepository.hpp"
#include "../repositories/IStudioRepository.hpp"
#include "../repositories/IAttendanceRepository.hpp"

class IRepositoryFactory {
public:
    virtual ~IRepositoryFactory() = default;
    
    virtual std::shared_ptr<IClientRepository> createClientRepository() = 0;
    virtual std::shared_ptr<IDanceHallRepository> createDanceHallRepository() = 0;
    virtual std::shared_ptr<IBookingRepository> createBookingRepository() = 0;
    virtual std::shared_ptr<ILessonRepository> createLessonRepository() = 0;
    virtual std::shared_ptr<ITrainerRepository> createTrainerRepository() = 0;
    virtual std::shared_ptr<IEnrollmentRepository> createEnrollmentRepository() = 0;
    virtual std::shared_ptr<ISubscriptionRepository> createSubscriptionRepository() = 0;
    virtual std::shared_ptr<ISubscriptionTypeRepository> createSubscriptionTypeRepository() = 0;
    virtual std::shared_ptr<IReviewRepository> createReviewRepository() = 0;
    virtual std::shared_ptr<IBranchRepository> createBranchRepository() = 0;
    virtual std::shared_ptr<IStudioRepository> createStudioRepository() = 0;
    virtual std::shared_ptr<IAttendanceRepository> createAttendanceRepository() = 0;
    
    virtual bool testConnection() const = 0;
    virtual void reconnect() = 0;
};