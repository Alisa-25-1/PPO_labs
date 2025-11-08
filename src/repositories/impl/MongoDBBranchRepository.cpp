#include "MongoDBBranchRepository.hpp"
#include "../../data/DateTimeUtils.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include <iostream>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/json.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::sub_array;
using bsoncxx::builder::basic::sub_document;

MongoDBBranchRepository::MongoDBBranchRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBBranchRepository::getCollection() const {
    return factory_->getDatabase().collection("branches");
}

std::optional<Branch> MongoDBBranchRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Филиал не найден в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Филиал найден в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToBranch(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find branch by ID: ") + e.what());
    }
}

std::vector<Branch> MongoDBBranchRepository::findByStudioId(const UUID& studioId) {
    std::vector<Branch> branches;
    
    try {
        std::cout << "🔍 Поиск филиалов для студии в MongoDB: " << studioId.toString() << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "studioId" << studioId.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto branch = mapDocumentToBranch(doc);
                branches.push_back(branch);
                count++;
                std::cout << "✅ Успешно создан филиал из MongoDB: " << branch.getName() 
                          << " (ID: " << branch.getId().toString() << ")" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания филиала из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено филиалов в MongoDB: " << count << std::endl;
        return branches;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findByStudioId: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find branches by studio ID: ") + e.what());
    }
}

std::vector<Branch> MongoDBBranchRepository::findAll() {
    std::vector<Branch> branches;
    
    try {
        std::cout << "🔍 Получение всех филиалов из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto branch = mapDocumentToBranch(doc);
                branches.push_back(branch);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге филиала из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено филиалов из MongoDB: " << count << std::endl;
        return branches;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all branches: ") + e.what());
    }
}

bool MongoDBBranchRepository::save(const Branch& branch) {
    validateBranch(branch);
    
    try {
        auto collection = getCollection();
        auto document = mapBranchToDocument(branch);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Филиал успешно сохранен в MongoDB: " << branch.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить филиал в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save branch: ") + e.what());
    }
}

bool MongoDBBranchRepository::update(const Branch& branch) {
    validateBranch(branch);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << branch.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "name" << branch.getName()
                << "phone" << branch.getPhone()
                << "openTime" << static_cast<int>(branch.getWorkingHours().openTime.count())
                << "closeTime" << static_cast<int>(branch.getWorkingHours().closeTime.count())
                << "studioId" << branch.getStudioId().toString()
                << "address" << mapAddressToDocument(branch.getAddress())
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Филиал успешно обновлен в MongoDB: " << branch.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Филиал не найден для обновления в MongoDB: " << branch.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update branch: ") + e.what());
    }
}

bool MongoDBBranchRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Филиал успешно удален из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Филиал не найден для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove branch: ") + e.what());
    }
}

bool MongoDBBranchRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check branch existence: ") + e.what());
    }
}

bsoncxx::document::value MongoDBBranchRepository::mapBranchToDocument(const Branch& branch) const {
    bsoncxx::builder::basic::document builder;
    
    builder.append(
        kvp("id", branch.getId().toString()),
        kvp("name", branch.getName()),
        kvp("phone", branch.getPhone()),
        kvp("openTime", static_cast<int>(branch.getWorkingHours().openTime.count())),
        kvp("closeTime", static_cast<int>(branch.getWorkingHours().closeTime.count())),
        kvp("studioId", branch.getStudioId().toString()),
        kvp("address", [&branch](sub_document sub_doc) {
            sub_doc.append(
                kvp("id", branch.getAddress().getId().toString()),
                kvp("country", branch.getAddress().getCountry()),
                kvp("city", branch.getAddress().getCity()),
                kvp("street", branch.getAddress().getStreet()),
                kvp("building", branch.getAddress().getBuilding()),
                kvp("timezoneOffset", static_cast<int>(branch.getAddress().getTimezoneOffset().count()))
            );
            
            if (!branch.getAddress().getApartment().empty()) {
                sub_doc.append(kvp("apartment", branch.getAddress().getApartment()));
            }
            
            if (!branch.getAddress().getPostalCode().empty()) {
                sub_doc.append(kvp("postalCode", branch.getAddress().getPostalCode()));
            }
        })
    );
    
    return builder.extract();
}

Branch MongoDBBranchRepository::mapDocumentToBranch(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        std::string name = doc["name"].get_string().value.to_string();
        std::string phone = doc["phone"].get_string().value.to_string();
        
        int openTimeHours = doc["openTime"].get_int32();
        int closeTimeHours = doc["closeTime"].get_int32();
        WorkingHours workingHours{std::chrono::hours(openTimeHours), std::chrono::hours(closeTimeHours)};
        
        UUID studioId = UUID::fromString(doc["studioId"].get_string().value.to_string());
        
        // Получаем адрес как вложенный документ
        auto addressDoc = doc["address"].get_document().view();
        
        UUID addressId = UUID::fromString(addressDoc["id"].get_string().value.to_string());
        std::string country = addressDoc["country"].get_string().value.to_string();
        std::string city = addressDoc["city"].get_string().value.to_string();
        std::string street = addressDoc["street"].get_string().value.to_string();
        std::string building = addressDoc["building"].get_string().value.to_string();
        
        int timezoneOffsetMinutes = addressDoc["timezoneOffset"].get_int32();
        auto timezoneOffset = std::chrono::minutes(timezoneOffsetMinutes);
        
        BranchAddress address(addressId, country, city, street, building, timezoneOffset);
        
        // Опциональные поля
        if (addressDoc["apartment"]) {
            address.setApartment(addressDoc["apartment"].get_string().value.to_string());
        }
        
        if (addressDoc["postalCode"]) {
            address.setPostalCode(addressDoc["postalCode"].get_string().value.to_string());
        }
        
        // Создаем филиал
        Branch branch(id, name, phone, workingHours, studioId, address);
        
        // Проверяем валидность
        if (!branch.isValid()) {
            std::string error = "Invalid branch data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", name=" + name;
            error += ", phone=" + phone;
            error += ", studioId=" + studioId.toString();
            throw std::invalid_argument(error);
        }
        
        return branch;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Branch из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Branch");
    }
}

bsoncxx::document::value MongoDBBranchRepository::mapAddressToDocument(const BranchAddress& address) const {
    auto builder = bsoncxx::builder::stream::document{}
        << "id" << address.getId().toString()
        << "country" << address.getCountry()
        << "city" << address.getCity()
        << "street" << address.getStreet()
        << "building" << address.getBuilding()
        << "timezoneOffset" << static_cast<int>(address.getTimezoneOffset().count());
    
    if (!address.getApartment().empty()) {
        builder << "apartment" << address.getApartment();
    }
    
    if (!address.getPostalCode().empty()) {
        builder << "postalCode" << address.getPostalCode();
    }
    
    return builder << bsoncxx::builder::stream::finalize;
}

void MongoDBBranchRepository::validateBranch(const Branch& branch) const {
    if (!branch.isValid()) {
        throw DataAccessException("Invalid branch data for MongoDB");
    }
}