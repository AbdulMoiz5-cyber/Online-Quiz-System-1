#ifndef QUESTION_H
#define QUESTION_H

#include <string>
#include <vector>
using namespace std;

class Question {
public:
    string questionText;
    vector<string> options;
    int correctOption;
    Question();
    Question(const string& text, const vector<string>& opts, int correct);

    void display() const;
    bool isCorrect(int answer) const;

    string toString() const; // for saving to file
    static Question fromString(const string& line); // for loading

    ~Question();
};

#endif
