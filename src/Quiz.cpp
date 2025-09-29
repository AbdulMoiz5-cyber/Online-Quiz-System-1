#include "Quiz.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

Quiz::Quiz() : title("Untitled Quiz") {}

Quiz::Quiz(const string& quizTitle) : title(quizTitle) {}

void Quiz::addQuestion(const Question& q) {
    questions.push_back(q);
}

void Quiz::removeQuestion(int index) {
    if (index >= 0 && index < (int)questions.size()) {
        questions.erase(questions.begin() + index);
    } else {
        cout << "Invalid index.\n";
    }
}

void Quiz::displayAllQuestions() const {
    for (size_t i = 0; i < questions.size(); ++i) {
        cout << "\nQuestion " << i + 1 << ":\n";
        questions[i].display();
    }
}

int Quiz::conductQuiz() {
    int score = 0;
    for (const auto& q : questions) {
        q.display();
        cout << "Your answer (1-4): ";
        int ans;
        cin >> ans;
        if (q.isCorrect(ans)) {
            cout << "Correct!\n";
            score++;
        } else {
            cout << "Incorrect!\n";
        }
        cout << "-------------------\n";
    }
    return score;
}

string Quiz::getTitle() const {
    return title;
}

void Quiz::saveToFile() const {
    ofstream out("data/" + title + ".txt");
    for (const auto& q : questions) {
        out << q.toString() << "---\n";  // '---' marks the end of a question block
    }
    out.close();
}

void Quiz::loadFromFile() {
    ifstream in("data/" + title + ".txt");
    if (!in.is_open()) {
        cerr << "Error: Quiz file '" << title << "' not found.\n";
        return;
    }

    string line, block;
    questions.clear();

    while (getline(in, line)) {
        if (line == "---") {
            questions.push_back(Question::fromString(block));
            block.clear();
        } else {
            block += line + "\n";
        }
    }

    if (questions.empty()) {
        cerr << "Warning: This quiz has no questions.\n";
    }

    in.close();
}

int Quiz::getTotalQuestions() const 
{ 
    return questions.size(); 
}

void Quiz::updateQuestion(int index, const Question& newQ) {
    if (index >= 0 && index < (int)questions.size()) {
        questions[index] = newQ;
    }
}


Quiz::~Quiz() {}
