#ifndef STUDIO_HPP
#define STUDIO_HPP

#include "../types/uuid.hpp"
#include <string>
#include <vector>

class Studio {
private:
    UUID id_;
    std::string name_;
    std::string description_;
    std::string contactEmail_;
    std::vector<UUID> branchIds_;

public:
    Studio();
    Studio(const UUID& id, const std::string& name, const std::string& contactEmail);
    
    // Геттеры
    UUID getId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getContactEmail() const;
    std::vector<UUID> getBranchIds() const;
    
    // Сеттеры
    void setDescription(const std::string& description);
    void setContactEmail(const std::string& email);
    
    // Управление филиалами
    void addBranch(const UUID& branchId);
    void removeBranch(const UUID& branchId);
    bool hasBranch(const UUID& branchId) const;
    
    // Валидация
    bool isValid() const;
    static bool isValidName(const std::string& name);
    static bool isValidEmail(const std::string& email);
    static bool isValidDescription(const std::string& description);
};

#endif // STUDIO_HPP