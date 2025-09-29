#ifndef QUIZ_H
#define QUIZ_H

#include <vector>
#include "Question.h"
using namespace std;

class Quiz {
private:
    string title;
    vector<Question> questions;

public:
    Quiz();
    Quiz(const string& quizTitle);

    void addQuestion(const Question& q);
    void removeQuestion(int index);
    void displayAllQuestions() const;

    int conductQuiz(); // returns score

    string getTitle() const;
    void loadFromFile();
    void saveToFile() const;
    int getTotalQuestions() const;
    void updateQuestion(int index, const Question& newQ);
    Question getQuestion(int index) const {
    if (index >= 0 && index < questions.size()) {
        return questions[index];
    }
    return Question(); // return empty if out of bounds
    }



    ~Quiz();
};

#endif
