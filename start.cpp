// main.cpp
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>
#include <algorithm>
#include <ctime>
#include <stack>

// Forward declarations
class Food;
class BasicFood;
class CompositeFood;
class FoodDatabase;
class DailyLog;
class DietProfile;
class TargetCalorieCalculator;
class HarrisBenedictCalculator;
class MifflinStJeorCalculator;
class Command;
class UndoManager;

// Observer pattern for updates
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update() = 0;
};

class Subject {
private:
    std::vector<Observer*> observers;
public:
    virtual ~Subject() = default;
    
    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }
    
    void removeObserver(Observer* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }
    
    void notifyObservers() {
        for (Observer* observer : observers) {
            observer->update();
        }
    }
};

// Food class - base class for BasicFood and CompositeFood
class Food {
protected:
    std::string identifier;
    std::vector<std::string> keywords;
    
public:
    Food(const std::string& id, const std::vector<std::string>& keys) 
        : identifier(id), keywords(keys) {}
    
    virtual ~Food() = default;
    
    const std::string& getIdentifier() const { return identifier; }
    const std::vector<std::string>& getKeywords() const { return keywords; }
    
    virtual double getCaloriesPerServing() const = 0;
    virtual std::string toString() const = 0;
    virtual std::string serialize() const = 0;
    
