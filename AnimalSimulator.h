#pragma once
#include "Animal.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <sqlite3.h>

using namespace std;

class AnimalSimulator {

private: 
    unordered_map<string, string> animalDescriptions;
    vector<string> actions;
    vector<shared_ptr<Animal>> trainedAnimals;
    sqlite3* db;
    mutex db_mutex;
public:
    AnimalSimulator();
    ~AnimalSimulator();

    void initializeDatabase();
    void saveGameState();
    void clearGameState();
    void loadGameState();
    void welcome();
    void concludeSession(bool save);
    Animal createAnimal();
    void simulate(Animal &animal);
    void displayAnimalDetails(const Animal &animal);

};