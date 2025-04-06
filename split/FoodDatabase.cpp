#include "FoodDatabase.hpp"

// Initialize the static instance
FoodDatabase* FoodDatabase::instance = nullptr;

FoodDatabase::FoodDatabase() : databaseFile("foods.txt") {}

FoodDatabase* FoodDatabase::getInstance() {
    if (!instance) {
        instance = new FoodDatabase();
    }
    return instance;
}

FoodDatabase::~FoodDatabase() {
    foods.clear();
}

void FoodDatabase::setDatabaseFile(const std::string& file) {
    databaseFile = file;
}

bool FoodDatabase::addFood(std::shared_ptr<Food> food) {
    if (foods.find(food->getIdentifier()) != foods.end()) {
        return false; // Food with this ID already exists
    }
    foods[food->getIdentifier()] = food;
    return true;
}

std::shared_ptr<Food> FoodDatabase::getFood(const std::string& id) {
    auto it = foods.find(id);
    if (it != foods.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Food>> FoodDatabase::findFoods(const std::vector<std::string>& keywords, bool matchAll) {
    std::vector<std::shared_ptr<Food>> results;
    for (const auto& pair : foods) {
        if ((matchAll && pair.second->matchesAllKeywords(keywords)) ||
            (!matchAll && pair.second->matchesAnyKeyword(keywords))) {
            results.push_back(pair.second);
        }
    }
    return results;
}

std::vector<std::shared_ptr<Food>> FoodDatabase::getAllFoods() {
    std::vector<std::shared_ptr<Food>> allFoods;
    for (const auto& pair : foods) {
        allFoods.push_back(pair.second);
    }
    return allFoods;
}

bool FoodDatabase::loadDatabase() {
    foods.clear();
    std::ifstream file(databaseFile);
    if (!file.is_open()) {
        std::cout << "Could not open database file. Creating a new one when saving." << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type, id, keywordsStr, componentsStr;
        
        // Parse line using semicolons as separators
        std::getline(iss, type, ';');
        std::getline(iss, id, ';');
        std::getline(iss, keywordsStr, ';');
        
        // Parse keywords
        std::vector<std::string> keywords;
        std::istringstream keywordStream(keywordsStr);
        std::string keyword;
        while (std::getline(keywordStream, keyword, ',')) {
            keywords.push_back(keyword);
        }
        
        if (type == "BASIC") {
            std::string caloriesStr;
            std::getline(iss, caloriesStr);
            double calories = std::stod(caloriesStr);
            
            auto food = std::make_shared<BasicFood>(id, keywords, calories);
            foods[id] = food;
        } 
        else if (type == "COMPOSITE") {
            std::getline(iss, componentsStr);
            
            // Parse components
            std::vector<FoodComponent> components;
            std::istringstream componentStream(componentsStr);
            std::string componentStr;
            
            while (std::getline(componentStream, componentStr, ',')) {
                std::istringstream componentParts(componentStr);
                std::string componentId, servingsStr;
                
                std::getline(componentParts, componentId, ':');
                std::getline(componentParts, servingsStr);
                double servings = std::stod(servingsStr);
                
                // Store component with a placeholder - will resolve after loading all foods
                components.push_back(FoodComponent(nullptr, servings));
                components.back().food = std::make_shared<BasicFood>(componentId, std::vector<std::string>(), 0);
            }
            
            auto food = std::make_shared<CompositeFood>(id, keywords, components);
            foods[id] = food;
        }
    }
    
    // Resolve component references
    for (auto& pair : foods) {
        auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(pair.second);
        if (compositeFood) {
            auto& components = const_cast<std::vector<FoodComponent>&>(compositeFood->getComponents());
            for (auto& component : components) {
                std::string componentId = component.food->getIdentifier();
                component.food = foods[componentId];
            }
        }
    }
    
    file.close();
    return true;
}

bool FoodDatabase::saveDatabase() {
    std::ofstream file(databaseFile);
    if (!file.is_open()) {
        std::cout << "Could not open database file for writing." << std::endl;
        return false;
    }
    
    for (const auto& pair : foods) {
        file << pair.second->serialize() << std::endl;
    }
    
    file.close();
    return true;
}