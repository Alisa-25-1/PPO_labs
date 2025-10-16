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
            .select({"t.id", "t.name", "t.biography", "t.qualification_level", "t.is_active", "ts.specialization"})
            .from("trainers t")
            .leftJoin("trainer_specializations ts", "t.id = ts.trainer_id")
            .where("t.id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            dbConnection_->commitTransaction(work);
            return std::nullopt;
        }
        
        auto trainer = mapResultToTrainer(result[0]);
        
        // Собираем специализации из всех строк
        for (const auto& row : result) {
            if (!row["specialization"].is_null()) {
                std::string specialization = row["specialization"].c_str();
                trainer.addSpecialization(specialization);
            }
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
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"t.id", "t.name", "t.biography", "t.qualification_level", "t.is_active", "ts.specialization"})
            .from("trainers t")
            .innerJoin("trainer_specializations ts", "t.id = ts.trainer_id")
            .where("ts.specialization = $1 AND t.is_active = true")
            .build();
        
        auto result = work.exec_params(query, specialization);
        
        std::vector<Trainer> trainers;
        std::map<UUID, Trainer> trainerMap;
        
        for (const auto& row : result) {
            UUID id = UUID::fromString(row["id"].c_str());
            
            if (trainerMap.find(id) == trainerMap.end()) {
                auto trainer = mapResultToTrainer(row);
                trainerMap[id] = trainer;
            }
            
            if (!row["specialization"].is_null()) {
                std::string spec = row["specialization"].c_str();
                trainerMap[id].addSpecialization(spec);
            }
        }
        
        for (auto& pair : trainerMap) {
            trainers.push_back(pair.second);
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
            .select({"t.id", "t.name", "t.biography", "t.qualification_level", "t.is_active", "ts.specialization"})
            .from("trainers t")
            .leftJoin("trainer_specializations ts", "t.id = ts.trainer_id")
            .where("t.is_active = true")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Trainer> trainers;
        std::map<UUID, Trainer> trainerMap;
        
        for (const auto& row : result) {
            UUID id = UUID::fromString(row["id"].c_str());
            
            if (trainerMap.find(id) == trainerMap.end()) {
                auto trainer = mapResultToTrainer(row);
                trainerMap[id] = trainer;
            }
            
            if (!row["specialization"].is_null()) {
                std::string specialization = row["specialization"].c_str();
                trainerMap[id].addSpecialization(specialization);
            }
        }
        
        for (auto& pair : trainerMap) {
            trainers.push_back(pair.second);
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
            .select({"t.id", "t.name", "t.biography", "t.qualification_level", "t.is_active", "ts.specialization"})
            .from("trainers t")
            .leftJoin("trainer_specializations ts", "t.id = ts.trainer_id")
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Trainer> trainers;
        std::map<UUID, Trainer> trainerMap;
        
        for (const auto& row : result) {
            UUID id = UUID::fromString(row["id"].c_str());
            
            if (trainerMap.find(id) == trainerMap.end()) {
                auto trainer = mapResultToTrainer(row);
                trainerMap[id] = trainer;
            }
            
            if (!row["specialization"].is_null()) {
                std::string specialization = row["specialization"].c_str();
                trainerMap[id].addSpecialization(specialization);
            }
        }
        
        for (auto& pair : trainerMap) {
            trainers.push_back(pair.second);
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
            SqlQueryBuilder specQueryBuilder;
            std::string specQuery = specQueryBuilder
                .insertInto("trainer_specializations")
                .values({{"trainer_id", "$1"}, {"specialization", "$2"}})
                .build();
            
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
        SqlQueryBuilder deleteBuilder;
        std::string deleteQuery = deleteBuilder
            .deleteFrom("trainer_specializations")
            .where("trainer_id = $1")
            .build();
        work.exec_params(deleteQuery, trainer.getId().toString());
        
        auto specializations = trainer.getSpecializations();
        for (const auto& specialization : specializations) {
            SqlQueryBuilder specQueryBuilder;
            std::string specQuery = specQueryBuilder
                .insertInto("trainer_specializations")
                .values({{"trainer_id", "$1"}, {"specialization", "$2"}})
                .build();
            
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
        SqlQueryBuilder deleteSpecBuilder;
        std::string deleteSpecQuery = deleteSpecBuilder
            .deleteFrom("trainer_specializations")
            .where("trainer_id = $1")
            .build();
        work.exec_params(deleteSpecQuery, id.toString());
        
        // Затем удаляем тренера
        SqlQueryBuilder deleteBuilder;
        std::string query = deleteBuilder
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
    std::vector<std::string> specializations; // Будет заполнено отдельно
    std::string biography = row["biography"].c_str();
    std::string qualificationLevel = row["qualification_level"].c_str();
    bool isActive = row["is_active"].as<bool>();
    
    Trainer trainer(id, name, specializations);
    trainer.setBiography(biography);
    trainer.setQualificationLevel(qualificationLevel);
    trainer.setActive(isActive);
    
    return trainer;
}

void PostgreSQLTrainerRepository::validateTrainer(const Trainer& trainer) const {
    if (!trainer.isValid()) {
        throw DataAccessException("Invalid trainer data");
    }
}