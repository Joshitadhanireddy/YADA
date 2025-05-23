#ifndef DIET_PROFILE_HPP
#define DIET_PROFILE_HPP

#include "Observer.hpp"
#include "common.hpp"
#include "Calculator.hpp"
#include <string>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>

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
    DietProfile();
    
    void setProfileFile(const std::string& file);
    
    void setGender(Gender g);
    Gender getGender() const;
    
    void setHeight(double h);
    double getHeight() const;
    
    void setAge(int a);
    int getAge() const;
    
    void setWeight(const std::string& date, double weight);
    double getWeight(const std::string& date) const;
    
    void setActivityLevel(const std::string& date, ActivityLevel level);
    ActivityLevel getActivityLevel(const std::string& date) const;
    
    void setCalculator(std::shared_ptr<TargetCalorieCalculator> calc);
    std::shared_ptr<TargetCalorieCalculator> getCalculator() const;
    
    double getTargetCalories(const std::string& date) const;
    
    bool loadProfile();
    bool saveProfile();
};

#endif // DIET_PROFILE_HPP