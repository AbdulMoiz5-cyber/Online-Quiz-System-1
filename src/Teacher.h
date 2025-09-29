#ifndef TEACHER_H
#define TEACHER_H

#include "User.h"

class Teacher : public User {
public:
    Teacher();
    Teacher(const string& uname, const string& pwd);

    void menu() override;
    void createQuiz();
    void editQuiz();
    void deleteQuiz();
    void viewAnalytics();
    void viewLeaderboard();


    ~Teacher();
};

#endif
