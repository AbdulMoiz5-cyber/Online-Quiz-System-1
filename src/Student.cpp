#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>

#include "Student.h"
#include "Quiz.h"
#include "FileManager.h"
#include "Result.h"
#include "Utils.h"
using namespace std;

void Student::takeQuiz() {
    string username;
    string password;

    
    auto quizList = FileManager::loadLines("data/quizzes.txt");
    
    if (quizList.empty()) {
        cout << "No quizzes available.\n";
        return;
    }
    

    cout << "\nAvailable quizzes:\n";
    for (size_t i = 0; i < quizList.size(); ++i) {
        cout << i + 1 << ". " << quizList[i] << "\n";
    }

    cout << "Enter quiz number to take: ";
    int choice = getValidatedInteger(1, quizList.size());

    string quizName = quizList[choice - 1];
    Quiz quiz(quizName);
    quiz.loadFromFile();

    if (quiz.getTotalQuestions() == 0) {
        cout << "Quiz is empty or failed to load.\n";
        return;
    }

    cout << "\nStarting quiz: " << quizName << "\n";

    //  Start timer
    auto start = chrono::steady_clock::now();

    int score = quiz.conductQuiz();
    int total = quiz.getTotalQuestions();

    //  End timer
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();

    bool isCheating = duration > 10;

    time_t now = time(0);
    string ts = ctime(&now);
    ts.pop_back(); // remove trailing newline

    Result result(getUsername(), quizName, score, total, ts, isCheating);
    FileManager::saveLine("data/results.txt", result.toString());

    cout << "\n✅ You scored " << score << " out of " << total << "\n";

    if (isCheating) {
        cout << "⚠️ Quiz completed too Late — result flagged for review.\n";
    }
}
Student::Student(const string& uname, const string& pass)
    : username(uname), password(pass) {}

    void Student::menu() {
    int choice;
    do {
        cout << "\n--- Student Menu ---\n";
        cout << "1. Take Quiz\n";
        cout << "2. View Results\n";
        cout << "3. Logout\n";
        cout << "Choose option: ";
        choice = getValidatedInteger(1, 3);

        switch (choice) {
            case 1: takeQuiz(); break;
            case 2: viewResults(); break;
            case 3: cout << "Logging out...\n"; break;
        }
    } while (choice != 3);
    
}
Student::~Student() {
}

void Student::viewResults() {
    cout << "Viewing results (feature coming soon).\n";
}

string Student::getUsername() const {
    return username;
}