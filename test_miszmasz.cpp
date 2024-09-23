#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <memory>
#include <sqlite3.h>
#include <mutex>


using namespace std;


string toLower(const string& str) {
    string lowerStr;
    lowerStr.reserve(str.size());
    for (char c : str) {
        lowerStr += tolower(c);
    }
    return lowerStr;
}

bool getUserConfirmation(const string &question) {
    string response;
    while(true) {
        cout << question << (" \033[1m(yes/no)\033[0m");
        cin >> response;
        string lowerResponse = toLower(response);

        if (lowerResponse == "yes") {
            return true;
        } else if (lowerResponse == "no"){
            return false;
        } else {
            cout << "\033[1m\033[31mPlease enter 'yes' or 'no'.\033[0m" << endl;
        }
    }
}

class Animal {
public:
    string type;
    string name;
    string colour;
    unordered_map<string, int> features;

    Animal(string type, string name, string colour, int growth, int happiness, int appearance, int strength, int satisfaction) : type(type), name(name), colour(colour) {
        features["growth"] = growth;
        features["happiness"] = happiness;
        features["appearance"] = appearance;
        features["strength"] = strength;
        features["satisfaction"] = satisfaction;
    }

    void performAction(const string& action) {
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

    void displayFeatures() {
        cout << "\033[32m\033[1m" << name << " is a " << colour << " " << type << " with the following features:\033[0m " << endl;
        for (const auto& feature : features) {
            cout << feature.first << ": " << feature.second << endl;
        }
    }

    bool hasReachedAdulthood() const {
        for (const auto& feature : features) {
            if (feature.second < 5) {
                return false;
            }
        } 
        return true;
    }
};

class AnimalSimulator {
private: 
    unordered_map<string, string> animalDescriptions;
    vector<string> actions;
    vector<shared_ptr<Animal>> trainedAnimals;
    sqlite3* db;
    mutex db_mutex;

public:
    AnimalSimulator() : db(nullptr) {
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

    ~AnimalSimulator() {
        if (db) {
            sqlite3_close(db);
        }
    }

    void initializeDatabase() {
        int rc = sqlite3_open("/home/klahaj/nauka_programowania/Animal_simulator/game_state.db", &db);
        if (rc) {
            cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            exit(1);
        }

        const char* sqlCreateTable = R"(
            CREATE TABLE IF NOT EXISTS GameStates (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                Type TEXT NOT NULL,
                Name TEXT NOT NULL,
                Colour TEXT NOT NULL,
                Growth INTEGER,
                Happiness INTEGER,
                Appearance INTEGER,
                Strength INTEGER,
                Satisfaction INTEGER
            )
        )";

        char* errMsg = nullptr;
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

    void saveGameState() {
        lock_guard<mutex> lock(db_mutex);

        const char* sqlInsert = R"(
            INSERT INTO GameStates (Type, Name, Colour, Growth, Happiness, Appearance, Strength, Satisfaction)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?);
        )";

        sqlite3_stmt* stmt;
        char* errmsg = 0;
        int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errmsg);
        if (rc != SQLITE_OK) {
            cerr << "Begin transaction error: " << errmsg << endl;
            sqlite3_free(errmsg);
            return;
        }

