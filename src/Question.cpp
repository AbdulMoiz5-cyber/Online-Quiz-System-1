#include "Question.h"
#include <iostream>
#include <sstream>
using namespace std;

Question::Question() : questionText(""), correctOption(0) {}

Question::Question(const string& text, const vector<string>& opts, int correct)
    : questionText(text), options(opts), correctOption(correct) {}

void Question::display() const {
    cout << "\n\033[1;34m" << questionText << "\033[0m\n"; // Blue bold
    for (size_t i = 0; i < options.size(); ++i) {
        cout << i + 1 << ". " << options[i] << "\n";
    }
}
    

bool Question::isCorrect(int answer) const {
    return (answer == correctOption);
}

string Question::toString() const {
    ostringstream oss;
    oss << questionText << "\n";
    for (const auto& opt : options) {
        oss << opt << "\n";
    }
    oss << correctOption << "\n";
    return oss.str();
}

Question Question::fromString(const string& block) {
    Question q;
    istringstream iss(block);
    string line;

    if (!getline(iss, q.questionText)) return q;

    for (int i = 0; i < 4; ++i) {
        if (!getline(iss, line)) return q;
        q.options.push_back(line);
    }

    if (getline(iss, line)) {
        try {
            int correct = stoi(line);
            if (correct >= 1 && correct <= 4) {
                q.correctOption = 'A' + (correct - 1);  // 1 → A, 2 → B, etc.
            } else {
                cerr << "⚠️ Invalid correct option number: " << correct << endl;
                q.correctOption = 'A';
            }
        } catch (...) {
            cerr << "⚠️ Cannot convert correct option to number: " << line << endl;
            q.correctOption = 'A';
        }
    }

    return q;
}


Question::~Question() {}
