#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "common.hpp"
#include <string>

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
    double calculateTargetCalories(Gender gender, double weightKg, double heightCm, int age, ActivityLevel activityLevel) const override;
    std::string getName() const override;
};

// Mifflin-St Jeor formula
class MifflinStJeorCalculator : public TargetCalorieCalculator {
public:
    double calculateTargetCalories(Gender gender, double weightKg, double heightCm, int age, ActivityLevel activityLevel) const override;
    std::string getName() const override;
};

#endif // CALCULATOR_H