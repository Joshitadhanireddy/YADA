#include "FoodTracker.hpp"

FoodTracker::FoodTracker(DailyLog& l, DietProfile& p) : log(l), profile(p) {
    log.addObserver(this);
    profile.addObserver(this);
}

FoodTracker::~FoodTracker() {
    log.removeObserver(this);
    profile.removeObserver(this);
}

void FoodTracker::update() {
    // This method is called when either log or profile changes
    displayDailySummary();
}

void FoodTracker::displayDailySummary() const {
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