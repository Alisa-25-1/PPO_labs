#pragma once
#include "../types/uuid.hpp"
#include "../models/Client.hpp"
#include <memory>
#include <optional>

class IClientRepository {
public:
    virtual ~IClientRepository() = default;
    
    virtual std::optional<Client> findById(const UUID& id) = 0;
    virtual std::optional<Client> findByEmail(const std::string& email) = 0;
    virtual std::vector<Client> findAll() = 0;
    virtual bool save(const Client& client) = 0;
    virtual bool update(const Client& client) = 0;
    virtual bool remove(const UUID& id) = 0;
    virtual bool exists(const UUID& id) = 0;
};