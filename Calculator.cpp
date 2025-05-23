#include "Calculator.hpp"

// Harris-Benedict calculator implementation
double HarrisBenedictCalculator::calculateTargetCalories(Gender gender, double weightKg, double heightCm, int age, ActivityLevel activityLevel) const {
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

std::string HarrisBenedictCalculator::getName() const {
    return "Harris-Benedict Equation";
}

// Mifflin-St Jeor calculator implementation
double MifflinStJeorCalculator::calculateTargetCalories(Gender gender, double weightKg, double heightCm, int age, ActivityLevel activityLevel) const {
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

std::string MifflinStJeorCalculator::getName() const {
    return "Mifflin-St Jeor Equation";
}