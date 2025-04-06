#ifndef DIET_MANAGER_APP_HPP
#define DIET_MANAGER_APP_HPP

#include "FoodDatabase.hpp"
#include "DailyLog.hpp"
#include "DietProfile.hpp"
#include "Command.hpp"
#include "FoodTracker.hpp"
#include <iostream>
#include <string>
#include <limits>
#include <vector>

// Main application class
class DietManagerApp {
private:
    FoodDatabase* foodDb;
    DailyLog log;
    DietProfile profile;
    UndoManager undoManager;
    FoodTracker tracker;
    bool running;
    
    void displayMainMenu();
    void manageFoods();
    void viewAllFoods();
    void searchFoods();
    void addBasicFood();
    void createCompositeFood();
    void logFoods();
    void addFoodToLog();
    void removeFoodFromLog();
    void manageProfile();
    void viewProfile();
    void editBasicInfo();
    void updateWeight();
    void updateActivityLevel();
    void changeCalculator();
    void selectDate();
    void saveData();
    
public:
    DietManagerApp();
    void init();
    void run();
};

#endif // DIET_MANAGER_APP_HPP