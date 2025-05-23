# YADA - Yet Another Diet Assistant

This command-line system was created as a DASS (Design and Analysis of Software Systems) Assignment.

## Authors 
[@JoshitaDhanireddy](https://github.com/joshita-dhanireddy) & [@AmeyBangera](https://github.com/AmeyBangera)

**Note** : A Design.pdf is also present in this repository that explains our design rationale (with a class diagram and sequence diagrams) in the making of the program.

## How to run the program?
```bash 
mkdir build
cd build
cmake ..
make
mv foods.txt build
./diet_assistant
```

## Overview
YADA is a command-line diet management system that helps users track their food intake, calculate daily calorie goals, and manage their diet profile. 

## Features
The main menu has the following features
1. `Manage Foods` is for viewing, creating, adding and deleting foods from the foods file.
    
    - `View All Foods` for viewing the foods present in food.txt
    - `Search Foods` for searching foods using keywords (either applying all the keywords or any keyword). 
    - `Add Basic Food` for adding a new food to foods.txt
    - `Create Composite Food` for combining basic foods to create a composite food and add to foods.txt
---
2. `Log Foods` to either add or delete foods to or from the daily log.

    - `Add Food to Log` to add a food item to dailylog.txt (again, search methods or view all can be u to pick which food item to add).
    - `Delete Food from Log` to remove any item in the log (it will present a list of items currently in log and you can pick an item to remove from this list).
---
3. `Manage Profile` to view and update profile information.

    - `Edit Basic Information`to edit gender, age, height(things you don't change often).
    - `Update Current Weight` to update weight.
    - `Update Activity Level` to update activity level.
    - `Change Target Calorie Calculator` to pick between the two calculators currently available
---
4. `Select Date` to select which date all your updates and summaries correspond to (you can manually enter a date or pick from available dates).
---
5. `Undo Last Action` to undo any saved action 
---
6. `Save Data` to save an action
---
7. `Exit` to exit the program
