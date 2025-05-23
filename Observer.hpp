#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include <algorithm>

class Subject;
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(Subject* subject = nullptr) = 0;
};

class Subject {
private:
    std::vector<Observer*> observers;
public:
    virtual ~Subject() = default;
    
    void addObserver(Observer* observer);
    void removeObserver(Observer* observer);
    void notifyObservers();
};

#endif // OBSERVER_H