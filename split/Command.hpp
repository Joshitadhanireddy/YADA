#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "DailyLog.hpp"
#include <string>
#include <sstream>
#include <memory>
#include <stack>
#include <vector>
#include <algorithm>
#include "FoodDatabase.hpp"
// Command pattern for undo functionality
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string toString() const = 0;
};

// Add food command
class AddFoodCommand : public Command {
private:
    DailyLog& log;
    std::shared_ptr<Food> food;
    double servings;
    
public:
    AddFoodCommand(DailyLog& l, std::shared_ptr<Food> f, double s);
    
    void execute() override;
    void undo() override;
    std::string toString() const override;
};

// Remove food command
class RemoveFoodCommand : public Command {
private:
    DailyLog& log;
    int index;
    LogEntry savedEntry;
    
public:
    RemoveFoodCommand(DailyLog& l, int i);
    
    void execute() override;
    void undo() override;
    std::string toString() const override;
};

// Change date command
class ChangeDateCommand : public Command {
private:
    DailyLog& log;
    std::string oldDate;
    std::string newDate;
    
public:
    ChangeDateCommand(DailyLog& l, const std::string& newD);
    
    void execute() override;
    void undo() override;
    std::string toString() const override;
};
class AddFoodToDbCommand : public Command {
    private:
        FoodDatabase* foodDb; 
        std::shared_ptr<Food> food;
        
    public:
        AddFoodToDbCommand(FoodDatabase* db, std::shared_ptr<Food> f);
        void execute() override;
        void undo() override;
        std::string toString() const override;
    };
// UndoManager class
class UndoManager {
private:
    std::stack<std::shared_ptr<Command>> undoStack;
    
public:
    bool executeCommand(std::shared_ptr<Command> command);
    bool canUndo() const;
    void undo();
    std::vector<std::string> getCommandHistory() const;
    void clearHistory();
};

#endif // COMMAND_HPP