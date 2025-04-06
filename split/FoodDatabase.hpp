#ifndef FOOD_DATABASE_H
#define FOOD_DATABASE_H

#include "Food.hpp"
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

// FoodDatabase class - Singleton pattern
class FoodDatabase {
private:
    static FoodDatabase* instance;
    std::map<std::string, std::shared_ptr<Food>> foods;
    std::string databaseFile;
    
    // Private constructor for singleton
    FoodDatabase();
    
public:
    static FoodDatabase* getInstance();
    ~FoodDatabase();
    
    void setDatabaseFile(const std::string& file);
    bool addFood(std::shared_ptr<Food> food);
    std::shared_ptr<Food> getFood(const std::string& id);
    std::vector<std::shared_ptr<Food>> findFoods(const std::vector<std::string>& keywords, bool matchAll);
    std::vector<std::shared_ptr<Food>> getAllFoods();
    bool loadDatabase();
    bool saveDatabase();
};

#endif // FOOD_DATABASE_H