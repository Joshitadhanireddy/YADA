#ifndef DAILY_LOG_HPP
#define DAILY_LOG_HPP

#include "Observer.hpp"
#include "Food.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

// Forward declaration
class FoodDatabase;

// LogEntry class for individual food entries
class LogEntry {
public:
    std::shared_ptr<Food> food;
    double servings;
    
    LogEntry(std::shared_ptr<Food> f, double s);
    double getCalories() const;
    std::string toString() const;
    std::string serialize() const;
};

// DayLog class for a single day's log
class DayLog {
private:
    std::vector<LogEntry> entries;
    
public:
    void addEntry(const LogEntry& entry);
    void removeEntry(int index);
    const std::vector<LogEntry>& getEntries() const;
    double getTotalCalories() const;
    std::string toString() const;
};

// DailyLog class for all dates
class DailyLog : public Subject {
private:
    std::map<std::string, DayLog> logs;
    std::string currentDate;
    std::string logFile;
    
    std::string formatDate(int year, int month, int day);
    
public:
    DailyLog();
    
    void setLogFile(const std::string& file);
    const std::string& getCurrentDate() const;
    void setCurrentDate(const std::string& date);
    DayLog& getCurrentDayLog();
    const DayLog& getCurrentDayLog() const;
    bool dateExists(const std::string& date) const;
    std::vector<std::string> getAllDates() const;
    void addFoodToCurrentDay(std::shared_ptr<Food> food, double servings);
    void removeFoodFromCurrentDay(int index);
    bool loadLog();
    bool saveLog();
};

#endif // DAILY_LOG_HPP