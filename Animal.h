#pragma once
#include <string>
#include <unordered_map>

using namespace std;

class Animal
{
public:
    std::string type;
    std::string name;
    std::string color;
    std::unordered_map<std::string, int> traits;

    Animal(std::string type, std::string name, std::string color, int growth, int happiness, int appearance, int strength, int satisfaction);

    void performAction(const std::string &action);
    void displayTraits() const;
    bool hasReachedAdulthood() const;
};
