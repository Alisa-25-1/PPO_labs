#include "MongoDBTrainerRepository.hpp"
#include "../../data/MongoDBRepositoryFactory.hpp"
#include "../../data/DateTimeUtils.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <iostream>

MongoDBTrainerRepository::MongoDBTrainerRepository(std::shared_ptr<MongoDBRepositoryFactory> factory)
    : factory_(std::move(factory)) {}

mongocxx::collection MongoDBTrainerRepository::getCollection() const {
    return factory_->getDatabase().collection("trainers");
}

std::optional<Trainer> MongoDBTrainerRepository::findById(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.find_one(filter.view());
        
        if (!result) {
            std::cout << "❌ Тренер не найден в MongoDB: " << id.toString() << std::endl;
            return std::nullopt;
        }
        
        std::cout << "✅ Тренер найден в MongoDB: " << id.toString() << std::endl;
        return mapDocumentToTrainer(result->view());
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findById: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find trainer by ID: ") + e.what());
    }
}

std::vector<Trainer> MongoDBTrainerRepository::findBySpecialization(const std::string& specialization) {
    std::vector<Trainer> trainers;
    
    try {
        std::cout << "🔍 Поиск тренеров по специализации в MongoDB: " << specialization << std::endl;
        
        auto collection = getCollection();
        
        // В MongoDB используем оператор $in для поиска в массиве
        auto filter = bsoncxx::builder::stream::document{}
            << "specializations" << bsoncxx::builder::stream::open_document
                << "$in" << bsoncxx::builder::stream::open_array
                    << specialization
                << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::close_document
            << "isActive" << true
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto trainer = mapDocumentToTrainer(doc);
                trainers.push_back(trainer);
                count++;
                std::cout << "✅ Найден тренер: " << trainer.getName() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка создания тренера из MongoDB документа: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "📊 Найдено тренеров в MongoDB: " << count << std::endl;
        return trainers;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findBySpecialization: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find trainers by specialization: ") + e.what());
    }
}

std::vector<Trainer> MongoDBTrainerRepository::findActiveTrainers() {
    std::vector<Trainer> trainers;
    
    try {
        std::cout << "🔍 Поиск активных тренеров в MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "isActive" << true
            << bsoncxx::builder::stream::finalize;
        
        auto cursor = collection.find(filter.view());
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto trainer = mapDocumentToTrainer(doc);
                trainers.push_back(trainer);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге тренера из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено активных тренеров из MongoDB: " << count << std::endl;
        return trainers;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findActiveTrainers: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find active trainers: ") + e.what());
    }
}

std::vector<Trainer> MongoDBTrainerRepository::findAll() {
    std::vector<Trainer> trainers;
    
    try {
        std::cout << "🔍 Получение всех тренеров из MongoDB" << std::endl;
        
        auto collection = getCollection();
        auto cursor = collection.find({});
        
        int count = 0;
        for (auto&& doc : cursor) {
            try {
                auto trainer = mapDocumentToTrainer(doc);
                trainers.push_back(trainer);
                count++;
            } catch (const std::exception& e) {
                std::cerr << "❌ Ошибка при маппинге тренера из MongoDB: " << e.what() << std::endl;
                continue;
            }
        }
        
        std::cout << "✅ Успешно загружено тренеров из MongoDB: " << count << std::endl;
        return trainers;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in findAll: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to find all trainers: ") + e.what());
    }
}

bool MongoDBTrainerRepository::save(const Trainer& trainer) {
    validateTrainer(trainer);
    
    try {
        auto collection = getCollection();
        auto document = mapTrainerToDocument(trainer);
        
        auto result = collection.insert_one(document.view());
        
        if (result && result->result().inserted_count() > 0) {
            std::cout << "✅ Тренер успешно сохранен в MongoDB: " << trainer.getId().toString() << std::endl;
            return true;
        }
        
        std::cerr << "❌ Не удалось сохранить тренера в MongoDB" << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in save: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to save trainer: ") + e.what());
    }
}

