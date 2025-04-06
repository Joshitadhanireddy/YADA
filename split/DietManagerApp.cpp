#include "DietManagerApp.hpp"

DietManagerApp::DietManagerApp() 
    : foodDb(FoodDatabase::getInstance()), 
      tracker(log, profile),
      running(true) {
}

void DietManagerApp::init() {
    // Load data
    foodDb->loadDatabase();
    log.loadLog();
    profile.loadProfile();
    
    std::cout << "Welcome to YADA (Yet Another Diet Assistant)!\n";
}

void DietManagerApp::run() {
    init();
    
    while (running) {
        displayMainMenu();
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: manageFoods(); break;
            case 2: logFoods(); break;
            case 3: manageProfile(); break;
            case 4: selectDate(); break;
            case 5: 
                if (undoManager.canUndo()) {
                    undoManager.undo();
                    std::cout << "Last action undone.\n";
                } else {
                    std::cout << "Nothing to undo.\n";
                }
                break;
            case 6: 
                running = false;
                saveData();
                std::cout << "Thank you for using YADA. Goodbye!\n";
                break;
            default: std::cout << "Invalid choice. Try again.\n";
        }
    }
}

void DietManagerApp::displayMainMenu() {
    std::cout << "\n===== YADA (Yet Another Diet Assistant) =====\n";
    std::cout << "Current Date: " << log.getCurrentDate() << "\n";
    std::cout << "1. Manage Foods\n";
    std::cout << "2. Log Foods\n";
    std::cout << "3. Manage Profile\n";
    std::cout << "4. Select Date\n";
    std::cout << "5. Undo Last Action\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter choice: ";
}

void DietManagerApp::manageFoods() {
    while (true) {
        std::cout << "\n===== Manage Foods =====\n";
        std::cout << "1. View All Foods\n";
        std::cout << "2. Search Foods\n";
        std::cout << "3. Add Basic Food\n";
        std::cout << "4. Create Composite Food\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter choice: ";
        
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore();
        switch (choice) {
            case 1: viewAllFoods(); break;
            case 2: searchFoods(); break;
            case 3: addBasicFood(); break;
            case 4: createCompositeFood(); break;
            case 5: return;
            default: std::cout << "Invalid choice. Try again.\n";
        }
    }
}

void DietManagerApp::viewAllFoods() {
    std::cout << "\n===== All Foods =====\n";
    auto foods = foodDb->getAllFoods();
    if (foods.empty()) {
        std::cout << "No foods in database.\n";
        return;
    }
    
    for (size_t i = 0; i < foods.size(); ++i) {
        std::cout << i + 1 << ". " << foods[i]->toString() << "\n";
    }
}

void DietManagerApp::searchFoods() {
    std::cout << "\n===== Search Foods =====\n";
    std::cout << "Enter keywords (comma separated): ";
    std::string keywordsStr;
    std::getline(std::cin, keywordsStr);
    
    std::vector<std::string> keywords;
    std::istringstream iss(keywordsStr);
    std::string keyword;
    while (std::getline(iss, keyword, ',')) {
        // Trim whitespace
        keyword.erase(0, keyword.find_first_not_of(" \t"));
        keyword.erase(keyword.find_last_not_of(" \t") + 1);
        if (!keyword.empty()) {
            keywords.push_back(keyword);
        }
    }
    
    std::cout << "Match (1) All keywords or (2) Any keyword? ";
    int matchChoice;
    std::cin >> matchChoice;
    std::cin.ignore();
    
    bool matchAll = (matchChoice == 1);
    auto results = foodDb->findFoods(keywords, matchAll);
    
    std::cout << "\n===== Search Results =====\n";
    if (results.empty()) {
        std::cout << "No matching foods found.\n";
        return;
    }
    
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << i + 1 << ". " << results[i]->toString() << "\n";
    }
}

void DietManagerApp::addBasicFood() {
    std::cout << "\n===== Add Basic Food =====\n";
    
    std::string identifier;
    std::cout << "Enter food identifier: ";
    std::getline(std::cin, identifier);
    
    if (foodDb->getFood(identifier)) {
        std::cout << "A food with this identifier already exists.\n";
        return;
    }
    
    std::cout << "Enter keywords (comma separated): ";
    std::string keywordsStr;
    std::getline(std::cin, keywordsStr);
    
    std::vector<std::string> keywords;
    std::istringstream iss(keywordsStr);
    std::string keyword;
    while (std::getline(iss, keyword, ',')) {
        // Trim whitespace
        keyword.erase(0, keyword.find_first_not_of(" \t"));
        keyword.erase(keyword.find_last_not_of(" \t") + 1);
        if (!keyword.empty()) {
            keywords.push_back(keyword);
        }
    }
    
    double calories;
    std::cout << "Enter calories per serving: ";
    std::cin >> calories;
    std::cin.ignore();
    
    auto food = std::make_shared<BasicFood>(identifier, keywords, calories);
    if (foodDb->addFood(food)) {
        saveData();
        std::cout << "Basic food added successfully.\n";
    } else {
        std::cout << "Failed to add food.\n";
    }
}

