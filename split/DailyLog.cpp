#include "DailyLog.hpp"
#include "FoodDatabase.hpp"

LogEntry::LogEntry(std::shared_ptr<Food> f, double s) : food(f), servings(s) {}

double LogEntry::getCalories() const {
    return food->getCaloriesPerServing() * servings;
}

std::string LogEntry::toString() const {
    std::stringstream ss;
    ss << servings << " serving(s) of " << food->getIdentifier() 
       << " (" << getCalories() << " calories)";
    return ss.str();
}

std::string LogEntry::serialize() const {
    std::stringstream ss;
    ss << food->getIdentifier() << ":" << servings;
    return ss.str();
}

// DayLog implementation
void DayLog::addEntry(const LogEntry& entry) {
    entries.push_back(entry);
}

void DayLog::removeEntry(int index) {
    if (index >= 0 && index < static_cast<int>(entries.size())) {
        entries.erase(entries.begin() + index);
    }
}

const std::vector<LogEntry>& DayLog::getEntries() const {
    return entries;
}

double DayLog::getTotalCalories() const {
    double total = 0.0;
    for (const auto& entry : entries) {
        total += entry.getCalories();
    }
    return total;
}

std::string DayLog::toString() const {
    std::stringstream ss;
    ss << "Daily Food Log:\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        ss << i + 1 << ". " << entries[i].toString() << "\n";
    }
    ss << "Total Calories: " << getTotalCalories() << "\n";
    return ss.str();
}

// DailyLog implementation
DailyLog::DailyLog() : logFile("dailylog.txt") {
    // Set current date as default
    time_t now = time(0);
    tm* ltm = localtime(&now);
    currentDate = formatDate(1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
}

std::string DailyLog::formatDate(int year, int month, int day) {
    std::stringstream ss;
    ss << year << "-" 
       << (month < 10 ? "0" : "") << month << "-"
       << (day < 10 ? "0" : "") << day;
    return ss.str();
}

void DailyLog::setLogFile(const std::string& file) {
    logFile = file;
}

const std::string& DailyLog::getCurrentDate() const {
    return currentDate;
}

void DailyLog::setCurrentDate(const std::string& date) {
    currentDate = date;
    notifyObservers();
}

DayLog& DailyLog::getCurrentDayLog() {
    return logs[currentDate];
}

const DayLog& DailyLog::getCurrentDayLog() const {
    return logs.at(currentDate);
}

bool DailyLog::dateExists(const std::string& date) const {
    return logs.find(date) != logs.end();
}

std::vector<std::string> DailyLog::getAllDates() const {
    std::vector<std::string> dates;
    for (const auto& pair : logs) {
        dates.push_back(pair.first);
    }
    std::sort(dates.begin(), dates.end());
    return dates;
}

void DailyLog::addFoodToCurrentDay(std::shared_ptr<Food> food, double servings) {
    logs[currentDate].addEntry(LogEntry(food, servings));
    notifyObservers();
}

void DailyLog::removeFoodFromCurrentDay(int index) {
    logs[currentDate].removeEntry(index);
    notifyObservers();
}

bool DailyLog::loadLog() {
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

bool DailyLog::saveLog() {
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