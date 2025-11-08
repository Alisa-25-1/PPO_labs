#ifndef REPOSITORY_FACTORY_CREATOR_HPP
#define REPOSITORY_FACTORY_CREATOR_HPP

#include <memory>
#include <iostream>
#include "IRepositoryFactory.hpp"
#include "PostgreSQLRepositoryFactory.hpp"
#include "MongoDBRepositoryFactory.hpp"
#include "../core/Config.hpp"

class RepositoryFactoryCreator {
public:
    static std::shared_ptr<IRepositoryFactory> createFactory(const Config& config) {
        std::string dbType = config.getDatabaseType();
        
        if (dbType == "postgres") {
            std::string connectionString = config.getPostgresConnectionString();
            std::cout << "🔧 Creating PostgreSQL repository factory" << std::endl;
            // ЯВНОЕ ПРЕОБРАЗОВАНИЕ ТИПА
            return std::static_pointer_cast<IRepositoryFactory>(
                std::make_shared<PostgreSQLRepositoryFactory>(connectionString)
            );
        }
        else if (dbType == "mongodb") {
            // Временно возвращаем PostgreSQL, но с правильным типом
            std::string connectionString = config.getMongoConnectionString();
            std::string databaseName = config.getMongoDatabaseName();
            std::cout << "🔧 Creating MongoDB repository factory" << std::endl;
            return std::make_shared<MongoDBRepositoryFactory>(connectionString, databaseName);
        }
        else {
            throw std::runtime_error("Unsupported database type: " + dbType);
        }
    }
};

#endif // REPOSITORY_FACTORY_CREATOR_HPP