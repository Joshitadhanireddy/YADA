#include "Food.hpp"

// Food class implementation
Food::Food(const std::string& id, const std::vector<std::string>& keys) 
    : identifier(id), keywords(keys) {}

const std::string& Food::getIdentifier() const { return identifier; }
const std::vector<std::string>& Food::getKeywords() const { return keywords; }

bool Food::matchesAllKeywords(const std::vector<std::string>& searchKeys) const {
    for (const auto& key : searchKeys) {
        bool found = false;
        for (const auto& foodKey : keywords) {
            if (foodKey.find(key) != std::string::npos) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

bool Food::matchesAnyKeyword(const std::vector<std::string>& searchKeys) const {
    if (searchKeys.empty()) return true;
    
    for (const auto& key : searchKeys) {
        for (const auto& foodKey : keywords) {
            if (foodKey.find(key) != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

// BasicFood class implementation
BasicFood::BasicFood(const std::string& id, const std::vector<std::string>& keys, double cals)
    : Food(id, keys), calories(cals) {}

double BasicFood::getCaloriesPerServing() const { return calories; }

std::string BasicFood::toString() const {
    std::stringstream ss;
    ss << identifier << " (";
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) ss << ", ";
    }
    ss << ") - " << calories << " calories per serving";
    return ss.str();
}

std::string BasicFood::serialize() const {
    std::stringstream ss;
    ss << "BASIC;" << identifier << ";";
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) ss << ",";
    }
    ss << ";" << calories;
    return ss.str();
}

// FoodComponent implementation
FoodComponent::FoodComponent(std::shared_ptr<Food> f, double s) : food(f), servings(s) {}

// CompositeFood implementation
CompositeFood::CompositeFood(const std::string& id, const std::vector<std::string>& keys,
                const std::vector<FoodComponent>& comps)
    : Food(id, keys), components(comps) {}

double CompositeFood::getCaloriesPerServing() const {
    double totalCalories = 0.0;
    for (const auto& comp : components) {
        totalCalories += comp.food->getCaloriesPerServing() * comp.servings;
    }
    return totalCalories;
}

const std::vector<FoodComponent>& CompositeFood::getComponents() const { return components; }

std::string CompositeFood::toString() const {
    std::stringstream ss;
    ss << identifier << " (";
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) ss << ", ";
    }
    ss << ") - " << getCaloriesPerServing() << " calories per serving\n";
    ss << "Components:\n";
    for (const auto& comp : components) {
        ss << "  - " << comp.servings << " serving(s) of " << comp.food->getIdentifier() << "\n";
    }
    return ss.str();
}

std::string CompositeFood::serialize() const {
    std::stringstream ss;
    ss << "COMPOSITE;" << identifier << ";";
    for (size_t i = 0; i < keywords.size(); ++i) {
        ss << keywords[i];
        if (i < keywords.size() - 1) ss << ",";
    }
    ss << ";";
    for (size_t i = 0; i < components.size(); ++i) {
        ss << components[i].food->getIdentifier() << ":" << components[i].servings;
        if (i < components.size() - 1) ss << ",";
    }
    return ss.str();
}