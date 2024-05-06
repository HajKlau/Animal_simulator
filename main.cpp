#include "AnimalSimulator.h"
#include <sqlite3.h>

int main() {
    sqlite3_config(SQLITE_CONFIG_SERIALIZED);
    AnimalSimulator simulator;
    simulator.welcome();
    Animal animal = simulator.createAnimal();
    simulator.simulate(animal);

    return 0;
}