#pragma once
#include "../models/Client.hpp"
#include <memory>
#include <optional>

class IClientRepository {
public:
    virtual ~IClientRepository() = default;
    
    virtual std::optional<Client> findById(int id) = 0;
    virtual std::optional<Client> findByEmail(const std::string& email) = 0;
    virtual bool save(const Client& client) = 0;
    virtual bool update(const Client& client) = 0;
    virtual bool remove(int id) = 0;
    virtual bool exists(int id) = 0;
};