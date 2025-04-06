#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <vector>
#include <memory>
#include <sstream>

// Food class - base class for BasicFood and CompositeFood
class Food {
protected:
    std::string identifier;
    std::vector<std::string> keywords;
    
public:
    Food(const std::string& id, const std::vector<std::string>& keys);
    virtual ~Food() = default;
    
    const std::string& getIdentifier() const;
    const std::vector<std::string>& getKeywords() const;
    
    virtual double getCaloriesPerServing() const = 0;
    virtual std::string toString() const = 0;
    virtual std::string serialize() const = 0;
    
    bool matchesAllKeywords(const std::vector<std::string>& searchKeys) const;
    bool matchesAnyKeyword(const std::vector<std::string>& searchKeys) const;
};

// BasicFood class
class BasicFood : public Food {
private:
    double calories;
    
public:
    BasicFood(const std::string& id, const std::vector<std::string>& keys, double cals);
    
    double getCaloriesPerServing() const override;
    std::string toString() const override;
    std::string serialize() const override;
};

// FoodComponent class for Composite pattern
class FoodComponent {
public:
    std::shared_ptr<Food> food;
    double servings;
    
    FoodComponent(std::shared_ptr<Food> f, double s);
};

// CompositeFood class
class CompositeFood : public Food {
private:
    std::vector<FoodComponent> components;
    
public:
    CompositeFood(const std::string& id, const std::vector<std::string>& keys,
                 const std::vector<FoodComponent>& comps);
    
    double getCaloriesPerServing() const override;
    const std::vector<FoodComponent>& getComponents() const;
    std::string toString() const override;
    std::string serialize() const override;
};

#endif // FOOD_H