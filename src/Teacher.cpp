#include "Teacher.h"
#include "Quiz.h"
#include "FileManager.h"
#include "Result.h"
#include "Utils.h"

#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
using namespace std;

Teacher::Teacher() {}

Teacher::Teacher(const string& uname, const string& pwd)
    : User(uname, pwd) {}

void Teacher::menu() {
    int choice;
    do {
        cout << "\n===== TEACHER DASHBOARD =====\n";
        cout << "1. Create Quiz\n";
        cout << "2. Edit Quiz\n";
        cout << "3. Delete Quiz\n";
        cout << "4. View Analytics\n";
        cout << "5. View Leaderboard\n";
        cout << "6. Logout\n";
        cout << "=============================\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: createQuiz(); break;
            case 2: editQuiz(); break;
            case 3: deleteQuiz(); break;
            case 4: viewAnalytics(); break;
            case 5: viewLeaderboard(); break;
            case 6: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice.\n"; break;
        }
    } while (choice != 6);
}

void Teacher::createQuiz() {
    string quizName;
    cout << "Enter quiz title: ";
    cin.ignore();
    getline(cin, quizName);

    Quiz quiz(quizName);

    int numQuestions;
    cout << "How many questions? ";
    cin >> numQuestions;

    cin.ignore();
    for (int i = 0; i < numQuestions; ++i) {
        string qtext;
        vector<string> opts(4);
        int correct;

        cout << "Enter question " << i + 1 << ": ";
        getline(cin, qtext);

        for (int j = 0; j < 4; ++j) {
            cout << "Option " << j + 1 << ": ";
            getline(cin, opts[j]);
        }

        cout << "Correct option number (1-4): ";
        cin >> correct;
        cin.ignore();

        Question q(qtext, opts, correct);
        quiz.addQuestion(q);
    }

    quiz.saveToFile();
    cout << "Quiz saved successfully!\n";

    FileManager::saveLine("data/quizzes.txt", quizName);

}


void Teacher::editQuiz() {
    auto quizList = FileManager::loadLines("data/quizzes.txt");
    if (quizList.empty()) {
        cout << "No quizzes available.\n";
        return;
    }

    cout << "\nAvailable quizzes:\n";
    for (size_t i = 0; i < quizList.size(); ++i) {
        cout << i + 1 << ". " << quizList[i] << "\n";
    }

    cout << "Enter quiz number to edit: ";
    int choice = getValidatedInteger(1, quizList.size());

    string quizName = quizList[choice - 1];
    Quiz quiz(quizName);
    quiz.loadFromFile();

    if (quiz.getTotalQuestions() == 0) {
        cout << "This quiz has no questions.\n";
        return;
    }

    quiz.displayAllQuestions();

    cout << "Enter question number to edit: ";
    int qnum = getValidatedInteger(1, quiz.getTotalQuestions());

    cin.ignore();
    string newText;
    vector<string> newOptions(4);
    int newCorrect;

    cout << "New question text: ";
    getline(cin, newText);
    for (int i = 0; i < 4; ++i) {
        cout << "New option " << i + 1 << ": ";
        getline(cin, newOptions[i]);
    }
    cout << "Enter correct option number (1-4): ";
    newCorrect = getValidatedInteger(1, 4);

    Question newQ(newText, newOptions, newCorrect);
    quiz.updateQuestion(qnum - 1, newQ);
    quiz.saveToFile();

    cout << "Question updated successfully!\n";
}


void Teacher::deleteQuiz() {
    auto quizList = FileManager::loadLines("data/quizzes.txt");
    if (quizList.empty()) {
        cout << "No quizzes available.\n";
        return;
    }

    cout << "\nAvailable quizzes:\n";
    for (size_t i = 0; i < quizList.size(); ++i) {
        cout << i + 1 << ". " << quizList[i] << "\n";
    }

    cout << "Enter quiz number to delete: ";
    int choice = getValidatedInteger(1, quizList.size());

    string quizName = quizList[choice - 1];
    string filePath = "data/" + quizName + ".txt";

    // Delete quiz file
    if (remove(filePath.c_str()) != 0) {
        cout << "Error deleting quiz file.\n";
        return;
    }

    // Remove from quizzes.txt
    quizList.erase(quizList.begin() + (choice - 1));
    FileManager::overwriteFile("data/quizzes.txt", quizList);

    cout << "Quiz '" << quizName << "' deleted successfully.\n";
}

void Teacher::viewAnalytics() {
    auto lines = FileManager::loadLines("data/results.txt");

    map<string, vector<int>> quizScores;
    for (const auto& line : lines) {
        Result r = Result::fromString(line);
        quizScores[r.getQuizTitle()].push_back(r.getScore());
    }

    cout << "\n--- Quiz Performance Analytics ---\n";
    for (const auto& [title, scores] : quizScores) {
        int sum = 0;
        for (int s : scores) sum += s;
        double avg = scores.empty() ? 0 : static_cast<double>(sum) / scores.size();
        cout << "Quiz: " << title << " | Attempts: " << scores.size() 
                  << " | Avg Score: " << avg << "\n";
    }

    if (quizScores.empty()) {
        cout << "No quiz results available.\n";
        return;
    }
    
}
void Teacher::viewLeaderboard() {
    auto quizList = FileManager::loadLines("data/quizzes.txt");
    if (quizList.empty()) {
        cout << "No quizzes available.\n";
        return;
    }

    cout << "\nAvailable quizzes:\n";
    for (size_t i = 0; i < quizList.size(); ++i) {
        cout << i + 1 << ". " << quizList[i] << "\n";
    }

    cout << "Enter quiz number to view leaderboard: ";
    int choice = getValidatedInteger(1, quizList.size());

    string selectedQuiz = quizList[choice - 1];

    auto lines = FileManager::loadLines("data/results.txt");
    vector<Result> quizResults;

    for (const auto& line : lines) {
        Result r = Result::fromString(line);
        if (r.getQuizTitle() == selectedQuiz) {
            quizResults.push_back(r);
        }
    }

    if (quizResults.empty()) {
        cout << "No results found for this quiz.\n";
        return;
    }

    // Sort by score descending
    sort(quizResults.begin(), quizResults.end(), [](const Result& a, const Result& b) {
        return a.getScore() > b.getScore();
    });

    cout << "\n🏆 Leaderboard for: " << selectedQuiz << "\n";
    cout << "-----------------------------\n";

    int limit = min(3, (int)quizResults.size());
    for (int i = 0; i < limit; ++i) {
        cout << i + 1 << ". " << quizResults[i].getUsername()
                  << " | Score: " << quizResults[i].getScore() << "\n";
    }
}

Teacher::~Teacher() {}
