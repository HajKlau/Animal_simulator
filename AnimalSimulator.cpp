#include "AnimalSimulator.h"
#include "Utility.h"
#include "Animal.h"
#include <iostream>
#include <string>
#include <memory>
#include <sqlite3.h>
#include <mutex>

AnimalSimulator::AnimalSimulator() : db(nullptr) {

        initializeDatabase();

        animalDescriptions = {
            {"mammals", "\033[1mMammals\033[0m are a diverse group of animals that share several key traits, such as hair or fur covering the body, the ability to give birth to live young and feed them with their mother's milk. Their advanced nervous systems allow for complex social behavior."},
            {"reptiles", "\033[1mReptiles\033[0m are cold-blooded animals with hard scales that inhabit a variety of environments around the world. They include snakes, lizards, crocodiles and turtles."},
            {"amphibians", "\033[1mAmphibians\033[0m are a unique group of cold-blooded animals that undergo metamorphosis from an aquatic larva with gills to an adult usually breathing atmospheric air. They are sensitive to changes in the environment, making them important indicators of ecosystem health."},
            {"birds", "\033[1mBirds\033[0m are warm-blooded animals characterized by the presence of feathers, a beak without teeth, and folding wings that in most cases allow flight."},
            {"fish", "\033[1mFish\033[0m are cold-blooded aquatic animals that have fins and breathe with gills. They inhabit a variety of aquatic environments, from freshwater to salty oceans."},
            {"arthropods", "\033[1mArthropods\033[0m are the most numerous group of animals on Earth, including insects, spiders, crustaceans and others. They are characterized by a segmented body, an external skeleton and paired, differentiated limbs."}
        };
        actions = {"eat", "play", "wash", "sleep", "stroke"};
    
    }

AnimalSimulator::~AnimalSimulator() {
        if (db) {
            sqlite3_close(db);
        }
    }

void AnimalSimulator::initializeDatabase() {
    int rc = sqlite3_open("game_state.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        concludeSession(false);
    }

    const char *sqlCreateTable = R"(
        CREATE TABLE IF NOT EXISTS GameStates (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Type TEXT NOT NULL,
            Name TEXT NOT NULL,
            color TEXT NOT NULL,
            Growth INTEGER,
            Happiness INTEGER,
            Appearance INTEGER,
            Strength INTEGER,
            Satisfaction INTEGER
        )
    )";

    char *errMsg = nullptr;
    rc = sqlite3_exec(db, sqlCreateTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error creating SQLite table: " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        exit(1);
    } else {
        cout << "Table created successfully" << endl;
    }
}

void AnimalSimulator::saveGameState() {
    lock_guard<mutex> lock(db_mutex);
    clearGameState();

    const char *sqlInsert = R"(
        INSERT INTO GameStates (Type, Name, color, Growth, Happiness, Appearance, Strength, Satisfaction)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt *stmt;
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Begin transaction error, game will not be saved: " << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    rc = sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement, game will not be saved: " << sqlite3_errmsg(db) << endl;
        return;
    }

    for (const auto& animal : trainedAnimals) {
        sqlite3_bind_text(stmt, 1, animal->type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, animal->name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, animal->color.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, animal->features["growth"]);
        sqlite3_bind_int(stmt, 5, animal->features["happiness"]);
        sqlite3_bind_int(stmt, 6, animal->features["appearance"]);
        sqlite3_bind_int(stmt, 7, animal->features["strength"]);
        sqlite3_bind_int(stmt, 8, animal->features["satisfaction"]);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "Failed to insert game state, game will not be saved: " << sqlite3_errmsg(db) << endl;
            sqlite3_clear_bindings(stmt);
            sqlite3_reset(stmt);
            int rollback_rc = sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, &errMsg);
            if (rollback_rc != SQLITE_OK) {
                cerr << "Failed to rollback transaction: " << errMsg << endl;
                sqlite3_free(errMsg);
            } else {
                cout << "Rollback executed successfully." << endl;
            }

            sqlite3_finalize(stmt);
            return;
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt); 

    rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
    }
}

void AnimalSimulator::clearGameState() {
    const char *sqlDelete = "DELETE FROM GameStates;";
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sqlDelete, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error clearing game state: " << errMsg << endl;
        sqlite3_free(errMsg);
        exit(1);
    }
}

