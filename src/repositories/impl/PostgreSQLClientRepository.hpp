#ifndef POSTGRESQL_CLIENT_REPOSITORY_HPP
#define POSTGRESQL_CLIENT_REPOSITORY_HPP

#include "../IClientRepository.hpp"
#include "../../data/DatabaseConnection.hpp"           
#include "../../data/exceptions/DataAccessException.hpp" 
#include "../../data/SqlQueryBuilder.hpp"
#include <memory>

class PostgreSQLClientRepository : public IClientRepository {
public:
    explicit PostgreSQLClientRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Client> findById(const UUID& id) override;
    std::optional<Client> findByEmail(const std::string& email) override;
    bool save(const Client& client) override;
    bool update(const Client& client) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Client mapResultToClient(const pqxx::row& row) const;
    void validateClient(const Client& client) const;
    
    // Добавляем объявления вспомогательных методов
    std::string clientStatusToString(AccountStatus status) const;
    AccountStatus stringToClientStatus(const std::string& status) const;
};

#endif // POSTGRESQLCLIENTREPOSITORY_HPP