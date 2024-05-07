#include "Animal.h"
#include "Utility.h"
#include <iostream>
#include <cctype>

Animal::Animal(string type, string name, string color, int growth, int happiness, int appearance, int strength, int satisfaction) : type(type), name(name), color(color) {
    features["growth"] = growth;
    features["happiness"] = happiness;
    features["appearance"] = appearance;
    features["strength"] = strength;
    features["satisfaction"] = satisfaction;
}

void Animal::performAction(const string &action) {
    if (action == "eat") {
        features["growth"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << features["growth"] << " growth" << "\033[0m" << endl;
    } else if (action == "play") {
        features["happiness"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << features["happiness"] << " happiness" << "\033[0m" <<  endl;
    } else if (action == "wash") {
        features["appearance"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << features["appearance"] << " appearance" << "\033[0m" << endl;
    } else if (action == "sleep") {
        features["strength"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << features["strength"] << " strength" << "\033[0m" << endl;
    } else if (action == "stroke") {
        features["satisfaction"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << features["satisfaction"] << " satisfaction" << "\033[0m" << endl;
    } else {
        cout << "\033[31m\033[1mUnknown action: " << "'" << action << "'. No changes made.\033[0m" << endl;
    }   
}

void Animal::displayFeatures() const {
    cout << "\033[32m\033[1m" << name << " is a " << color << " " << type << " with the following features:\033[0m " << endl;
    for (const auto &feature : features) {
        cout << feature.first << ": " << feature.second << endl;
    }
}

bool Animal::hasReachedAdulthood() const {
    for (const auto &feature : features) {
        if (feature.second < 1) {
            return false;
        }
    } 
    return true;
}