void DietManagerApp::createCompositeFood() {
    std::cout << "\n===== Create Composite Food =====\n";
    
    std::string identifier;
    std::cout << "Enter food identifier: ";
    std::getline(std::cin, identifier);
    
    if (foodDb->getFood(identifier)) {
        std::cout << "A food with this identifier already exists.\n";
        return;
    }
    
    std::cout << "Enter keywords (comma separated): ";
    std::string keywordsStr;
    std::getline(std::cin, keywordsStr);
    
    std::vector<std::string> keywords;
    std::istringstream iss(keywordsStr);
    std::string keyword;
    while (std::getline(iss, keyword, ',')) {
        // Trim whitespace
        keyword.erase(0, keyword.find_first_not_of(" \t"));
        keyword.erase(keyword.find_last_not_of(" \t") + 1);
        if (!keyword.empty()) {
            keywords.push_back(keyword);
        }
    }
    
    std::vector<FoodComponent> components;
    bool addingComponents = true;
    
    while (addingComponents) {
        // Show available foods
        auto foods = foodDb->getAllFoods();
        std::cout << "\nAvailable Foods:\n";
        for (size_t i = 0; i < foods.size(); ++i) {
            std::cout << i + 1 << ". " << foods[i]->getIdentifier() << "\n";
        }
        
        int foodIndex;
        std::cout << "Select food number (0 to finish): ";
        std::cin >> foodIndex;
        std::cin.ignore();
        
        if (foodIndex == 0) {
            addingComponents = false;
        } else if (foodIndex >= 1 && foodIndex <= static_cast<int>(foods.size())) {
            double servings;
            std::cout << "Enter number of servings: ";
            std::cin >> servings;
            std::cin.ignore();
            
            components.push_back(FoodComponent(foods[foodIndex - 1], servings));
            std::cout << "Component added.\n";
        } else {
            std::cout << "Invalid food number.\n";
        }
    }
    
    if (components.empty()) {
        std::cout << "Composite food must have at least one component.\n";
        return;
    }
    
    auto food = std::make_shared<CompositeFood>(identifier, keywords, components);
    if (foodDb->addFood(food)) {
        saveData();
        std::cout << "Composite food created successfully.\n";
    } else {
        std::cout << "Failed to create composite food.\n";
    }
}

void DietManagerApp::logFoods() {
    while (true) {
        std::cout << "\n===== Log Foods for " << log.getCurrentDate() << " =====\n";
        std::cout << log.getCurrentDayLog().toString();
        std::cout << "\n1. Add Food to Log\n";
        std::cout << "2. Remove Food from Log\n";
        std::cout << "3. Back to Main Menu\n";
        std::cout << "Enter choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: addFoodToLog(); break;
            case 2: removeFoodFromLog(); break;
            case 3: return;
            default: std::cout << "Invalid choice. Try again.\n";
        }
    }
}

