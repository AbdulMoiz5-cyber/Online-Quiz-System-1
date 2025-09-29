#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {
protected:
    string username;
    string password;

public:
    User();
    User(const string& uname, const string& pwd);

    virtual void menu() = 0;
    string getUsername() const;

    virtual ~User();
};

#endif
