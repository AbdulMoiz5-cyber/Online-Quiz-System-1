#include "Result.h"
#include <iostream>
#include <sstream>
using namespace std;

Result::Result() : studentUsername(""), quizTitle(""), score(0), total(0) {}

Result::Result(const string& uname, const string& qtitle, int s, int t, const string& ts, bool cheatFlag)
    : studentUsername(uname), quizTitle(qtitle), score(s), total(t), timestamp(ts), flagged(cheatFlag) {}


void Result::display() const {
    cout << "Quiz: " << quizTitle << "\n"
              << "Score: " << score << "/" << total << "\n"
              << "Date: " << timestamp << "\n";
    if (flagged) {
        cout << "⚠️ Suspected Cheating (answered too quickly)\n";
    }
}


string Result::toString() const {
    return studentUsername + "|" + quizTitle + "|" + to_string(score) + "|" +
           to_string(total) + "|" + timestamp + "|" + (flagged ? "1" : "0");
}


Result Result::fromString(const string& line) {
    istringstream iss(line);
    string uname, qtitle, scoreStr, totalStr, ts, flagStr;

    getline(iss, uname, '|');
    getline(iss, qtitle, '|');
    getline(iss, scoreStr, '|');
    getline(iss, totalStr, '|');
    getline(iss, ts, '|');
    getline(iss, flagStr);

    int score = stoi(scoreStr);
    int total = stoi(totalStr);
    bool flagged = (flagStr == "1");

    return Result(uname, qtitle, score, total, ts, flagged);
}

string Result::getUsername() const 
{ 
    return studentUsername; 
}
string Result::getQuizTitle() const { return quizTitle; }

int Result::getScore() const { return score; }



Result::~Result() {}
