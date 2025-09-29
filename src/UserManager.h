#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <string>
using namespace std;

class UserManager {
public:
    static bool login(const string& role, const string& username, const string& password);
    static bool registerUser(const string& role, const string& username, const string& password);
};

#endif