bool MongoDBTrainerRepository::update(const Trainer& trainer) {
    validateTrainer(trainer);
    
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << trainer.getId().toString()
            << bsoncxx::builder::stream::finalize;
        
        // Создаем массив специализаций с помощью basic builder
        bsoncxx::builder::basic::array specializations_array;
        for (const auto& spec : trainer.getSpecializations()) {
            specializations_array.append(spec);
        }
        
        auto update_doc = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
                << "name" << trainer.getName()
                << "biography" << trainer.getBiography()
                << "qualificationLevel" << trainer.getQualificationLevel()
                << "isActive" << trainer.isActive()
                << "specializations" << specializations_array
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.update_one(filter.view(), update_doc.view());
        
        if (result && result->modified_count() > 0) {
            std::cout << "✅ Тренер успешно обновлен в MongoDB: " << trainer.getId().toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Тренер не найден для обновления в MongoDB: " << trainer.getId().toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in update: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to update trainer: ") + e.what());
    }
}

bool MongoDBTrainerRepository::remove(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.delete_one(filter.view());
        
        if (result && result->deleted_count() > 0) {
            std::cout << "✅ Тренер успешно удален из MongoDB: " << id.toString() << std::endl;
            return true;
        }
        
        std::cout << "⚠️  Тренер не найден для удаления в MongoDB: " << id.toString() << std::endl;
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in remove: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to remove trainer: ") + e.what());
    }
}

bool MongoDBTrainerRepository::exists(const UUID& id) {
    try {
        auto collection = getCollection();
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id.toString()
            << bsoncxx::builder::stream::finalize;
        
        auto result = collection.count_documents(filter.view());
        return result > 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ MongoDB Error in exists: " << e.what() << std::endl;
        throw DataAccessException(std::string("Failed to check trainer existence: ") + e.what());
    }
}

Trainer MongoDBTrainerRepository::mapDocumentToTrainer(const bsoncxx::document::view& doc) const {
    try {
        UUID id = UUID::fromString(doc["id"].get_string().value.to_string());
        std::string name = doc["name"].get_string().value.to_string();
        std::string biography = doc["biography"].get_string().value.to_string();
        std::string qualificationLevel = doc["qualificationLevel"].get_string().value.to_string();
        bool isActive = doc["isActive"].get_bool().value;
        
        // Собираем специализации из массива
        std::vector<std::string> specializations;
        if (doc["specializations"]) {
            auto specializationsArray = doc["specializations"].get_array().value;
            for (auto&& specElem : specializationsArray) {
                specializations.push_back(specElem.get_string().value.to_string());
            }
        }
        
        // Создаем тренера
        Trainer trainer(id, name, specializations);
        trainer.setBiography(biography);
        trainer.setQualificationLevel(qualificationLevel);
        trainer.setActive(isActive);
        
        // Проверяем валидность
        if (!trainer.isValid()) {
            std::string error = "Invalid trainer data from MongoDB: ";
            error += "id=" + id.toString();
            error += ", name=" + name;
            error += ", qualificationLevel=" + qualificationLevel;
            throw std::invalid_argument(error);
        }
        
        return trainer;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Критическая ошибка маппинга Trainer из MongoDB: " << e.what() << std::endl;
        throw DataAccessException("Failed to map MongoDB document to Trainer");
    }
}

bsoncxx::document::value MongoDBTrainerRepository::mapTrainerToDocument(const Trainer& trainer) const {
    auto builder = bsoncxx::builder::basic::document{};
    
    builder.append(
        bsoncxx::builder::basic::kvp("id", trainer.getId().toString()),
        bsoncxx::builder::basic::kvp("name", trainer.getName()),
        bsoncxx::builder::basic::kvp("biography", trainer.getBiography()),
        bsoncxx::builder::basic::kvp("qualificationLevel", trainer.getQualificationLevel()),
        bsoncxx::builder::basic::kvp("isActive", trainer.isActive())
    );
    
    // Создаем массив специализаций
    auto specializations_array = bsoncxx::builder::basic::array{};
    for (const auto& specialization : trainer.getSpecializations()) {
        specializations_array.append(specialization);
    }
    
    builder.append(bsoncxx::builder::basic::kvp("specializations", specializations_array));
    
    return builder.extract();
}

void MongoDBTrainerRepository::validateTrainer(const Trainer& trainer) const {
    if (!trainer.isValid()) {
        throw DataAccessException("Invalid trainer data for MongoDB");
    }
}