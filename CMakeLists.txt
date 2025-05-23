cmake_minimum_required(VERSION 3.10)
project(DietAssistant)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(diet_assistant
    main.cpp
    Observer.cpp
    Food.cpp
    FoodDatabase.cpp
    DailyLog.cpp
    Calculator.cpp
    DietProfile.cpp
    Command.cpp
    FoodTracker.cpp
    DietManagerApp.cpp
)

target_include_directories(diet_assistant PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})