#ifndef STUDENT_H
#define STUDENT_H

#include "User.h"
using namespace std;
class Student : public User {
private:
    string username;
    string password;

public:
    Student(const string& uname, const string& pass);
    ~Student(); 
    string getUsername() const;
    void menu();
    void takeQuiz();
    void viewResults();
};

#endif
