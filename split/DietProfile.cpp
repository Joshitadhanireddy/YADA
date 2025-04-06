#include "DietProfile.hpp"

DietProfile::DietProfile() 
    : gender(Gender::Male), heightCm(170), age(30), 
      calculator(std::make_shared<HarrisBenedictCalculator>()),
      profileFile("profile.txt") {
    // Initialize with default values
    std::string today = "2023-01-01"; // Default date for initialization
    weightsByDate[today] = 70.0;
    activityLevelsByDate[today] = ActivityLevel::ModeratelyActive;
}

void DietProfile::setProfileFile(const std::string& file) {
    profileFile = file;
}

void DietProfile::setGender(Gender g) {
    gender = g;
    notifyObservers();
}

Gender DietProfile::getGender() const {
    return gender;
}

void DietProfile::setHeight(double h) {
    heightCm = h;
    notifyObservers();
}

double DietProfile::getHeight() const {
    return heightCm;
}

void DietProfile::setAge(int a) {
    age = a;
    notifyObservers();
}

int DietProfile::getAge() const {
    return age;
}

void DietProfile::setWeight(const std::string& date, double weight) {
    weightsByDate[date] = weight;
    notifyObservers();
}

double DietProfile::getWeight(const std::string& date) const {
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

void DietProfile::setActivityLevel(const std::string& date, ActivityLevel level) {
    activityLevelsByDate[date] = level;
    notifyObservers();
}

ActivityLevel DietProfile::getActivityLevel(const std::string& date) const {
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

void DietProfile::setCalculator(std::shared_ptr<TargetCalorieCalculator> calc) {
    calculator = calc;
    notifyObservers();
}

std::shared_ptr<TargetCalorieCalculator> DietProfile::getCalculator() const {
    return calculator;
}

double DietProfile::getTargetCalories(const std::string& date) const {
    return calculator->calculateTargetCalories(
        gender, getWeight(date), heightCm, age, getActivityLevel(date));
}

bool DietProfile::loadProfile() {
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
        
        if (calculatorStr == "Harris-Benedict Equation") {
            calculator = std::make_shared<HarrisBenedictCalculator>();
        } else if (calculatorStr == "Mifflin-St Jeor Equation") {
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

bool DietProfile::saveProfile() {
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
    
    for (auto it = activityLevelsByDate.begin(); it != activityLevelsByDate.end(); ++it) {
        file << it->first << ":" << static_cast<int>(it->second);
        if (std::next(it) != activityLevelsByDate.end()) file << ",";
    }
    file << std::endl;
    
    file.close();
    return true;
}