void DietManagerApp::addFoodToLog() {
    std::cout << "\n===== Add Food to Log =====\n";
    
    // Show available foods or search
    std::cout << "1. Select from all foods\n";
    std::cout << "2. Search for food\n";
    std::cout << "Enter choice: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    std::vector<std::shared_ptr<Food>> foods;
    
    if (choice == 1) {
        foods = foodDb->getAllFoods();
    } else if (choice == 2) {
        std::cout << "Enter keywords (comma separated): ";
        std::string keywordsStr;
        std::getline(std::cin, keywordsStr);
        
        std::vector<std::string> keywords;
        std::istringstream iss(keywordsStr);
        std::string keyword;
        while (std::getline(iss, keyword, ',')) {
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);
            std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
            if (!keyword.empty()) {
                keywords.push_back(keyword);
            }
        }
        
        std::cout << "Match (1) All keywords or (2) Any keyword? ";
        int matchChoice;
        std::cin >> matchChoice;
        std::cin.ignore();
        
        bool matchAll = (matchChoice == 1);
        foods = foodDb->findFoods(keywords, matchAll);
    } else {
        std::cout << "Invalid choice.\n";
        return;
    }
    
    if (foods.empty()) {
        std::cout << "No foods available.\n";
        return;
    }
    
    std::cout << "\nAvailable Foods:\n";
    for (size_t i = 0; i < foods.size(); ++i) {
        std::cout << i + 1 << ". " << foods[i]->getIdentifier() << " (" 
                << foods[i]->getCaloriesPerServing() << " calories per serving)\n";
    }
    
    int foodIndex;
    std::cout << "Select food number: ";
    std::cin >> foodIndex;
    std::cin.ignore();
    
    if (foodIndex < 1 || foodIndex > static_cast<int>(foods.size())) {
        std::cout << "Invalid food number.\n";
        return;
    }
    
    double servings;
    std::cout << "Enter number of servings: ";
    std::cin >> servings;
    std::cin.ignore();
    
    auto command = std::make_shared<AddFoodCommand>(log, foods[foodIndex - 1], servings);
    undoManager.executeCommand(command);
    saveData();
    std::cout << "Food added to log.\n";
}

void DietManagerApp::removeFoodFromLog() {
    std::cout << "\n===== Remove Food from Log =====\n";
    
    const auto& entries = log.getCurrentDayLog().getEntries();
    if (entries.empty()) {
        std::cout << "No entries to remove.\n";
        return;
    }
    
    std::cout << "Current Entries:\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        std::cout << i + 1 << ". " << entries[i].toString() << "\n";
    }
    
    int entryIndex;
    std::cout << "Select entry number to remove: ";
    std::cin >> entryIndex;
    std::cin.ignore();
    
    if (entryIndex < 1 || entryIndex > static_cast<int>(entries.size())) {
        std::cout << "Invalid entry number.\n";
        return;
    }
    
    auto command = std::make_shared<RemoveFoodCommand>(log, entryIndex - 1);
    undoManager.executeCommand(command);
    saveData();
    std::cout << "Entry removed from log.\n";
}

void DietManagerApp::manageProfile() {
    while (true) {
        std::cout << "\n===== Manage Profile =====\n";
        std::cout << "1. View Profile\n";
        std::cout << "2. Edit Basic Information\n";
        std::cout << "3. Update Current Weight\n";
        std::cout << "4. Update Activity Level\n";
        std::cout << "5. Change Target Calorie Calculator\n";
        std::cout << "6. Back to Main Menu\n";
        std::cout << "Enter choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: viewProfile(); break;
            case 2: editBasicInfo(); break;
            case 3: updateWeight(); break;
            case 4: updateActivityLevel(); break;
            case 5: changeCalculator(); break;
            case 6: return;
            default: std::cout << "Invalid choice. Try again.\n";
        }
    }
}

void DietManagerApp::viewProfile() {
    std::cout << "\n===== Profile Information =====\n";
    std::cout << "Gender: " << (profile.getGender() == Gender::Male ? "Male" : "Female") << "\n";
    std::cout << "Height: " << profile.getHeight() << " cm\n";
    std::cout << "Age: " << profile.getAge() << " years\n";
    std::cout << "Current Weight: " << profile.getWeight(log.getCurrentDate()) << " kg\n";
    
    ActivityLevel level = profile.getActivityLevel(log.getCurrentDate());
    std::cout << "Activity Level: ";
    switch (level) {
        case ActivityLevel::Sedentary: std::cout << "Sedentary"; break;
        case ActivityLevel::LightlyActive: std::cout << "Lightly Active"; break;
        case ActivityLevel::ModeratelyActive: std::cout << "Moderately Active"; break;
        case ActivityLevel::VeryActive: std::cout << "Very Active"; break;
        case ActivityLevel::ExtremelyActive: std::cout << "Extremely Active"; break;
    }
    std::cout << "\n";
    
    std::cout << "Target Calorie Calculator: " << profile.getCalculator()->getName() << "\n";
    std::cout << "Target Calories: " << profile.getTargetCalories(log.getCurrentDate()) << " calories\n";
}

void DietManagerApp::editBasicInfo() {
    std::cout << "\n===== Edit Basic Information =====\n";
    
    int genderChoice;
    std::cout << "Select Gender (1: Male, 2: Female): ";
    std::cin >> genderChoice;
    Gender gender = (genderChoice == 2) ? Gender::Female : Gender::Male;
    profile.setGender(gender);
    
    double height;
    std::cout << "Enter Height (cm): ";
    std::cin >> height;
    profile.setHeight(height);
    
    int age;
    std::cout << "Enter Age: ";
    std::cin >> age;
    std::cin.ignore();
    profile.setAge(age);

    saveData();

    std::cout << "Basic information updated.\n";
}

