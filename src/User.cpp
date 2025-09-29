#include "User.h"
using namespace std;

User::User() : username(""), password("") {}

User::User(const string& uname, const string& pwd) 
    : username(uname), password(pwd) {}

string User::getUsername() const {
    return username;
}

User::~User() {}
