#ifndef HALL_HPP
#define HALL_HPP

#include <string>

class Hall {
private:
    int id_;
    std::string name_;
    std::string description_;
    int capacity_;
    std::string floorType_;
    std::string equipment_;
    int branchId_;

public:
    Hall();
    Hall(int id, std::string name, int capacity, int branchId);
    
    int getId() const;
    std::string getName() const;
    std::string getDescription() const;
    int getCapacity() const;
    std::string getFloorType() const;
    std::string getEquipment() const;
    int getBranchId() const;
    
    void setDescription(const std::string& description);
    void setFloorType(const std::string& floorType);
    void setEquipment(const std::string& equipment);
    
    bool isValid() const;

    // Операторы сравнения
    bool operator==(const Hall& other) const;
    bool operator!=(const Hall& other) const;
};

#endif // HALL_HPP