    bool matchesAllKeywords(const std::vector<std::string>& searchKeys) const {
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
    
    bool matchesAnyKeyword(const std::vector<std::string>& searchKeys) const {
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
};

// BasicFood class
class BasicFood : public Food {
private:
    double calories;
    
public:
    BasicFood(const std::string& id, const std::vector<std::string>& keys, double cals)
        : Food(id, keys), calories(cals) {}
    
    double getCaloriesPerServing() const override { return calories; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << identifier << " (";
        for (size_t i = 0; i < keywords.size(); ++i) {
            ss << keywords[i];
            if (i < keywords.size() - 1) ss << ", ";
        }
        ss << ") - " << calories << " calories per serving";
        return ss.str();
    }
    
    std::string serialize() const override {
        std::stringstream ss;
        ss << "BASIC;" << identifier << ";";
        for (size_t i = 0; i < keywords.size(); ++i) {
            ss << keywords[i];
            if (i < keywords.size() - 1) ss << ",";
        }
        ss << ";" << calories;
        return ss.str();
    }
};

// FoodComponent class for Composite pattern
class FoodComponent {
public:
    std::shared_ptr<Food> food;
    double servings;
    
    FoodComponent(std::shared_ptr<Food> f, double s) : food(f), servings(s) {}
};

// CompositeFood class
class CompositeFood : public Food {
private:
    std::vector<FoodComponent> components;
    
public:
    CompositeFood(const std::string& id, const std::vector<std::string>& keys,
                 const std::vector<FoodComponent>& comps)
        : Food(id, keys), components(comps) {}
    
    double getCaloriesPerServing() const override {
        double totalCalories = 0.0;
        for (const auto& comp : components) {
            totalCalories += comp.food->getCaloriesPerServing() * comp.servings;
        }
        return totalCalories;
    }
    
    const std::vector<FoodComponent>& getComponents() const { return components; }
    
    std::string toString() const override {
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
    
    std::string serialize() const override {
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
};

// FoodDatabase class - Singleton pattern
class FoodDatabase {
private:
    static FoodDatabase* instance;
    std::map<std::string, std::shared_ptr<Food>> foods;
    std::string databaseFile;
    
    // Private constructor for singleton
    FoodDatabase() : databaseFile("foods.txt") {}
    
public:
    static FoodDatabase* getInstance() {
        if (!instance) {
            instance = new FoodDatabase();
        }
        return instance;
    }
    
    ~FoodDatabase() {
        foods.clear();
    }
    
    void setDatabaseFile(const std::string& file) {
        databaseFile = file;
    }
    
    bool addFood(std::shared_ptr<Food> food) {
        if (foods.find(food->getIdentifier()) != foods.end()) {
            return false; // Food with this ID already exists
        }
        foods[food->getIdentifier()] = food;
        return true;
    }
    
    std::shared_ptr<Food> getFood(const std::string& id) {
        auto it = foods.find(id);
        if (it != foods.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::vector<std::shared_ptr<Food>> findFoods(const std::vector<std::string>& keywords, bool matchAll) {
        std::vector<std::shared_ptr<Food>> results;
        for (const auto& pair : foods) {
            if ((matchAll && pair.second->matchesAllKeywords(keywords)) ||
                (!matchAll && pair.second->matchesAnyKeyword(keywords))) {
                results.push_back(pair.second);
            }
        }
        return results;
    }
    
    std::vector<std::shared_ptr<Food>> getAllFoods() {
        std::vector<std::shared_ptr<Food>> allFoods;
        for (const auto& pair : foods) {
            allFoods.push_back(pair.second);
        }
        return allFoods;
    }
    
    bool loadDatabase() {
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
    
    bool saveDatabase() {
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
};

// Initialize the static instance
FoodDatabase* FoodDatabase::instance = nullptr;

// DailyLog class for tracking food consumption
class LogEntry {
public:
    std::shared_ptr<Food> food;
    double servings;
    
    LogEntry(std::shared_ptr<Food> f, double s) : food(f), servings(s) {}
    
    double getCalories() const {
        return food->getCaloriesPerServing() * servings;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << servings << " serving(s) of " << food->getIdentifier() 
           << " (" << getCalories() << " calories)";
        return ss.str();
    }
    
    std::string serialize() const {
        std::stringstream ss;
        ss << food->getIdentifier() << ":" << servings;
        return ss.str();
    }
};

// Represents a single day in the log
class DayLog {
private:
    std::vector<LogEntry> entries;
    
public:
    void addEntry(const LogEntry& entry) {
        entries.push_back(entry);
    }
    
    void removeEntry(int index) {
        if (index >= 0 && index < static_cast<int>(entries.size())) {
            entries.erase(entries.begin() + index);
        }
    }
    
    const std::vector<LogEntry>& getEntries() const {
        return entries;
    }
    
    double getTotalCalories() const {
        double total = 0.0;
        for (const auto& entry : entries) {
            total += entry.getCalories();
        }
        return total;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Daily Food Log:\n";
        for (size_t i = 0; i < entries.size(); ++i) {
            ss << i + 1 << ". " << entries[i].toString() << "\n";
        }
        ss << "Total Calories: " << getTotalCalories() << "\n";
        return ss.str();
    }
};

// Represents the entire log for all dates
class DailyLog : public Subject {
private:
    std::map<std::string, DayLog> logs;
    std::string currentDate;
    std::string logFile;
    
    std::string formatDate(int year, int month, int day) {
        std::stringstream ss;
        ss << year << "-" 
           << (month < 10 ? "0" : "") << month << "-"
           << (day < 10 ? "0" : "") << day;
        return ss.str();
    }
    
public:
    DailyLog() : logFile("dailylog.txt") {
        // Set current date as default
        time_t now = time(0);
        tm* ltm = localtime(&now);
        currentDate = formatDate(1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    }
    
    void setLogFile(const std::string& file) {
        logFile = file;
    }
    
    const std::string& getCurrentDate() const {
        return currentDate;
    }
    
    void setCurrentDate(const std::string& date) {
        currentDate = date;
        notifyObservers();
    }
    
    DayLog& getCurrentDayLog() {
        return logs[currentDate];
    }
    
    const DayLog& getCurrentDayLog() const {
        return logs.at(currentDate);
    }
    
    bool dateExists(const std::string& date) const {
        return logs.find(date) != logs.end();
    }
    
    std::vector<std::string> getAllDates() const {
        std::vector<std::string> dates;
        for (const auto& pair : logs) {
            dates.push_back(pair.first);
        }
        std::sort(dates.begin(), dates.end());
        return dates;
    }
    
    void addFoodToCurrentDay(std::shared_ptr<Food> food, double servings) {
        logs[currentDate].addEntry(LogEntry(food, servings));
        notifyObservers();
    }
    
    void removeFoodFromCurrentDay(int index) {
        logs[currentDate].removeEntry(index);
        notifyObservers();
    }
    
    bool loadLog() {
        logs.clear();
        std::ifstream file(logFile);
        if (!file.is_open()) {
            std::cout << "Could not open log file. Creating a new one when saving." << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string date, entriesStr;
            
            std::getline(iss, date, ';');
            std::getline(iss, entriesStr);
            
            DayLog dayLog;
            std::istringstream entriesStream(entriesStr);
            std::string entryStr;
            
            while (std::getline(entriesStream, entryStr, ',')) {
                std::istringstream entryParts(entryStr);
                std::string foodId, servingsStr;
                
                std::getline(entryParts, foodId, ':');
                std::getline(entryParts, servingsStr);
                double servings = std::stod(servingsStr);
                
                auto food = FoodDatabase::getInstance()->getFood(foodId);
                if (food) {
                    dayLog.addEntry(LogEntry(food, servings));
                }
            }
            
            logs[date] = dayLog;
        }
        
        file.close();
        return true;
    }
    
    bool saveLog() {
        std::ofstream file(logFile);
        if (!file.is_open()) {
            std::cout << "Could not open log file for writing." << std::endl;
            return false;
        }
        
        for (const auto& pair : logs) {
            file << pair.first << ";";
            const auto& entries = pair.second.getEntries();
            for (size_t i = 0; i < entries.size(); ++i) {
                file << entries[i].serialize();
                if (i < entries.size() - 1) file << ",";
            }
            file << std::endl;
        }
        
        file.close();
        return true;
    }
};

// Activity level enum
enum class ActivityLevel {
    Sedentary,
    LightlyActive,
    ModeratelyActive,
    VeryActive,
    ExtremelyActive
};

// Gender enum
enum class Gender {
    Male,
    Female
};

// Target Calorie Calculator - Strategy pattern
class TargetCalorieCalculator {
public:
    virtual ~TargetCalorieCalculator() = default;
    virtual double calculateTargetCalories(Gender gender, double weightKg, double heightCm, int age, ActivityLevel activityLevel) const = 0;
    virtual std::string getName() const = 0;
};

// Harris-Benedict formula
class HarrisBenedictCalculator : public TargetCalorieCalculator {
public:
    double calculateTargetCalories(Gender gender, double weightKg, double heightCm, int age, ActivityLevel activityLevel) const override {
        double bmr;
        if (gender == Gender::Male) {
            bmr = 88.362 + (13.397 * weightKg) + (4.799 * heightCm) - (5.677 * age);
        } else {
            bmr = 447.593 + (9.247 * weightKg) + (3.098 * heightCm) - (4.330 * age);
        }
        
        double activityFactor;
        switch (activityLevel) {
            case ActivityLevel::Sedentary: activityFactor = 1.2; break;
            case ActivityLevel::LightlyActive: activityFactor = 1.375; break;
            case ActivityLevel::ModeratelyActive: activityFactor = 1.55; break;
            case ActivityLevel::VeryActive: activityFactor = 1.725; break;
            case ActivityLevel::ExtremelyActive: activityFactor = 1.9; break;
            default: activityFactor = 1.2;
        }
        
        return bmr * activityFactor;
    }
    
    std::string getName() const override {
        return "Harris-Benedict Equation";
    }
};

// Mifflin-St Jeor formula
class MifflinStJeorCalculator : public TargetCalorieCalculator {
public:
    double calculateTargetCalories(Gender gender, double weightKg, double heightCm, int age, ActivityLevel activityLevel) const override {
        double bmr;
        if (gender == Gender::Male) {
            bmr = (10 * weightKg) + (6.25 * heightCm) - (5 * age) + 5;
        } else {
            bmr = (10 * weightKg) + (6.25 * heightCm) - (5 * age) - 161;
        }
        
        double activityFactor;
        switch (activityLevel) {
            case ActivityLevel::Sedentary: activityFactor = 1.2; break;
            case ActivityLevel::LightlyActive: activityFactor = 1.375; break;
            case ActivityLevel::ModeratelyActive: activityFactor = 1.55; break;
            case ActivityLevel::VeryActive: activityFactor = 1.725; break;
            case ActivityLevel::ExtremelyActive: activityFactor = 1.9; break;
            default: activityFactor = 1.2;
        }
        
        return bmr * activityFactor;
    }
    
    std::string getName() const override {
        return "Mifflin-St Jeor Equation";
    }
};

// User's Diet Profile
class DietProfile : public Subject {
private:
    Gender gender;
    double heightCm;
    int age;
    std::map<std::string, double> weightsByDate;
    std::map<std::string, ActivityLevel> activityLevelsByDate;
    std::shared_ptr<TargetCalorieCalculator> calculator;
    std::string profileFile;
    
public:
    DietProfile() 
        : gender(Gender::Male), heightCm(170), age(30), 
          calculator(std::make_shared<HarrisBenedictCalculator>()),
          profileFile("profile.txt") {
        // Initialize with default values
        std::string today = DailyLog().getCurrentDate();
        weightsByDate[today] = 70.0;
        activityLevelsByDate[today] = ActivityLevel::ModeratelyActive;
    }
    
    void setProfileFile(const std::string& file) {
        profileFile = file;
    }
    
    void setGender(Gender g) {
        gender = g;
        notifyObservers();
    }
    
    Gender getGender() const {
        return gender;
    }
    
    void setHeight(double h) {
        heightCm = h;
        notifyObservers();
    }
    
    double getHeight() const {
        return heightCm;
    }
    
    void setAge(int a) {
        age = a;
        notifyObservers();
    }
    
    int getAge() const {
        return age;
    }
    
    void setWeight(const std::string& date, double weight) {
        weightsByDate[date] = weight;
        notifyObservers();
    }
    
    double getWeight(const std::string& date) const {
        // Try to get weight for specific date
        auto it = weightsByDate.find(date);
        if (it != weightsByDate.end()) {
            return it->second;
        }
        
        // If not found, find the most recent date before the given date
        double weight = 70.0; // Default weight
        std::string mostRecentDate = "";
        
        for (const auto& pair : weightsByDate) {
            if (pair.first <= date && (mostRecentDate.empty() || pair.first > mostRecentDate)) {
                mostRecentDate = pair.first;
                weight = pair.second;
            }
        }
        
        return weight;
    }
    
    void setActivityLevel(const std::string& date, ActivityLevel level) {
        activityLevelsByDate[date] = level;
        notifyObservers();
    }
    
    ActivityLevel getActivityLevel(const std::string& date) const {
        // Try to get activity level for specific date
        auto it = activityLevelsByDate.find(date);
        if (it != activityLevelsByDate.end()) {
            return it->second;
        }
        
        // If not found, find the most recent date before the given date
        ActivityLevel level = ActivityLevel::ModeratelyActive; // Default
        std::string mostRecentDate = "";
        
        for (const auto& pair : activityLevelsByDate) {
            if (pair.first <= date && (mostRecentDate.empty() || pair.first > mostRecentDate)) {
                mostRecentDate = pair.first;
                level = pair.second;
            }
        }
        
        return level;
    }
    
    void setCalculator(std::shared_ptr<TargetCalorieCalculator> calc) {
        calculator = calc;
        notifyObservers();
    }
    
    std::shared_ptr<TargetCalorieCalculator> getCalculator() const {
        return calculator;
    }
    
    double getTargetCalories(const std::string& date) const {
        return calculator->calculateTargetCalories(
            gender, getWeight(date), heightCm, age, getActivityLevel(date));
    }
    
    bool loadProfile() {
        std::ifstream file(profileFile);
        if (!file.is_open()) {
            std::cout << "Could not open profile file. Creating a new one when saving." << std::endl;
            return false;
        }
        
        std::string line;
        
        // Read basic info
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string genderStr, heightStr, ageStr, calculatorStr;
            
            std::getline(iss, genderStr, ';');
            std::getline(iss, heightStr, ';');
            std::getline(iss, ageStr, ';');
            std::getline(iss, calculatorStr);
            
            gender = (genderStr == "Male") ? Gender::Male : Gender::Female;
            heightCm = std::stod(heightStr);
            age = std::stoi(ageStr);
            
            if (calculatorStr == "Harris-Benedict") {
                calculator = std::make_shared<HarrisBenedictCalculator>();
            } else if (calculatorStr == "Mifflin-St Jeor") {
                calculator = std::make_shared<MifflinStJeorCalculator>();
            }
        }
        
        // Read weights by date
        weightsByDate.clear();
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string pair;
            
            while (std::getline(iss, pair, ',')) {
                std::istringstream pairStream(pair);
                std::string date, weightStr;
                
                std::getline(pairStream, date, ':');
                std::getline(pairStream, weightStr);
                
                weightsByDate[date] = std::stod(weightStr);
            }
        }
        
        // Read activity levels by date
        activityLevelsByDate.clear();
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string pair;
            
            while (std::getline(iss, pair, ',')) {
                std::istringstream pairStream(pair);
                std::string date, levelStr;
                
                std::getline(pairStream, date, ':');
                std::getline(pairStream, levelStr);
                
                int level = std::stoi(levelStr);
                activityLevelsByDate[date] = static_cast<ActivityLevel>(level);
            }
        }
        
        file.close();
        return true;
    }
    
    bool saveProfile() {
        std::ofstream file(profileFile);
        if (!file.is_open()) {
            std::cout << "Could not open profile file for writing." << std::endl;
            return false;
        }
        
        // Write basic info
        file << (gender == Gender::Male ? "Male" : "Female") << ";"
             << heightCm << ";"
             << age << ";"
             << calculator->getName() << std::endl;
        
        // Write weights by date
        for (auto it = weightsByDate.begin(); it != weightsByDate.end(); ++it) {
            file << it->first << ":" << it->second;
            if (std::next(it) != weightsByDate.end()) file << ",";
        }
        file << std::endl;
        
        // Write activity levels by date
        for (auto it = activityLevelsByDate.begin(); it != activityLevelsByDate.end(); ++it) {
            file << it->first << ":" << static_cast<int>(it->second);
            if (std::next(it) != activityLevelsByDate.end()) file << ",";
        }
        file << std::endl;
        
        file.close();
        return true;
    }
};

// Command pattern for undo functionality
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string toString() const = 0;
};

// Add food command
class AddFoodCommand : public Command {
private:
    DailyLog& log;
    std::shared_ptr<Food> food;
    double servings;
    
public:
    AddFoodCommand(DailyLog& l, std::shared_ptr<Food> f, double s)
        : log(l), food(f), servings(s) {}
    
    void execute() override {
        log.addFoodToCurrentDay(food, servings);
    }
    
    void undo() override {
        auto& dayLog = log.getCurrentDayLog();
        const auto& entries = dayLog.getEntries();
        
        // Find the entry with this food and servings and remove it
        for (int i = entries.size() - 1; i >= 0; --i) {
            if (entries[i].food == food && entries[i].servings == servings) {
                log.removeFoodFromCurrentDay(i);
                break;
            }
        }
    }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "Add " << servings << " serving(s) of " << food->getIdentifier();
        return ss.str();
    }
};

// Remove food command
class RemoveFoodCommand : public Command {
private:
    DailyLog& log;
    int index;
    LogEntry savedEntry;
    
public:
    RemoveFoodCommand(DailyLog& l, int i)
        : log(l), index(i), savedEntry(log.getCurrentDayLog().getEntries()[i]) {}
    
    void execute() override {
        log.removeFoodFromCurrentDay(index);
    }
    
    void undo() override {
        log.addFoodToCurrentDay(savedEntry.food, savedEntry.servings);
    }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "Remove " << savedEntry.servings << " serving(s) of " << savedEntry.food->getIdentifier();
        return ss.str();
    }
};

// Change date command
class ChangeDateCommand : public Command {
private:
    DailyLog& log;
    std::string oldDate;
    std::string newDate;
    
public:
    ChangeDateCommand(DailyLog& l, const std::string& newD)
        : log(l), oldDate(l.getCurrentDate()), newDate(newD) {}
    
    void execute() override {
        log.setCurrentDate(newDate);
    }
    
    void undo() override {
        log.setCurrentDate(oldDate);
    }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "Change date from " << oldDate << " to " << newDate;
        return ss.str();
    }
};

// UndoManager class
class UndoManager {
private:
    std::stack<std::shared_ptr<Command>> undoStack;
    
public:
    void executeCommand(std::shared_ptr<Command> command) {
        command->execute();
        undoStack.push(command);
    }
    
    bool canUndo() const {
        return !undoStack.empty();
    }
    
    void undo() {
        if (canUndo()) {
            undoStack.top()->undo();
            undoStack.pop();
        }
    }
    
    std::vector<std::string> getCommandHistory() const {
        std::vector<std::string> history;
        std::stack<std::shared_ptr<Command>> tempStack = undoStack;
        
        while (!tempStack.empty()) {
            history.push_back(tempStack.top()->toString());
            tempStack.pop();
        }
        
        std::reverse(history.begin(), history.end());
        return history;
    }
    
    void clearHistory() {
        while (!undoStack.empty()) {
            undoStack.pop();
        }
    }
};

// FoodTracker class that implements Observer
class FoodTracker : public Observer {
private:
    DailyLog& log;
    DietProfile& profile;
    
public:
    FoodTracker(DailyLog& l, DietProfile& p) : log(l), profile(p) {
        log.addObserver(this);
        profile.addObserver(this);
    }
    
    ~FoodTracker() {
        log.removeObserver(this);
        profile.removeObserver(this);
    }
    
    void update() override {
        // This method is called when either log or profile changes
        displayDailySummary();
    }
    
    void displayDailySummary() const {
        std::string date = log.getCurrentDate();
        std::cout << "\n===== Daily Summary for " << date << " =====\n";
        
        double targetCalories = profile.getTargetCalories(date);
        double consumedCalories = log.getCurrentDayLog().getTotalCalories();
        double difference = consumedCalories - targetCalories;
        
        std::cout << "Target Calories: " << std::fixed << std::setprecision(1) << targetCalories << std::endl;
        std::cout << "Consumed Calories: " << std::fixed << std::setprecision(1) << consumedCalories << std::endl;
        std::cout << "Difference: " << std::fixed << std::setprecision(1) << difference;
        if (difference < 0) {
            std::cout << " (under target)";
        } else if (difference > 0) {
            std::cout << " (over target)";
        } else {
            std::cout << " (exactly on target)";
        }
        std::cout << std::endl;
    }
};

// Main application class
class DietManagerApp {
private:
    FoodDatabase* foodDb;
    DailyLog log;
    DietProfile profile;
    UndoManager undoManager;
    FoodTracker tracker;
    bool running;
    
    void displayMainMenu() {
        std::cout << "\n===== YADA (Yet Another Diet Assistant) =====\n";
        std::cout << "Current Date: " << log.getCurrentDate() << "\n";
        std::cout << "1. Manage Foods\n";
        std::cout << "2. Log Foods\n";
        std::cout << "3. Manage Profile\n";
        std::cout << "4. Select Date\n";
        std::cout << "5. Undo Last Action\n";
        std::cout << "6. Save Data\n";
        std::cout << "7. Exit\n";
        std::cout << "Enter choice: ";
    }
    
    void manageFoods() {
        while (true) {
            std::cout << "\n===== Manage Foods =====\n";
            std::cout << "1. View All Foods\n";
            std::cout << "2. Search Foods\n";
            std::cout << "3. Add Basic Food\n";
            std::cout << "4. Create Composite Food\n";
            std::cout << "5. Back to Main Menu\n";
            std::cout << "Enter choice: ";
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: viewAllFoods(); break;
                case 2: searchFoods(); break;
                case 3: addBasicFood(); break;
                case 4: createCompositeFood(); break;
                case 5: return;
                default: std::cout << "Invalid choice. Try again.\n";
            }
        }
    }
    
    void viewAllFoods() {
        std::cout << "\n===== All Foods =====\n";
        auto foods = foodDb->getAllFoods();
        if (foods.empty()) {
            std::cout << "No foods in database.\n";
            return;
        }
        
        for (size_t i = 0; i < foods.size(); ++i) {
            std::cout << i + 1 << ". " << foods[i]->toString() << "\n";
        }
    }
    
    void searchFoods() {
        std::cout << "\n===== Search Foods =====\n";
        std::cout << "Enter keywords (comma separated): ";
        std::string keywordsStr;
        std::getline(std::cin, keywordsStr);
        
        std::vector<std::string> keywords;
        std::istringstream iss(keywordsStr);
        std::string keyword;
        while (std::getline(iss, keyword, ',')) {
            // Trim whitespace
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);
            if (!keyword.empty()) {
                keywords.push_back(keyword);
            }
        }
        
        std::cout << "Match (1) All keywords or (2) Any keyword? ";
        int matchChoice;
        std::cin >> matchChoice;
        std::cin.ignore();
        
        bool matchAll = (matchChoice == 1);
        auto results = foodDb->findFoods(keywords, matchAll);
        
        std::cout << "\n===== Search Results =====\n";
        if (results.empty()) {
            std::cout << "No matching foods found.\n";
            return;
        }
        
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << i + 1 << ". " << results[i]->toString() << "\n";
        }
    }
    
    void addBasicFood() {
        std::cout << "\n===== Add Basic Food =====\n";
        
        std::string identifier;
        std::cout << "Enter food identifier: ";
        std::getline(std::cin, identifier);
        
        if (foodDb->getFood(identifier)) {
            std::cout << "A food with this identifier already exists.\n";
            return;
        }
        
        std::cout << "Enter keywords (comma separated): ";
        std::string keywordsStr;
        std::getline(std::cin, keywordsStr);
        
        std::vector<std::string> keywords;
        std::istringstream iss(keywordsStr);
        std::string keyword;
        while (std::getline(iss, keyword, ',')) {
            // Trim whitespace
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);
            if (!keyword.empty()) {
                keywords.push_back(keyword);
            }
        }
        
        double calories;
        std::cout << "Enter calories per serving: ";
        std::cin >> calories;
        std::cin.ignore();
        
        auto food = std::make_shared<BasicFood>(identifier, keywords, calories);
        if (foodDb->addFood(food)) {
            std::cout << "Basic food added successfully.\n";
        } else {
            std::cout << "Failed to add food.\n";
        }
    }
    
    void createCompositeFood() {
        std::cout << "\n===== Create Composite Food =====\n";
        
        std::string identifier;
        std::cout << "Enter food identifier: ";
        std::getline(std::cin, identifier);
        
        if (foodDb->getFood(identifier)) {
            std::cout << "A food with this identifier already exists.\n";
            return;
        }
        
        std::cout << "Enter keywords (comma separated): ";
        std::string keywordsStr;
        std::getline(std::cin, keywordsStr);
        
        std::vector<std::string> keywords;
        std::istringstream iss(keywordsStr);
        std::string keyword;
        while (std::getline(iss, keyword, ',')) {
            // Trim whitespace
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);
            if (!keyword.empty()) {
                keywords.push_back(keyword);
            }
        }
        
        std::vector<FoodComponent> components;
        bool addingComponents = true;
        
        while (addingComponents) {
            // Show available foods
            auto foods = foodDb->getAllFoods();
            std::cout << "\nAvailable Foods:\n";
            for (size_t i = 0; i < foods.size(); ++i) {
                std::cout << i + 1 << ". " << foods[i]->getIdentifier() << "\n";
            }
            
            int foodIndex;
            std::cout << "Select food number (0 to finish): ";
            std::cin >> foodIndex;
            std::cin.ignore();
            
            if (foodIndex == 0) {
                addingComponents = false;
            } else if (foodIndex >= 1 && foodIndex <= static_cast<int>(foods.size())) {
                double servings;
                std::cout << "Enter number of servings: ";
                std::cin >> servings;
                std::cin.ignore();
                
                components.push_back(FoodComponent(foods[foodIndex - 1], servings));
                std::cout << "Component added.\n";
            } else {
                std::cout << "Invalid food number.\n";
            }
        }
        
        if (components.empty()) {
            std::cout << "Composite food must have at least one component.\n";
            return;
        }
        
        auto food = std::make_shared<CompositeFood>(identifier, keywords, components);
        if (foodDb->addFood(food)) {
            std::cout << "Composite food created successfully.\n";
        } else {
            std::cout << "Failed to create composite food.\n";
        }
    }
    
    void logFoods() {
        while (true) {
            std::cout << "\n===== Log Foods for " << log.getCurrentDate() << " =====\n";
            std::cout << log.getCurrentDayLog().toString();
            std::cout << "\n1. Add Food to Log\n";
            std::cout << "2. Remove Food from Log\n";
            std::cout << "3. Back to Main Menu\n";
            std::cout << "Enter choice: ";
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: addFoodToLog(); break;
                case 2: removeFoodFromLog(); break;
                case 3: return;
                default: std::cout << "Invalid choice. Try again.\n";
            }
        }
    }
    
    void addFoodToLog() {
        std::cout << "\n===== Add Food to Log =====\n";
        
        // Show available foods or search
        std::cout << "1. Select from all foods\n";
        std::cout << "2. Search for food\n";
        std::cout << "Enter choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        std::vector<std::shared_ptr<Food>> foods;
        
        if (choice == 1) {
            foods = foodDb->getAllFoods();
        } else if (choice == 2) {
            std::cout << "Enter keywords (comma separated): ";
            std::string keywordsStr;
            std::getline(std::cin, keywordsStr);
            
            std::vector<std::string> keywords;
            std::istringstream iss(keywordsStr);
            std::string keyword;
            while (std::getline(iss, keyword, ',')) {
                // Trim whitespace
                keyword.erase(0, keyword.find_first_not_of(" \t"));
                keyword.erase(keyword.find_last_not_of(" \t") + 1);
                if (!keyword.empty()) {
                    keywords.push_back(keyword);
                }
            }
            
            std::cout << "Match (1) All keywords or (2) Any keyword? ";
            int matchChoice;
            std::cin >> matchChoice;
            std::cin.ignore();
            
            bool matchAll = (matchChoice == 1);
            foods = foodDb->findFoods(keywords, matchAll);
        } else {
            std::cout << "Invalid choice.\n";
            return;
        }
        
        if (foods.empty()) {
            std::cout << "No foods available.\n";
            return;
        }
        
        std::cout << "\nAvailable Foods:\n";
        for (size_t i = 0; i < foods.size(); ++i) {
            std::cout << i + 1 << ". " << foods[i]->getIdentifier() << " (" 
                      << foods[i]->getCaloriesPerServing() << " calories per serving)\n";
        }
        
        int foodIndex;
        std::cout << "Select food number: ";
        std::cin >> foodIndex;
        std::cin.ignore();
        
        if (foodIndex < 1 || foodIndex > static_cast<int>(foods.size())) {
            std::cout << "Invalid food number.\n";
            return;
        }
        
        double servings;
        std::cout << "Enter number of servings: ";
        std::cin >> servings;
        std::cin.ignore();
        
        auto command = std::make_shared<AddFoodCommand>(log, foods[foodIndex - 1], servings);
        undoManager.executeCommand(command);
        std::cout << "Food added to log.\n";
    }
    
    void removeFoodFromLog() {
        std::cout << "\n===== Remove Food from Log =====\n";
        
        const auto& entries = log.getCurrentDayLog().getEntries();
        if (entries.empty()) {
            std::cout << "No entries to remove.\n";
            return;
        }
        
        std::cout << "Current Entries:\n";
        for (size_t i = 0; i < entries.size(); ++i) {
            std::cout << i + 1 << ". " << entries[i].toString() << "\n";
        }
        
        int entryIndex;
        std::cout << "Select entry number to remove: ";
        std::cin >> entryIndex;
        std::cin.ignore();
        
        if (entryIndex < 1 || entryIndex > static_cast<int>(entries.size())) {
            std::cout << "Invalid entry number.\n";
            return;
        }
        
        auto command = std::make_shared<RemoveFoodCommand>(log, entryIndex - 1);
        undoManager.executeCommand(command);
        std::cout << "Entry removed from log.\n";
    }
    
    void manageProfile() {
        while (true) {
            std::cout << "\n===== Manage Profile =====\n";
            std::cout << "1. View Profile\n";
            std::cout << "2. Edit Basic Information\n";
            std::cout << "3. Update Current Weight\n";
            std::cout << "4. Update Activity Level\n";
            std::cout << "5. Change Target Calorie Calculator\n";
            std::cout << "6. Back to Main Menu\n";
            std::cout << "Enter choice: ";
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: viewProfile(); break;
                case 2: editBasicInfo(); break;
                case 3: updateWeight(); break;
                case 4: updateActivityLevel(); break;
                case 5: changeCalculator(); break;
                case 6: return;
                default: std::cout << "Invalid choice. Try again.\n";
            }
        }
    }
    
    void viewProfile() {
        std::cout << "\n===== Profile Information =====\n";
        std::cout << "Gender: " << (profile.getGender() == Gender::Male ? "Male" : "Female") << "\n";
        std::cout << "Height: " << profile.getHeight() << " cm\n";
        std::cout << "Age: " << profile.getAge() << " years\n";
        std::cout << "Current Weight: " << profile.getWeight(log.getCurrentDate()) << " kg\n";
        
        ActivityLevel level = profile.getActivityLevel(log.getCurrentDate());
        std::cout << "Activity Level: ";
        switch (level) {
            case ActivityLevel::Sedentary: std::cout << "Sedentary"; break;
            case ActivityLevel::LightlyActive: std::cout << "Lightly Active"; break;
            case ActivityLevel::ModeratelyActive: std::cout << "Moderately Active"; break;
            case ActivityLevel::VeryActive: std::cout << "Very Active"; break;
            case ActivityLevel::ExtremelyActive: std::cout << "Extremely Active"; break;
        }
        std::cout << "\n";
        
        std::cout << "Target Calorie Calculator: " << profile.getCalculator()->getName() << "\n";
        std::cout << "Target Calories: " << profile.getTargetCalories(log.getCurrentDate()) << " calories\n";
    }
    
    void editBasicInfo() {
        std::cout << "\n===== Edit Basic Information =====\n";
        
        int genderChoice;
        std::cout << "Select Gender (1: Male, 2: Female): ";
        std::cin >> genderChoice;
        Gender gender = (genderChoice == 2) ? Gender::Female : Gender::Male;
        profile.setGender(gender);
        
        double height;
        std::cout << "Enter Height (cm): ";
        std::cin >> height;
        profile.setHeight(height);
        
        int age;
        std::cout << "Enter Age: ";
        std::cin >> age;
        std::cin.ignore();
        profile.setAge(age);
        
        std::cout << "Basic information updated.\n";
    }
    
    void updateWeight() {
        std::cout << "\n===== Update Weight =====\n";
        std::cout << "Current weight for " << log.getCurrentDate() << ": " 
                  << profile.getWeight(log.getCurrentDate()) << " kg\n";
        
        double weight;
        std::cout << "Enter new weight (kg): ";
        std::cin >> weight;
        std::cin.ignore();
        
        profile.setWeight(log.getCurrentDate(), weight);
        std::cout << "Weight updated.\n";
    }
    
    void updateActivityLevel() {
        std::cout << "\n===== Update Activity Level =====\n";
        std::cout << "1. Sedentary\n";
        std::cout << "2. Lightly Active\n";
        std::cout << "3. Moderately Active\n";
        std::cout << "4. Very Active\n";
        std::cout << "5. Extremely Active\n";
        std::cout << "Select activity level: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice < 1 || choice > 5) {
            std::cout << "Invalid choice.\n";
            return;
        }
        
        ActivityLevel level = static_cast<ActivityLevel>(choice - 1);
        profile.setActivityLevel(log.getCurrentDate(), level);
        std::cout << "Activity level updated.\n";
    }
    
    void changeCalculator() {
        std::cout << "\n===== Change Target Calorie Calculator =====\n";
        std::cout << "1. Harris-Benedict Equation\n";
        std::cout << "2. Mifflin-St Jeor Equation\n";
        std::cout << "Select calculator: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 1) {
            profile.setCalculator(std::make_shared<HarrisBenedictCalculator>());
            std::cout << "Calculator changed to Harris-Benedict Equation.\n";
        } else if (choice == 2) {
            profile.setCalculator(std::make_shared<MifflinStJeorCalculator>());
            std::cout << "Calculator changed to Mifflin-St Jeor Equation.\n";
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
    
    void selectDate() {
        std::cout << "\n===== Select Date =====\n";
        std::cout << "Current date: " << log.getCurrentDate() << "\n";
        std::cout << "1. Enter specific date\n";
        std::cout << "2. Select from existing dates\n";
        std::cout << "Enter choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        std::string newDate;
        
        if (choice == 1) {
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::getline(std::cin, newDate);
            
            // Basic validation
            if (newDate.length() != 10 || newDate[4] != '-' || newDate[7] != '-') {
                std::cout << "Invalid date format. Use YYYY-MM-DD.\n";
                return;
            }
        } else if (choice == 2) {
            auto dates = log.getAllDates();
            if (dates.empty()) {
                std::cout << "No dates available in log.\n";
                return;
            }
            
            std::cout << "Available Dates:\n";
            for (size_t i = 0; i < dates.size(); ++i) {
                std::cout << i + 1 << ". " << dates[i] << "\n";
            }
            
            int dateIndex;
            std::cout << "Select date number: ";
            std::cin >> dateIndex;
            std::cin.ignore();
            
            if (dateIndex < 1 || dateIndex > static_cast<int>(dates.size())) {
                std::cout << "Invalid date number.\n";
                return;
            }
            
            newDate = dates[dateIndex - 1];
        } else {
            std::cout << "Invalid choice.\n";
            return;
        }
        
        auto command = std::make_shared<ChangeDateCommand>(log, newDate);
        undoManager.executeCommand(command);
        std::cout << "Date changed to " << newDate << ".\n";
    }
    
    void saveData() {
        std::cout << "\n===== Saving Data =====\n";
        
        bool foodDbSaved = foodDb->saveDatabase();
        bool logSaved = log.saveLog();
        bool profileSaved = profile.saveProfile();
        
        if (foodDbSaved && logSaved && profileSaved) {
            std::cout << "All data saved successfully.\n";
        } else {
            std::cout << "Some data could not be saved.\n";
            if (!foodDbSaved) std::cout << "- Food database not saved.\n";
            if (!logSaved) std::cout << "- Daily log not saved.\n";
            if (!profileSaved) std::cout << "- Profile not saved.\n";
        }
    }
    
public:
    DietManagerApp() 
        : foodDb(FoodDatabase::getInstance()), 
          tracker(log, profile),
          running(true) {
    }
    
    void init() {
        // Load data
        foodDb->loadDatabase();
        log.loadLog();
        profile.loadProfile();
        
        std::cout << "Welcome to YADA (Yet Another Diet Assistant)!\n";
    }
    
    void run() {
        init();
        
        while (running) {
            displayMainMenu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: manageFoods(); break;
                case 2: logFoods(); break;
                case 3: manageProfile(); break;
                case 4: selectDate(); break;
                case 5: 
                    if (undoManager.canUndo()) {
                        undoManager.undo();
                        std::cout << "Last action undone.\n";
                    } else {
                        std::cout << "Nothing to undo.\n";
                    }
                    break;
                case 6: saveData(); break;
                case 7: 
                    running = false;
                    saveData();
                    std::cout << "Thank you for using YADA. Goodbye!\n";
                    break;
                default: std::cout << "Invalid choice. Try again.\n";
            }
        }
    }
};

int main() {
    DietManagerApp app;
    app.run();
    return 0;
}