void AnimalSimulator::loadGameState() {
    lock_guard<mutex> lock(db_mutex);
    trainedAnimals.clear();

    const char *sqlSelect = "SELECT Type, Name, color, Growth, Happiness, Appearance, Strength, Satisfaction FROM GameStates";
    sqlite3_stmt* stmt;
    bool hasAnimals = false;

    int rc = sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare select statement, save will not be loaded: " << sqlite3_errmsg(db) << endl;
        return;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string color = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int growth = sqlite3_column_int(stmt, 3);
        int happiness = sqlite3_column_int(stmt, 4);
        int appearance = sqlite3_column_int(stmt, 5);
        int strength = sqlite3_column_int(stmt, 6);
        int satisfaction = sqlite3_column_int(stmt, 7);
        hasAnimals = true;

        auto animal = make_shared<Animal>(type, name, color, growth, happiness, appearance, strength, satisfaction);
        trainedAnimals.push_back(animal);
        displayAnimalDetails(*animal);
    }
    sqlite3_finalize(stmt);

    if (hasAnimals) {
        if (!getUserConfirmation("Do you want to continue with saved animals?")) {
            clearGameState();
        }
    }
}

void AnimalSimulator::welcome() {
        cout << "\033[1m\033[35mWelcome to the Animal Simulator. Have fun!\033[0m" << endl;
         if (trainedAnimals.empty()) {
            loadGameState();
        }
    }

void AnimalSimulator::concludeSession(bool save) {
    if (save) {
        if (getUserConfirmation("\033[1mDo you want to save the game state before exiting?\033[0m ")) {
            saveGameState();
        }
    }
    cout << "\033[1m\033[35mThank you, that's all for today, see you.\033[0m" << endl;
    exit(0);
    }

Animal AnimalSimulator::createAnimal() {
        string type, name, color;
        cout << "\033[4mPlease select the type of animal from the list below and enter it or 'exit' to quit:\033[0m" << endl;
        for (const auto& pair : animalDescriptions) {
            cout << pair.first << endl;
        }
        cin >> type;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string lowerType = toLower(type);
        if (toLower(type) == "exit") {
            concludeSession(false);
        }
        auto description = animalDescriptions.find(lowerType);
        if (description != animalDescriptions.end()) {
            cout << "\033[32mGood choice!\033[0m " << description->second << endl;
        } else {
            cout << "\033[31m\033[1mThere is no such type of animal. Please try again.\033[0m" << endl;
            return createAnimal();
        }
        cout << "\033[4mNow you can enter its name\033[0m: ";
        while (true) {
            getline(cin, name);
            if (!isValidInput(name)) {
                cout << "\033[4mPlease enter a valid name:\033[0m ";
            } else {
                break;
            }
        }
        cout << "\033[4mcolor\033[0m: ";
        while (true) {
            getline(cin, color);
            if (!isValidInput(color)) {
                cout << "\033[4mPlease enter a valid color:\033[0m ";
            } else {
                break;
            }
        }
        return Animal(lowerType, name, color, 0, 0, 0, 0, 0);
    }
void AnimalSimulator::simulate(Animal &animal) {
    string action;

    do {
        cout << "\033[4mPlease select the action you want to perform from the list below:\033[0m" << endl;
        for (const auto& act : actions) {
            cout << act << endl;
        }
        cout << endl << "Or type 'exit' to exit the program." << endl << "Action: ";
        cin >> action;
        string lowerAction = toLower(action);

        if (lowerAction == "exit") {
            concludeSession(!trainedAnimals.empty()); 
        }

        animal.performAction(lowerAction);
        animal.displayFeatures();

        if (animal.hasReachedAdulthood()) {
            trainedAnimals.push_back(make_shared<Animal>(animal));
            cout << "\033[32m\033[1mCongratulations! Your pet has reached adulthood! \033[0m" << endl;

            if (trainedAnimals.size() == 3) {
                cout << "\033[1m\033[35mCongratulations, you already have 3 adult pets in your family!\033[0m" << endl;
                concludeSession(true);
            }

            if (!getUserConfirmation("\033[1mDo you want to develop a new pet?\033[0m")) {
                concludeSession(true);
            } else {
                welcome();
                animal = createAnimal();
                continue;
            }
        }

    } while (true);
}

void AnimalSimulator::displayAnimalDetails(const Animal &animal) {
    cout << "\033[1m" << endl << "Name: " << animal.name << endl << "Color: " << animal.color << endl <<"Type: " << animal.type << endl << endl << "Trained features: \033[0m" << endl;
    for (const auto& feature : animal.features) {
        cout << feature.first << ": " << feature.second << endl;
    }
}


