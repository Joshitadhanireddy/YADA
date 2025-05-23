#include "Command.hpp"

// AddFoodCommand implementation
AddFoodCommand::AddFoodCommand(DailyLog& l, std::shared_ptr<Food> f, double s)
    : log(l), food(f), servings(s) {}

void AddFoodCommand::execute() {
    log.addFoodToCurrentDay(food, servings);
}

void AddFoodCommand::undo() {
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

std::string AddFoodCommand::toString() const {
    std::stringstream ss;
    ss << "Add " << servings << " serving(s) of " << food->getIdentifier();
    return ss.str();
}

// RemoveFoodCommand implementation
RemoveFoodCommand::RemoveFoodCommand(DailyLog& l, int i)
    : log(l), index(i), savedEntry(log.getCurrentDayLog().getEntries()[i]) {}

void RemoveFoodCommand::execute() {
    log.removeFoodFromCurrentDay(index);
}

void RemoveFoodCommand::undo() {
    log.addFoodToCurrentDay(savedEntry.food, savedEntry.servings);
}

std::string RemoveFoodCommand::toString() const {
    std::stringstream ss;
    ss << "Remove " << savedEntry.servings << " serving(s) of " << savedEntry.food->getIdentifier();
    return ss.str();
}
SetGenderCommand::SetGenderCommand(DietProfile& p, Gender newG)
    : profile(p), oldGender(p.getGender()), newGender(newG) {}

void SetGenderCommand::execute() {
    profile.setGender(newGender);
}

void SetGenderCommand::undo() {
    profile.setGender(oldGender);
}

std::string SetGenderCommand::toString() const {
    return "Change gender";
}
SetHeightCommand::SetHeightCommand(DietProfile& p, double newH)
    : profile(p), oldHeight(p.getHeight()), newHeight(newH) {}

void SetHeightCommand::execute() {
    profile.setHeight(newHeight);
}

void SetHeightCommand::undo() {
    profile.setHeight(oldHeight);
}

std::string SetHeightCommand::toString() const {
    std::stringstream ss;
    ss << "Change height from " << oldHeight << " to " << newHeight << " cm";
    return ss.str();
}

// SetAgeCommand implementation
SetAgeCommand::SetAgeCommand(DietProfile& p, int newA)
    : profile(p), oldAge(p.getAge()), newAge(newA) {}

void SetAgeCommand::execute() {
    profile.setAge(newAge);
}

void SetAgeCommand::undo() {
    profile.setAge(oldAge);
}

std::string SetAgeCommand::toString() const {
    std::stringstream ss;
    ss << "Change age from " << oldAge << " to " << newAge;
    return ss.str();
}

// SetWeightCommand implementation
SetWeightCommand::SetWeightCommand(DietProfile& p, const std::string& d, double newW)
    : profile(p), date(d), oldWeight(p.getWeight(d)), newWeight(newW) {}

void SetWeightCommand::execute() {
    profile.setWeight(date, newWeight);
}

void SetWeightCommand::undo() {
    profile.setWeight(date, oldWeight);
}

std::string SetWeightCommand::toString() const {
    std::stringstream ss;
    ss << "Change weight for " << date << " from " << oldWeight << " to " << newWeight << " kg";
    return ss.str();
}

// SetActivityLevelCommand implementation
SetActivityLevelCommand::SetActivityLevelCommand(DietProfile& p, const std::string& d, ActivityLevel newL)
    : profile(p), date(d), oldLevel(p.getActivityLevel(d)), newLevel(newL) {}

void SetActivityLevelCommand::execute() {
    profile.setActivityLevel(date, newLevel);
}

void SetActivityLevelCommand::undo() {
    profile.setActivityLevel(date, oldLevel);
}

std::string SetActivityLevelCommand::toString() const {
    std::stringstream ss;
    ss << "Change activity level for " << date;
    return ss.str();
}

// SetCalculatorCommand implementation
SetCalculatorCommand::SetCalculatorCommand(DietProfile& p, std::shared_ptr<TargetCalorieCalculator> newCalc)
    : profile(p), oldCalculator(p.getCalculator()), newCalculator(newCalc) {}

void SetCalculatorCommand::execute() {
    profile.setCalculator(newCalculator);
}

void SetCalculatorCommand::undo() {
    profile.setCalculator(oldCalculator);
}

std::string SetCalculatorCommand::toString() const {
    return "Change calorie calculator";
}
// ChangeDateCommand implementation
ChangeDateCommand::ChangeDateCommand(DailyLog& l, const std::string& newD)
    : log(l), oldDate(l.getCurrentDate()), newDate(newD) {}

void ChangeDateCommand::execute() {
    log.setCurrentDate(newDate);
}

void ChangeDateCommand::undo() {
    log.setCurrentDate(oldDate);
}

std::string ChangeDateCommand::toString() const {
    std::stringstream ss;
    ss << "Change date from " << oldDate << " to " << newDate;
    return ss.str();
}

// UndoManager implementation
bool UndoManager::executeCommand(std::shared_ptr<Command> command) {
    command->execute();
    undoStack.push(command);
    std::cout << "Command executed: " << command->toString() << "\n";
    return true;
}

bool UndoManager::canUndo() const {
    return !undoStack.empty();
}

void UndoManager::undo() {
    if (canUndo()) {
        undoStack.top()->undo();
        undoStack.pop();
    }
}

std::vector<std::string> UndoManager::getCommandHistory() const {
    std::vector<std::string> history;
    std::stack<std::shared_ptr<Command>> tempStack = undoStack;
    
    while (!tempStack.empty()) {
        history.push_back(tempStack.top()->toString());
        tempStack.pop();
    }
    
    std::reverse(history.begin(), history.end());
    return history;
}

void UndoManager::clearHistory() {
    while (!undoStack.empty()) {
        undoStack.pop();
    }
}
AddFoodToDbCommand::AddFoodToDbCommand(FoodDatabase* db, std::shared_ptr<Food> f)
    : foodDb(db), food(f) {}

void AddFoodToDbCommand::execute() {
    foodDb->addFood(food);
}

void AddFoodToDbCommand::undo() {
    foodDb->removeFood(food->getIdentifier());
}

std::string AddFoodToDbCommand::toString() const {
    std::stringstream ss;
    ss << "Add food '" << food->getIdentifier() << "' to database";
    return ss.str();
}