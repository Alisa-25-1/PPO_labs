#ifndef POSTGRESQL_BRANCH_REPOSITORY_HPP
#define POSTGRESQL_BRANCH_REPOSITORY_HPP

#include "../IBranchRepository.hpp"
#include "../../data/DatabaseConnection.hpp"
#include "../../data/exceptions/DataAccessException.hpp"
#include "../../data/SqlQueryBuilder.hpp"
#include <memory>
#include <pqxx/pqxx>

class PostgreSQLBranchRepository : public IBranchRepository {
public:
    explicit PostgreSQLBranchRepository(std::shared_ptr<DatabaseConnection> dbConnection);
    
    std::optional<Branch> findById(const UUID& id) override;
    std::vector<Branch> findByStudioId(const UUID& studioId) override;
    std::vector<Branch> findAll() override;
    bool save(const Branch& branch) override;
    bool update(const Branch& branch) override;
    bool remove(const UUID& id) override;
    bool exists(const UUID& id) override;

private:
    std::shared_ptr<DatabaseConnection> dbConnection_;
    
    // Вспомогательные методы
    std::optional<BranchAddress> findAddressById(const UUID& addressId, pqxx::work& work);
    Branch mapResultToBranch(const pqxx::row& row, const BranchAddress& address) const;
    BranchAddress mapResultToAddress(const pqxx::row& row) const;
    bool saveAddress(const BranchAddress& address, pqxx::work& work);
    bool updateAddress(const BranchAddress& address, pqxx::work& work);
    void validateBranch(const Branch& branch) const;
};

#endif // POSTGRESQL_BRANCH_REPOSITORY_HPP