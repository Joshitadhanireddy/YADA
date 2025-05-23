#ifndef FOOD_TRACKER_HPP
#define FOOD_TRACKER_HPP

#include "Observer.hpp"
#include "DailyLog.hpp"
#include "DietProfile.hpp"
#include <iostream>
#include <iomanip>

// FoodTracker class that implements Observer
class FoodTracker : public Observer {
private:
    DailyLog& log;
    DietProfile& profile;
    
public:
    FoodTracker(DailyLog& l, DietProfile& p);
    ~FoodTracker();
    
    void update(Subject* subject = nullptr) override;
    void displayDailySummary() const;
};

#endif // FOOD_TRACKER_HPP