#include "UserManager.h"
#include "FileManager.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

bool UserManager::login(const string& role, const string& username, const string& password) {
    auto lines = FileManager::loadLines("data/users.txt");
    string encryptedInput = xorEncryptDecrypt(password);

    for (const auto& line : lines) {
        istringstream iss(line);
        string r, u, p;
        getline(iss, r, '|');
        getline(iss, u, '|');
        getline(iss, p, '|');

        if (toLower(r) == toLower(role) &&
        toLower(u) == toLower(username) &&
        p == encryptedInput)
        return true;
    }
    return false;
}

bool UserManager::registerUser(const string& role, const string& username, const string& password) {
    auto lines = FileManager::loadLines("data/users.txt");

    for (const auto& line : lines) {
        istringstream iss(line);
        string r, u, p;
        getline(iss, r, '|');
        getline(iss, u, '|');

        if (toLower(r) == toLower(role) && toLower(u) == toLower(username)) {
            cout << "⚠️ Username already exists under this role. Please try a different one.\n";
            return false;
        }
    }

    string encryptedPass = xorEncryptDecrypt(password);
    string entry = role + "|" + username + "|" + encryptedPass;
    FileManager::saveLine("data/users.txt", entry);

    return true;
}

