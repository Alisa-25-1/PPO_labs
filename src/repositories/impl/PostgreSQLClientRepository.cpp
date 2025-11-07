#include "PostgreSQLClientRepository.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include "../../data/DateTimeUtils.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include "../../services/exceptions/ValidationException.hpp" 

PostgreSQLClientRepository::PostgreSQLClientRepository(
    std::shared_ptr<DatabaseConnection> dbConnection)
    : dbConnection_(std::move(dbConnection)) {}

std::optional<Client> PostgreSQLClientRepository::findById(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "email", "phone", "password_hash", "registration_date", "status"})
            .from("clients")
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(query, id.toString());
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        auto client = mapResultToClient(result[0]);
        dbConnection_->commitTransaction(work);
        return client;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find client by ID: ") + e.what());
    }
}

std::optional<Client> PostgreSQLClientRepository::findByEmail(const std::string& email) {
    std::cout << "🔍 PostgreSQLClientRepository::findByEmail - Поиск по email: " << email << std::endl;
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "email", "phone", "password_hash", "registration_date", "status"})
            .from("clients")
            .where("LOWER(email) = LOWER($1)")
            .build();
        
        std::cout << "🔍 PostgreSQLClientRepository::findByEmail - SQL запрос: " << query << std::endl;
        
        auto result = work.exec_params(query, email);
        
        if (result.empty()) {
            std::cout << "❌ PostgreSQLClientRepository::findByEmail - Клиент не найден в БД: " << email << std::endl;
            dbConnection_->commitTransaction(work);
            return std::nullopt;
        }
        
        std::cout << "✅ PostgreSQLClientRepository::findByEmail - Найдена запись в БД" << std::endl;
        std::cout << "🔍 PostgreSQLClientRepository::findByEmail - Данные из БД:" << std::endl;
        std::cout << "   ID: " << result[0]["id"].c_str() << std::endl;
        std::cout << "   Name: " << result[0]["name"].c_str() << std::endl;
        std::cout << "   Email: " << result[0]["email"].c_str() << std::endl;
        std::cout << "   Password Hash: " << result[0]["password_hash"].c_str() << std::endl;
        std::cout << "   Status: " << result[0]["status"].c_str() << std::endl;
        
        auto client = mapResultToClient(result[0]);
        dbConnection_->commitTransaction(work);
        
        std::cout << "✅ PostgreSQLClientRepository::findByEmail - Успешно создан объект Client" << std::endl;
        return client;
        
    } catch (const pqxx::unique_violation& e) {
        throw UniqueViolationException("Email already exists: " + std::string(e.what()));
    } catch (const pqxx::foreign_key_violation& e) {
        throw ForeignKeyViolationException("Foreign key violation: " + std::string(e.what()));
    } catch (const std::exception& e) {
        std::cerr << "❌ PostgreSQLClientRepository::findByEmail - Ошибка: " << e.what() << std::endl;
        throw QueryException(std::string("Failed to find client by email: ") + e.what());
    }
}

std::vector<Client> PostgreSQLClientRepository::findAll() {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"id", "name", "email", "phone", "password_hash", "registration_date", "status"})
            .from("clients")
            .orderBy("registration_date", false)
            .build();
        
        auto result = work.exec(query);
        
        std::vector<Client> clients;
        for (const auto& row : result) {
            clients.push_back(mapResultToClient(row));
        }
        
        dbConnection_->commitTransaction(work);
        return clients;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to find all clients: ") + e.what());
    }
}

bool PostgreSQLClientRepository::save(const Client& client) {
    validateClient(client);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"id", "$1"},
            {"name", "$2"},
            {"email", "$3"},
            {"phone", "$4"},
            {"password_hash", "$5"},
            {"registration_date", "$6"},
            {"status", "$7"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .insertInto("clients")
            .values(values)
            .build();
        
        work.exec_params(
            query,
            client.getId().toString(),
            client.getName(),
            client.getEmail(),
            client.getPhone(),
            client.getPasswordHash(),
            DateTimeUtils::formatTimeForPostgres(client.getRegistrationDate()),
            clientStatusToString(client.getStatus())
        );
        
        dbConnection_->commitTransaction(work);
        return true;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to save client: ") + e.what());
    }
}

bool PostgreSQLClientRepository::update(const Client& client) {
    validateClient(client);
    
    try {
        auto work = dbConnection_->beginTransaction();
        
        std::map<std::string, std::string> values = {
            {"name", "$2"},
            {"email", "$3"},
            {"phone", "$4"},
            {"password_hash", "$5"},
            {"status", "$6"}
        };
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .update("clients")
            .set(values)
            .where("id = $1")
            .build();
        
        auto result = work.exec_params(
            query,
            client.getId().toString(),
            client.getName(),
            client.getEmail(),
            client.getPhone(),
            client.getPasswordHash(),
            clientStatusToString(client.getStatus())
        );
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to update client: ") + e.what());
    }
}

bool PostgreSQLClientRepository::remove(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .deleteFrom("clients")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return result.affected_rows() > 0;
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to remove client: ") + e.what());
    }
}

bool PostgreSQLClientRepository::exists(const UUID& id) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("clients")
            .where("id = $1")
            .build();
            
        auto result = work.exec_params(query, id.toString());
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check client existence: ") + e.what());
    }
}

bool PostgreSQLClientRepository::emailExists(const std::string& email) {
    try {
        auto work = dbConnection_->beginTransaction();
        
        SqlQueryBuilder queryBuilder;
        std::string query = queryBuilder
            .select({"1"})
            .from("clients")
            .where("email = $1")
            .build();
            
        auto result = work.exec_params(query, email);
        
        dbConnection_->commitTransaction(work);
        return !result.empty();
        
    } catch (const std::exception& e) {
        throw QueryException(std::string("Failed to check email existence: ") + e.what());
    }
}

Client PostgreSQLClientRepository::mapResultToClient(const pqxx::row& row) const {
    UUID id = UUID::fromString(row["id"].c_str());
    std::string name = row["name"].c_str();
    std::string email = row["email"].c_str();
    std::string phone = row["phone"].c_str();
    std::string passwordHash = row["password_hash"].c_str();

    auto registrationDate = DateTimeUtils::parseTimeFromPostgres(row["registration_date"].c_str());
    
    // Создаем клиента с базовыми данными (4 параметра)
    Client client(id, name, email, phone);
    
    client.setPasswordHash(passwordHash);

    client.setRegistrationDate(registrationDate);
    
    // Статус устанавливаем через соответствующие методы
    AccountStatus status = stringToClientStatus(row["status"].c_str());
    switch (status) {
        case AccountStatus::ACTIVE:
            client.activate();
            break;
        case AccountStatus::INACTIVE:
            client.deactivate();
            break;
        case AccountStatus::SUSPENDED:
            client.suspend();
            break;
    }
    
    return client;
}

void PostgreSQLClientRepository::validateClient(const Client& client) const {
    if (!client.isValid()) {
        throw DataAccessException("Invalid client data");
    }
}

std::string PostgreSQLClientRepository::clientStatusToString(AccountStatus status) const {
    switch (status) {
        case AccountStatus::ACTIVE: return "ACTIVE";
        case AccountStatus::INACTIVE: return "INACTIVE";
        case AccountStatus::SUSPENDED: return "SUSPENDED";
        default: return "ACTIVE";
    }
}

AccountStatus PostgreSQLClientRepository::stringToClientStatus(const std::string& status) const {
    if (status == "INACTIVE") return AccountStatus::INACTIVE;
    if (status == "SUSPENDED") return AccountStatus::SUSPENDED;
    return AccountStatus::ACTIVE;
}