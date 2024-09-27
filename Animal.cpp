#include "Animal.h"
#include "Utility.h"
#include <iostream>
#include <cctype>

Animal::Animal(string type, string name, string color, int growth, int happiness, int appearance, int strength, int satisfaction) : type(type), name(name), color(color)
{
    traits["growth"] = growth;
    traits["happiness"] = happiness;
    traits["appearance"] = appearance;
    traits["strength"] = strength;
    traits["satisfaction"] = satisfaction;
}

void Animal::performAction(const string &action)
{
    if (action == "eat")
    {
        traits["growth"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << traits["growth"] << " growth" << "\033[0m" << endl;
    }
    else if (action == "play")
    {
        traits["happiness"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << traits["happiness"] << " happiness" << "\033[0m" << endl;
    }
    else if (action == "wash")
    {
        traits["appearance"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << traits["appearance"] << " appearance" << "\033[0m" << endl;
    }
    else if (action == "sleep")
    {
        traits["strength"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << traits["strength"] << " strength" << "\033[0m" << endl;
    }
    else if (action == "pet")
    {
        traits["satisfaction"] += 1;
        cout << "\033[1mPerformed: " << action << ", " << name << " now has " << traits["satisfaction"] << " satisfaction" << "\033[0m" << endl;
    }
    else
    {
        cout << "\033[31m\033[1mUnknown action: " << "'" << action << "'. No changes made.\033[0m" << endl;
    }
}

void Animal::displayTraits() const
{
    cout << "\033[32m\033[1m" << name << " is a " << color << " " << type << " with the following traits:\033[0m " << endl;
    for (const auto &trait : traits)
    {
        cout << trait.first << ": " << trait.second << endl;
    }
}

bool Animal::hasReachedAdulthood() const
{
    for (const auto &trait : traits)
    {
        if (trait.second < 1)
        {
            return false;
        }
    }
    return true;
}