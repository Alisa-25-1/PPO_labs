#ifndef TRAINER_HPP
#define TRAINER_HPP

#include "../types/uuid.hpp"
#include "../types/enums.hpp"
#include <string>
#include <vector>

class Trainer {
private:
    UUID id_;
    std::string name_;
    std::vector<std::string> specializations_;
    std::string biography_;
    std::string qualificationLevel_;
    bool isActive_;

public:
    Trainer();
    Trainer(const UUID& id, const std::string& name, const std::vector<std::string>& specializations);
    
    // Геттеры
    UUID getId() const;
    std::string getName() const;
    std::vector<std::string> getSpecializations() const;
    std::string getBiography() const;
    std::string getQualificationLevel() const;
    bool isActive() const;
    
    // Сеттеры
    void setBiography(const std::string& biography);
    void setQualificationLevel(const std::string& level);
    void setActive(bool active);
    
    // Управление специализациями
    void addSpecialization(const std::string& specialization);
    void removeSpecialization(const std::string& specialization);
    bool hasSpecialization(const std::string& specialization) const;
    
    // Валидация
    bool isValid() const;
    static bool isValidName(const std::string& name);
    static bool isValidSpecialization(const std::string& specialization);
    static bool isValidBiography(const std::string& biography);
    static bool isValidQualificationLevel(const std::string& level);
};

#endif // TRAINER_HPP