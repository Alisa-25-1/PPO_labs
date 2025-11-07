#include "MongoDBStudioRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <iostream>

MongoDBStudioRepository::MongoDBStudioRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBStudioRepository::getCollection() const {
    return factory_->getDatabase().collection("studios");
}

std::optional<Studio> MongoDBStudioRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Студия не найдена в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Студия найдена в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToStudio(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find studio by ID: ") + e.what());
    }
}

std::optional<Studio> MongoDBStudioRepository::findMainStudio() {
    try {
        std::cout << "🔍 Поиск основной студии в MongoDB" << std::endl;
        
        auto collection = getCollection();
        
        // Сортируем по ID и берем первую запись как основную студию
        auto options = mongocxx::options::find{};
        options.sort(bsoncxx::builder::stream::document{} << "id" << 1 << bsoncxx::builder::stream::finalize);
        options.limit(1);
        
        auto cursor = collection.find({}, options);
        
        for (auto&& doc : cursor) {
            std::cout << "✅ Основная студия найдена в MongoDB" << std::endl;
            return mapDocumentToStudio(doc);
        }
        
        std::cout << "❌ Основная студия не найдена в MongoDB" << std::endl;
        return std::nullopt;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findMainStudio: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find main studio: ") + e.what());
    }
}

std::vector<Studio> MongoDBStudioRepository::findAll() {
    std::vector<Studio> studios;
    
    try {
        std::cout << "🔍 Получение всех студий из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto studio = mapDocumentToStudio(doc);
                studios.push_back(studio);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге студии из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено студий из MongoDB: " << count << std::endl;
        return studios;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all studios: ") + e.what());
    }
}

bool MongoDBStudioRepository::save(const Studio& studio) {
    validateStudio(studio);
    
    try {
        auto collection = getCollection();
        auto document = mapStudioToDocument(studio);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Студия успешно сохранена в MongoDB: " << studio.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить студию в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save studio: ") + e.what());
    }
}

bool MongoDBStudioRepository::update(const Studio& studio) {
    validateStudio(studio);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << studio.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "name" << studio.getName()
                << "description" << studio.getDescription()
                << "contactEmail" << studio.getContactEmail()
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Студия успешно обновлена в MongoDB: " << studio.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Студия не найдена для обновления в MongoDB: " << studio.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update studio: ") + e.what());
    }
}

bool MongoDBStudioRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Студия успешно удалена из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Студия не найдена для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove studio: ") + e.what());
    }
}

bool MongoDBStudioRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check studio existence: ") + e.what());
    }
}

Studio MongoDBStudioRepository::mapDocumentToStudio(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        std::string name = doc["name"].get_string().value.to_string();
        std::string description = doc["description"].get_string().value.to_string();
        std::string contactEmail = doc["contactEmail"].get_string().value.to_string();
        
        // Создаем студию
        Studio studio(id, name, contactEmail);
        studio.setDescription(description);
        
        // Загружаем branchIds если они есть
        if (doc["branchIds"]) {
            auto branchIdsArray = doc["branchIds"].get_array().value;
            for (auto&& branchIdElem : branchIdsArray) {
                UUID branchId = UUID::fromString(branchIdElem.get_string().value.to_string());
                studio.addBranch(branchId);
            }
        }
        
        // Проверяем валидность
        if (!studio.isValid()) {
            std::string error = "Invalid studio data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", name=" + name;
            error += ", contactEmail=" + contactEmail;
            throw std::invalid_argument(error);
        }
        
        return studio;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Studio из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Studio");
    }
}

bsoncxx::document::value MongoDBStudioRepository::mapStudioToDocument(const Studio& studio) const {
    auto builder = bsoncxx::builder::basic::document{};
    
    builder.append(
        bsoncxx::builder::basic::kvp("id", studio.getId().toString()),
        bsoncxx::builder::basic::kvp("name", studio.getName()),
        bsoncxx::builder::basic::kvp("description", studio.getDescription()),
        bsoncxx::builder::basic::kvp("contactEmail", studio.getContactEmail())
    );
    
    // Создаем массив branchIds
    auto branch_ids_array = bsoncxx::builder::basic::array{};
    for (const auto& branchId : studio.getBranchIds()) {
        branch_ids_array.append(branchId.toString());
    }
    
    builder.append(bsoncxx::builder::basic::kvp("branchIds", branch_ids_array));
    
    return builder.extract();
}

void MongoDBStudioRepository::validateStudio(const Studio& studio) const {
    if (!studio.isValid()) {
        throw DataAccessException("Invalid studio data for MongoDB");
    }
}