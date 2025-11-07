#include "MongoDBClientRepository.hpp"
#include "../../data/DateTimeUtils.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include <iostream>

MongoDBClientRepository::MongoDBClientRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBClientRepository::getCollection() const {
    return factory_->getDatabase().collection("clients");
}

std::optional<Client> MongoDBClientRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            return std::nullopt;
        }
        
        return mapDocumentToClient(result->view());
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find client by ID: ") + e.what());
    }
}

std::optional<Client> MongoDBClientRepository::findByEmail(const std::string& email) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "email" << email
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            return std::nullopt;
        }
        
        return mapDocumentToClient(result->view());
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findByEmail: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find client by email: ") + e.what());
    }
}

std::vector<Client> MongoDBClientRepository::findAll() {
    std::vector<Client> clients;
    
    try {
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        for (auto&& doc : cursor) {
            clients.push_back(mapDocumentToClient(doc));
        }
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all clients: ") + e.what());
    }
    
    return clients;
}

bool MongoDBClientRepository::save(const Client& client) {
    try {
        auto collection = getCollection();
        auto document = mapClientToDocument(client);
        
        auto result = collection.insert_one(document.view());
        return result && result->result().inserted_count() > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save client: ") + e.what());
    }
}

bool MongoDBClientRepository::update(const Client& client) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << client.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "name" << client.getName()
                << "email" << client.getEmail()
                << "phone" << client.getPhone()
                << "passwordHash" << client.getPasswordHash()
                << "accountStatus" << EnumUtils::accountStatusToString(client.getStatus())
                << "registrationDate" << DateTimeUtils::formatTimeForMongoDB(client.getRegistrationDate())
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        return result && result->modified_count() > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update client: ") + e.what());
    }
}

bool MongoDBClientRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        return result && result->deleted_count() > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove client: ") + e.what());
    }
}

bool MongoDBClientRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check client existence: ") + e.what());
    }
}

bool MongoDBClientRepository::existsByEmail(const std::string& email) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "email" << email
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Error in existsByEmail: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check email existence: ") + e.what());
    }
}

Client MongoDBClientRepository::mapDocumentToClient(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        std::string name = doc["name"].get_string().value.to_string();
        std::string email = doc["email"].get_string().value.to_string();
        std::string phone = doc["phone"].get_string().value.to_string();
        std::string passwordHash = doc["passwordHash"].get_string().value.to_string();
        
        std::string statusStr = doc["accountStatus"].get_string().value.to_string();
        AccountStatus status = AccountStatus::ACTIVE;
        if (statusStr == "INACTIVE") status = AccountStatus::INACTIVE;
        else if (statusStr == "SUSPENDED") status = AccountStatus::SUSPENDED;
        
        auto registrationDate = DateTimeUtils::parseTimeFromMongoDB(
            doc["registrationDate"].get_string().value.to_string()
        );
        
        Client client(id, name, email, phone);
        client.setPasswordHash(passwordHash);
        client.setRegistrationDate(registrationDate); 
        
        // Устанавливаем статус
        switch (status) {
            case AccountStatus::ACTIVE: client.activate(); break;
            case AccountStatus::INACTIVE: client.deactivate(); break;
            case AccountStatus::SUSPENDED: client.suspend(); break;
        }
        
        return client;
    } catch (const std::exception& e) {
        std::cerr << "Error mapping document to Client: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Client");
    }
}

bsoncxx::document::value MongoDBClientRepository::mapClientToDocument(const Client& client) const {
    return bsoncxx::builder::stream::document{}
        << "id" << client.getId().toString()
        << "name" << client.getName()
        << "email" << client.getEmail()
        << "phone" << client.getPhone()
        << "passwordHash" << client.getPasswordHash()
        << "accountStatus" << EnumUtils::accountStatusToString(client.getStatus())
        << "registrationDate" << DateTimeUtils::formatTimeForMongoDB(client.getRegistrationDate())
        << bsoncxx::builder::stream::finalize;
}