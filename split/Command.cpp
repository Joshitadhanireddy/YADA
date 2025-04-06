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
void UndoManager::executeCommand(std::shared_ptr<Command> command) {
    command->execute();
    undoStack.push(command);
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