void DietManagerApp::updateWeight() {
    std::cout << "\n===== Update Weight =====\n";
    std::cout << "Current weight for " << log.getCurrentDate() << ": " 
              << profile.getWeight(log.getCurrentDate()) << " kg\n";
    
    double weight;
    std::cout << "Enter new weight (kg): ";
    std::cin >> weight;
    std::cin.ignore();
    
    profile.setWeight(log.getCurrentDate(), weight);

    saveData();

    std::cout << "Weight updated.\n";
}

void DietManagerApp::updateActivityLevel() {
    std::cout << "\n===== Update Activity Level =====\n";
    std::cout << "1. Sedentary\n";
    std::cout << "2. Lightly Active\n";
    std::cout << "3. Moderately Active\n";
    std::cout << "4. Very Active\n";
    std::cout << "5. Extremely Active\n";
    std::cout << "Select activity level: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice < 1 || choice > 5) {
        std::cout << "Invalid choice.\n";
        return;
    }
    
    ActivityLevel level = static_cast<ActivityLevel>(choice - 1);
    profile.setActivityLevel(log.getCurrentDate(), level);

    saveData();

    std::cout << "Activity level updated.\n";
}

void DietManagerApp::changeCalculator() {
    std::cout << "\n===== Change Target Calorie Calculator =====\n";
    std::cout << "1. Harris-Benedict Equation\n";
    std::cout << "2. Mifflin-St Jeor Equation\n";
    std::cout << "Select calculator: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    if (choice == 1) {
        profile.setCalculator(std::make_shared<HarrisBenedictCalculator>());
        saveData();
        std::cout << "Calculator changed to Harris-Benedict Equation.\n";
    } else if (choice == 2) {
        profile.setCalculator(std::make_shared<MifflinStJeorCalculator>());
        saveData();
        std::cout << "Calculator changed to Mifflin-St Jeor Equation.\n";
    } else {
        std::cout << "Invalid choice.\n";
    }
}

void DietManagerApp::selectDate() {
    std::cout << "\n===== Select Date =====\n";
    std::cout << "Current date: " << log.getCurrentDate() << "\n";
    std::cout << "1. Enter specific date\n";
    std::cout << "2. Select from existing dates\n";
    std::cout << "Enter choice: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    std::string newDate;
    
    if (choice == 1) {
        std::cout << "Enter date (YYYY-MM-DD): ";
        std::getline(std::cin, newDate);
        
        // Basic validation
        if (newDate.length() != 10 || newDate[4] != '-' || newDate[7] != '-') {
            std::cout << "Invalid date format. Use YYYY-MM-DD.\n";
            return;
        }
    } else if (choice == 2) {
        auto dates = log.getAllDates();
        if (dates.empty()) {
            std::cout << "No dates available in log.\n";
            return;
        }
        
        std::cout << "Available Dates:\n";
        for (size_t i = 0; i < dates.size(); ++i) {
            std::cout << i + 1 << ". " << dates[i] << "\n";
        }
        
        int dateIndex;
        std::cout << "Select date number: ";
        std::cin >> dateIndex;
        std::cin.ignore();
        
        if (dateIndex < 1 || dateIndex > static_cast<int>(dates.size())) {
            std::cout << "Invalid date number.\n";
            return;
        }
        
        newDate = dates[dateIndex - 1];
    } else {
        std::cout << "Invalid choice.\n";
        return;
    }
    
    auto command = std::make_shared<ChangeDateCommand>(log, newDate);
    undoManager.executeCommand(command);
    std::cout << "Date changed to " << newDate << ".\n";
}

void DietManagerApp::saveData() {
    std::cout << "\n===== Saving Data =====\n";
    
    bool foodDbSaved = foodDb->saveDatabase();
    bool logSaved = log.saveLog();
    bool profileSaved = profile.saveProfile();
    
    if (foodDbSaved && logSaved && profileSaved) {
        std::cout << "All data saved successfully.\n";
    } else {
        std::cout << "Some data could not be saved.\n";
        if (!foodDbSaved) std::cout << "- Food database not saved.\n";
        if (!logSaved) std::cout << "- Daily log not saved.\n";
        if (!profileSaved) std::cout << "- Profile not saved.\n";
    }
}