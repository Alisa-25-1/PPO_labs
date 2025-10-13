#include "PostgreSQLTrainerRepository.hpp"
#include <pqxx/pqxx>
#include "../../data/QueryFactory.hpp"

PostgreSQLTrainerRepository::PostgreSQLTrainerRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Trainer> PostgreSQLTrainerRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "biography", "qualification_level", "is_active"})
            .from("trainers")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto trainer = mapResultToTrainer(result[0]);
        // Загружаем специализации
        auto specializations = getTrainerSpecializations(id);
        for (const auto& specialization : specializations) {
            trainer.addSpecialization(specialization);
        }
        
        dbConnection_->commitTransaction(work);
        return trainer;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find trainer by ID: ") + e.what());
    }
}

std::vector<Trainer> PostgreSQLTrainerRepository::findBySpecialization(const std::string& specialization) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = QueryFactory::createFindBySpecializationQuery();
        
        auto result = work.exec_params(query, specialization);
        
        std::vector<Trainer> trainers;
        for (const auto& row : result) {
            auto trainer = mapResultToTrainer(row);
            auto specializations = getTrainerSpecializations(trainer.getId());
            for (const auto& spec : specializations) {
                trainer.addSpecialization(spec);
            }
            trainers.push_back(trainer);
        }
        
        dbConnection_->commitTransaction(work);
        return trainers;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find trainers by specialization: ") + e.what());
    }
}

std::vector<Trainer> PostgreSQLTrainerRepository::findActiveTrainers() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "biography", "qualification_level", "is_active"})
            .from("trainers")
            .where("is_active = true")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Trainer> trainers;
        for (const auto& row : result) {
            auto trainer = mapResultToTrainer(row);
            auto specializations = getTrainerSpecializations(trainer.getId());
            for (const auto& specialization : specializations) {
                trainer.addSpecialization(specialization);
            }
            trainers.push_back(trainer);
        }
        
        dbConnection_->commitTransaction(work);
        return trainers;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find active trainers: ") + e.what());
    }
}

std::vector<Trainer> PostgreSQLTrainerRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "biography", "qualification_level", "is_active"})
            .from("trainers")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Trainer> trainers;
        for (const auto& row : result) {
            auto trainer = mapResultToTrainer(row);
            auto specializations = getTrainerSpecializations(trainer.getId());
            for (const auto& specialization : specializations) {
                trainer.addSpecialization(specialization);
            }
            trainers.push_back(trainer);
        }
        
        dbConnection_->commitTransaction(work);
        return trainers;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all trainers: ") + e.what());
    }
}

bool PostgreSQLTrainerRepository::save(const Trainer& trainer) {
    validateTrainer(trainer);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        // Сохраняем основную информацию о тренере
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"name", "$2"},
            {"biography", "$3"},
            {"qualification_level", "$4"},
            {"is_active", "$5"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("trainers")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            trainer.getId().toString(),
            trainer.getName(),
            trainer.getBiography(),
            trainer.getQualificationLevel(),
            trainer.isActive()
        );
        
        // Сохраняем специализации
        auto specializations = trainer.getSpecializations();
        for (const auto& specialization : specializations) {
            std::string specQuery = 
                "INSERT INTO trainer_specializations (trainer_id, specialization) "
                "VALUES ($1, $2)";
            work.exec_params(specQuery, trainer.getId().toString(), specialization);
        }
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save trainer: ") + e.what());
    }
}

bool PostgreSQLTrainerRepository::update(const Trainer& trainer) {
    validateTrainer(trainer);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        // Обновляем основную информацию о тренере
        std::map<std::string, std::string> values = {
            {"name", "$2"},
            {"biography", "$3"},
            {"qualification_level", "$4"},
            {"is_active", "$5"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("trainers")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            trainer.getId().toString(),
            trainer.getName(),
            trainer.getBiography(),
            trainer.getQualificationLevel(),
            trainer.isActive()
        );
        
        // Обновляем специализации: удаляем старые и добавляем новые
        work.exec_params("DELETE FROM trainer_specializations WHERE trainer_id = $1", 
                         trainer.getId().toString());
        
        auto specializations = trainer.getSpecializations();
        for (const auto& specialization : specializations) {
            std::string specQuery = 
                "INSERT INTO trainer_specializations (trainer_id, specialization) "
                "VALUES ($1, $2)";
            work.exec_params(specQuery, trainer.getId().toString(), specialization);
        }
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update trainer: ") + e.what());
    }
}

bool PostgreSQLTrainerRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        // Удаляем специализации сначала
        work.exec_params("DELETE FROM trainer_specializations WHERE trainer_id = $1", 
                         id.toString());
        
        // Затем удаляем тренера
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("trainers")
            .where("id = $1")
            .build();
        
            auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove trainer: ") + e.what());
    }
}

bool PostgreSQLTrainerRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("trainers")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check trainer existence: ") + e.what());
    }
}

Trainer PostgreSQLTrainerRepository::mapResultToTrainer(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::vector<std::string> specializations; // Будет заполнено отдельным запросом
    std::string biography = row["biography"].c_str();
    std::string qualificationLevel = row["qualification_level"].c_str();
    bool isActive = row["is_active"].as<bool>();
    
    Trainer trainer(id, name, specializations);
    trainer.setBiography(biography);
    trainer.setQualificationLevel(qualificationLevel);
    trainer.setActive(isActive);
    
    return trainer;
}

std::vector<std::string> PostgreSQLTrainerRepository::getTrainerSpecializations(const UUID& trainerId) const {
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::string query = QueryFactory::createGetTrainerSpecializationsQuery();
        
        auto result = work.exec_params(query, trainerId.toString());
        
        std::vector<std::string> specializations;
        for (const auto& row : result) {
            specializations.push_back(row["specialization"].c_str());
        }
        
        dbConnection_->commitTransaction(work);
        return specializations;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to get trainer specializations: ") + e.what());
    }
}

void PostgreSQLTrainerRepository::validateTrainer(const Trainer& trainer) const {
    if (!trainer.isValid()) {
        throw DataAccessException("Invalid trainer data");
    }
}