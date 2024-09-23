#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <memory>


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

    Animal(string type, string name, string colour) : type(type), name(name), colour(colour) {
        features = {{"growth", 0}, {"happiness", 0}, {"appearance", 0}, {"strength", 0}, {"satisfaction", 0}};
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

public:
    AnimalSimulator() {
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
        return Animal(lowerType, name, colour);
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
                    animal = Animal ("", "", "");
                    welcome();
                    animal = createAnimal();
                    continue;
                } else {
                    cout << "\033[1m\033[35mThank you, that's it for today.\033[0m" << endl;
                    exit(0);
                }
            }

            if(!getUserConfirmation("\033[1mDo you want to continue taking care of the animal\033[0m")) {
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
    AnimalSimulator simulator;
    simulator.welcome();
    Animal animal = simulator.createAnimal();
    simulator.simulate(animal);

    return 0;
}