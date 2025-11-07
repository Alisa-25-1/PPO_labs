#ifndef MONGODB_BOOKING_REPOSITORY_HPP
#define MONGODB_BOOKING_REPOSITORY_HPP

#include "../IBookingRepository.hpp"
#include "../../data/exceptions/DataAccessException.hpp" 
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>

class MongoDBRepositoryFactory;

class MongoDBBookingRepository : public IBookingRepository {
private:
    std::shared_ptr<MongoDBRepositoryFactory> factory_;
    mongocxx::collection getCollection() const;

public:
    explicit MongoDBBookingRepository(std::shared_ptr<MongoDBRepositoryFactory> factory);
    
    std::optional<Booking> findById(const UUID& id) override;
    std::vector<Booking> findByClientId(const UUID& clientId) override;
    std::vector<Booking> findByHallId(const UUID& hallId) override;
    std::vector<Booking> findConflictingBookings(const UUID& hallId, const TimeSlot& timeSlot) override;
    std::vector<Booking> findAll() override;
    bool save(const Booking& booking) override;
    bool update(const Booking& booking) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    Booking mapDocumentToBooking(const bsoncxx::document::view& doc) const;
    bsoncxx::document::value mapBookingToDocument(const Booking& booking) const;
};

#endif // MONGODB_BOOKING_REPOSITORY_HPP