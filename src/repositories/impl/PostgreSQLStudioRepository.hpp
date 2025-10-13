#ifndef POSTGRESQL_STUDIO_REPOSITORY_HPP
#define POSTGRESQL_STUDIO_REPOSITORY_HPP

#include "../IStudioRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include <memory>

class PostgreSQLStudioRepository : public IStudioRepository {
public:
    explicit PostgreSQLStudioRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Studio> findById(const UUID& id) override;
    std::optional<Studio> findMainStudio() override;
    std::vector<Studio> findAll() override;
    bool save(const Studio& studio) override;
    bool update(const Studio& studio) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    Studio mapResultToStudio(const pqxx::row& row) const;
    void validateStudio(const Studio& studio) const;
};

#endif // POSTGRESQL_STUDIO_REPOSITORY_HPP