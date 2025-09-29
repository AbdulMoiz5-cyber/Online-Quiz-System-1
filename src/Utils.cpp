#include <iostream>
#include "Utils.h"
using namespace std;

int getValidatedInteger(int min, int max) {
    int input;
    while (true) {
        cin >> input;
        if (cin.fail() || input < min || input > max) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Enter a number between " << min << " and " << max << ": ";
        } else {
            cin.ignore(1000, '\n');
            return input;
        }
    }
}

string toLower(const string& str) {
    string lower = str;
    transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c){ return tolower(c); });
    return lower;
}


string xorEncryptDecrypt(const string& text, char key) {
    string result = text;
    for (char& c : result) {
        c ^= key;
    }
    return result;
}