        rc = sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        for (const auto& animal : trainedAnimals) {
            sqlite3_bind_text(stmt, 1, animal->type.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, animal->name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, animal->colour.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 4, animal->features["growth"]);
            sqlite3_bind_int(stmt, 5, animal->features["happiness"]);
            sqlite3_bind_int(stmt, 6, animal->features["appearance"]);
            sqlite3_bind_int(stmt, 7, animal->features["strength"]);
            sqlite3_bind_int(stmt, 8, animal->features["satisfaction"]);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                cerr << "Failed to insert game state: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_clear_bindings(stmt);
                sqlite3_reset(stmt);
                sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &errmsg);
                cerr << "Rollback executed due to an error." << std::endl;
                sqlite3_free(errmsg);
                sqlite3_finalize(stmt);
                return;
            }

            // Resetowanie zapytania dla kolejnego użycia
            sqlite3_clear_bindings(stmt);
            sqlite3_reset(stmt);
        }

        sqlite3_finalize(stmt); // Zwolnienie zasobów zapytania

        // Zatwierdzenie transakcji
        rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, &errmsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Commit error: " << errmsg << std::endl;
            sqlite3_free(errmsg);
        }
    }

    void loadLastGameState() {
        db_mutex.lock();
        const char* sqlSelectLastRow = "SELECT Type, Name, Colour, Growth, Happiness, Appearance, Strength, Satisfaction FROM GameStates ORDER BY ID DESC LIMIT 1;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sqlSelectLastRow, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                string colour = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                int growth = sqlite3_column_int(stmt, 3);
                int happiness = sqlite3_column_int(stmt, 4);
                int appearance = sqlite3_column_int(stmt, 5);
                int strength = sqlite3_column_int(stmt, 6);
                int satisfaction = sqlite3_column_int(stmt, 7);

                Animal animal(type, name, colour, growth, happiness, appearance, strength, satisfaction);
                displayAnimalDetails(animal);

                if (getUserConfirmation("Do you want to continue from this point?")) {
                    trainedAnimals.push_back(make_shared<Animal>(animal));
                    db_mutex.unlock();
                    simulate(animal);
                }
            }
            sqlite3_finalize(stmt);
        } else {
            cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db) << endl;
        }

        db_mutex.unlock();
    }

    void welcome() {
        cout << "\033[1m\033[35mWelcome to the Animal Simulator. Have fun!\033[0m" << endl;
    }

    Animal createAnimal() {
        string type, name, colour;
        cout << "\033[4mPlease select the type of animal from the list below and enter it or 'exit' to quit:\033[0m" << endl;
        for (const auto& pair : animalDescriptions) {
            cout << pair.first << endl;
        }
        cin >> type;
        string lowerType = toLower(type);
        if (toLower(type) == "exit") {
            exit(0);
        }
        auto description = animalDescriptions.find(lowerType);
        if (description != animalDescriptions.end()) {
            cout << "\033[32mGood choice!\033[0m " << description->second << endl;
        } else {
            cout << "\033[31m\033[1mThere is no such type of animal. Please try again.\033[0m" << endl;
            return createAnimal();
        }
        cout << "\033[4mNow you can enter its name\033[0m: ";
        cin >> name;
        cout << "\033[4mColour\033[0m: ";
        cin >> colour;
        return Animal(lowerType, name, colour, 0, 0, 0, 0, 0);
    }
 
    void simulate(Animal& animal) {
        string action;
    
        do {
            cout << "\033[4mPlease select the action you want to perform from the list below:\033[0m" << endl;
            for (const auto& act : actions) {
                cout << act << endl;
            }
            cin >> action;
            string lowerAction = toLower(action);
            animal.performAction(lowerAction);
            animal.displayFeatures();

            if (animal.hasReachedAdulthood()) {
                trainedAnimals.push_back(make_shared<Animal>(animal));
                cout << "\033[32m\033[1mCongratulations! Your pet has reached adulthood! \033[0m" << endl;

                if (trainedAnimals.size() == 2) {
                    cout << "\033[1m\033[35mCongratulations, you already have 3 adult pets in your family!\033[0m" << endl;
                    for (const auto& pet : trainedAnimals) {
                        displayAnimalDetails(*pet);
                    }
                    exit(0);
                }

                if (getUserConfirmation("\033[1mDo you want to develop a new pet?\033[0m")) {
                    animal = Animal ("", "", "", 0, 0, 0, 0, 0);
                    welcome();
                    animal = createAnimal();
                    continue;
                } else {
                    cout << "\033[1m\033[35mThank you, that's it for today.\033[0m" << endl;
                    
                }
            }

        if (!getUserConfirmation("\033[1mDo you want to continue taking care of the animal\033[0m")) {
    string userResponse;
    cout << "Do you want to save the game state before exiting? (yes/no): ";
    cin >> userResponse;
    if (toLower(userResponse) == "yes") {
        saveGameState(); // Zapis stanu gry
    }
    cout << "\033[1m\033[35mThank you, that's all for today, see you.\033[0m" << endl;
    exit(0);
}


        } while (true);
    }


    void displayAnimalDetails(const Animal &animal) {
        cout << "\033[1m" << endl << "Name: " << animal.name << endl << "Color: " << animal.colour << endl <<"Type: " << animal.type << endl << endl << "Trained features: \033[0m" << endl;
        for (const auto& feature : animal.features) {
            cout << feature.first << ": " << feature.second << endl;
        }
    }

};

int main() {
    while (true) {
        sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        AnimalSimulator simulator;
        simulator.welcome();

        // Sprawdź, czy istnieje zapisany stan gry i zapytaj użytkownika, czy chce go wczytać
        string response;
        do {
            cout << "Do you want to load the last saved game state? (yes/no)";
            cin >> response;
            response = toLower(response);
            if (response == "yes") {
                simulator.loadLastGameState();
                break;
            } else if (response == "no") {
                break;
            } else {
                cout << "Please enter 'yes' or 'no'." << endl;
            }
        } while (true);

        // Jeśli nie wczytano stanu gry lub użytkownik wybrał "no", kontynuuj od utworzenia nowego zwierzaka
        if (response != "yes") {
            Animal animal = simulator.createAnimal();
            simulator.simulate(animal);
        }

        cout << "Do you want to play again? (yes/no)";
        cin >> response;
        if (toLower(response) != "yes") {
            break;
        }
    }

    return 0;
}




