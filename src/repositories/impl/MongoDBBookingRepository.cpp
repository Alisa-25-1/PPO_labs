#include "MongoDBBookingRepository.hpp"
#include "../../data/DateTimeUtils.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include <iostream>

MongoDBBookingRepository::MongoDBBookingRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBBookingRepository::getCollection() const {
    return factory_->getDatabase().collection("bookings");
}

std::optional<Booking> MongoDBBookingRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            return std::nullopt;
        }
        
        return mapDocumentToBooking(result->view());
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find booking by ID: ") + e.what());
    }
}

std::vector<Booking> MongoDBBookingRepository::findByClientId(const UUID& clientId) {
    std::vector<Booking> bookings;
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "clientId" << clientId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        for (auto&& doc : cursor) {
            bookings.push_back(mapDocumentToBooking(doc));
        }
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findByClientId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find bookings by client ID: ") + e.what());
    }
    
    return bookings;
}

std::vector<Booking> MongoDBBookingRepository::findByHallId(const UUID& hallId) {
    std::vector<Booking> bookings;
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "hallId" << hallId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        for (auto&& doc : cursor) {
            bookings.push_back(mapDocumentToBooking(doc));
        }
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findByHallId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find bookings by hall ID: ") + e.what());
    }
    
    return bookings;
}

std::vector<Booking> MongoDBBookingRepository::findConflictingBookings(const UUID& hallId, const TimeSlot& timeSlot) {
    std::vector<Booking> bookings;
    
    try {
        auto collection = getCollection();
        
        // MongoDB query для поиска конфликтующих бронирований
        auto filter = bsoncxx::builder::stream::document{}
            << "hallId" << hallId.toString()
            << "status" << bsoncxx::builder::stream::open_document
                << "$in" << bsoncxx::builder::stream::open_array
                    << "PENDING" << "CONFIRMED"
                << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::close_document
            << "$or" << bsoncxx::builder::stream::open_array
                << bsoncxx::builder::stream::open_document
                    << "startTime" << bsoncxx::builder::stream::open_document
                        << "$lt" << DateTimeUtils::formatTimeForMongoDB(timeSlot.getEndTime())
                    << bsoncxx::builder::stream::close_document
                    << "endTime" << bsoncxx::builder::stream::open_document
                        << "$gt" << DateTimeUtils::formatTimeForMongoDB(timeSlot.getStartTime())
                    << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        for (auto&& doc : cursor) {
            bookings.push_back(mapDocumentToBooking(doc));
        }
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findConflictingBookings: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find conflicting bookings: ") + e.what());
    }
    
    return bookings;
}

std::vector<Booking> MongoDBBookingRepository::findAll() {
    std::vector<Booking> bookings;
    
    try {
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        for (auto&& doc : cursor) {
            bookings.push_back(mapDocumentToBooking(doc));
        }
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all bookings: ") + e.what());
    }
    
    return bookings;
}

bool MongoDBBookingRepository::save(const Booking& booking) {
    try {
        auto collection = getCollection();
        auto document = mapBookingToDocument(booking);
        
        auto result = collection.insert_one(document.view());
        return result && result->result().inserted_count() > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save booking: ") + e.what());
    }
}

bool MongoDBBookingRepository::update(const Booking& booking) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << booking.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "clientId" << booking.getClientId().toString()
                << "hallId" << booking.getHallId().toString()
                << "startTime" << DateTimeUtils::formatTimeForMongoDB(booking.getTimeSlot().getStartTime())
                << "endTime" << DateTimeUtils::formatTimeForMongoDB(booking.getTimeSlot().getEndTime())
                << "durationMinutes" << booking.getTimeSlot().getDurationMinutes()
                << "purpose" << booking.getPurpose()
                << "status" << EnumUtils::bookingStatusToString(booking.getStatus())
                << "createdAt" << DateTimeUtils::formatTimeForMongoDB(booking.getCreatedAt())
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        return result && result->modified_count() > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update booking: ") + e.what());
    }
}

bool MongoDBBookingRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        return result && result->deleted_count() > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove booking: ") + e.what());
    }
}

bool MongoDBBookingRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check booking existence: ") + e.what());
    }
}

Booking MongoDBBookingRepository::mapDocumentToBooking(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        UUID clientId = UUID::fromString(doc["clientId"].get_string().value.to_string());
        UUID hallId = UUID::fromString(doc["hallId"].get_string().value.to_string());
        
        auto startTime = DateTimeUtils::parseTimeFromMongoDB(doc["startTime"].get_string().value.to_string());
        int durationMinutes = doc["durationMinutes"].get_int32();
        TimeSlot timeSlot(startTime, durationMinutes);
        
        std::string purpose = doc["purpose"].get_string().value.to_string();
        
        Booking booking(id, clientId, hallId, timeSlot, purpose);
        
        // Устанавливаем статус
        std::string statusStr = doc["status"].get_string().value.to_string();
        if (statusStr == "CONFIRMED") booking.confirm();
        else if (statusStr == "CANCELLED") booking.cancel();
        else if (statusStr == "COMPLETED") booking.complete();
        
        return booking;
    } catch (const std::exception& e) {
        std::cerr << "Error mapping document to Booking: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Booking");
    }
}

bsoncxx::document::value MongoDBBookingRepository::mapBookingToDocument(const Booking& booking) const {
    return bsoncxx::builder::stream::document{}
        << "id" << booking.getId().toString()
        << "clientId" << booking.getClientId().toString()
        << "hallId" << booking.getHallId().toString()
        << "startTime" << DateTimeUtils::formatTimeForMongoDB(booking.getTimeSlot().getStartTime())
        << "endTime" << DateTimeUtils::formatTimeForMongoDB(booking.getTimeSlot().getEndTime())
        << "durationMinutes" << booking.getTimeSlot().getDurationMinutes()
        << "purpose" << booking.getPurpose()
        << "status" << EnumUtils::bookingStatusToString(booking.getStatus())
        << "createdAt" << DateTimeUtils::formatTimeForMongoDB(booking.getCreatedAt())
        << bsoncxx::builder::stream::finalize;
}