#include "Utility.h"
#include <iostream>
#include <cctype>

using namespace std;


string toLower(const string &str) {
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

bool isValidInput (const string &input) {
    if (input.length() > 20) {
        cout << "\033[1m\033[31mError: Input is too long. Maximum 20 characters allowed.\033[0m" << endl;
        return false;
    }

    for (char c : input) {
        if (!isalnum(c)) {
            cout << "\033[1m\033[31mError: Invalid characters detected. Only letters and digits are allowed.\033[0m" << endl;
            return false;
        }
    }

    return true;

}
