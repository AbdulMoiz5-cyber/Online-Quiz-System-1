#include "httplib.h"
#include "src/UserManager.h"
#include "src/FileManager.h"
#include "src/Quiz.h"
#include "src/Result.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

string currentUser = "";

string load_file(const string& path) {
    ifstream in(path);
    string content((istreambuf_iterator<char>(in)),
    istreambuf_iterator<char>());
    return content;
}

int main() {
    httplib::Server svr;
    svr.Get("/style.css", [](const httplib::Request&, httplib::Response& res) {
    res.set_content(load_file("web/style.css"), "text/css");
    });


    // Serve login page
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(load_file("web/index.html"), "text/html");
    });

    // Login handler
    svr.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        string username = req.get_param_value("username");
        string password = req.get_param_value("password");
        string role = req.get_param_value("role");

    if (UserManager::login(role, username, password)) {
        currentUser = username;
        if (role == "student")
            res.set_redirect("/student");
        else if (role == "teacher")
            res.set_redirect("/teacher");
    }

    });

    // Register handler
    svr.Post("/register", [](const httplib::Request& req, httplib::Response& res) {
        string username = req.get_param_value("username");
        string password = req.get_param_value("password");
        string role = req.get_param_value("role");

        if (UserManager::registerUser(role, username, password)) {
            res.set_content("<h2>✅ Registered successfully. Please login.</h2>", "text/html");
        } else {
            res.set_content("<h2>❌ Registration failed. User may already exist.</h2>", "text/html");
        }
    });

    // Student dashboard
    svr.Get("/student", [](const httplib::Request&, httplib::Response& res) {
        string page = load_file("web/student_template.html");

        size_t pos = page.find("{{username}}");
        if (pos != string::npos) {
            page.replace(pos, 11, currentUser);
        }

        res.set_content(page, "text/html");
    });

    // Quiz selection page
    svr.Get("/student/take", [](const httplib::Request&, httplib::Response& res) {
        string page = load_file("web/quiz_select_template.html");
        string quizzesHTML;

        auto quizList = FileManager::loadLines("data/quizzes.txt");

        if (quizList.empty()) {
            quizzesHTML = "<p>No quizzes available.</p>";
        } else {
            quizzesHTML += "<form action='/student/start' method='get'>";
            quizzesHTML += "<select name='quiz'>";
            for (const auto& q : quizList) {
                quizzesHTML += "<option value='" + q + "'>" + q + "</option>";
            }
            quizzesHTML += "</select><br><br>";
            quizzesHTML += "<button type='submit'>Start Quiz</button>";
            quizzesHTML += "</form>";
        }

        size_t pos = page.find("{{quiz_list}}");
        if (pos != string::npos) {
            page.replace(pos, 13, quizzesHTML);
        }

        res.set_content(page, "text/html");
    });

    // First question handler
// Helper to replace all occurrences in a string
auto replace_all = [](string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
};

// Route to load first quiz question
svr.Get("/student/start", [replace_all](const httplib::Request& req, httplib::Response& res) {
    try {
        string quizName = req.get_param_value("quiz");

        // 🚫 Prevent duplicate attempt
        auto results = FileManager::loadLines("data/results.txt");
        for (const string& line : results) {
            if (line.find(currentUser + "," + quizName) == 0 || 
                line.find(currentUser + "|" + quizName) == 0) {
                res.set_content("<h2>❌ You have already attempted this quiz.</h2><a href='/student'>⬅ Back</a>", "text/html");
                return;
            }
        }

        // 📘 Load quiz
        Quiz quiz(quizName);
        quiz.loadFromFile();

        if (quiz.getTotalQuestions() == 0) {
            res.set_content("<h2>❌ This quiz has no questions or failed to load.</h2>", "text/html");
            return;
        }

        // 🧠 Get first question
        Question q = quiz.getQuestion(0);
        if (q.options.size() != 4) {
            res.set_content("<h2>❌ Invalid question format. Each question must have exactly 4 options.</h2>", "text/html");
            return;
        }

        string page = load_file("web/quiz_question_template.html");

        // ✅ Generate options HTML
        string optionsHTML;
        char option = 'A';
        for (const auto& opt : q.options) {
            optionsHTML += "<label><input type='radio' name='answer' value='" + string(1, option) + "' required> " + opt + "</label><br>";
            option++;
        }

        // ✅ Replace ALL placeholders in the page
        replace_all(page, "{{question_text}}", q.questionText);
        replace_all(page, "{{options_html}}", optionsHTML);
        replace_all(page, "{{quiz}}", quizName);
        replace_all(page, "{{index}}", "0");
        replace_all(page, "{{score}}", "0");
        replace_all(page, "{{total}}", to_string(quiz.getTotalQuestions()));

        res.set_content(page, "text/html");
    }
    catch (const exception& e) {
        res.set_content("<h2>❌ Internal server error: " + string(e.what()) + "</h2>", "text/html");
    }
});


// Answer and go to next question
svr.Post("/student/next", [](const httplib::Request& req, httplib::Response& res) {
    // Helper to replace all placeholders
    auto replace_all = [](string& str, const string& from, const string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    };

    try {
        string quizName = req.get_param_value("quiz");
        int index = stoi(req.get_param_value("index"));
        int score = stoi(req.get_param_value("score"));
        string answer = req.get_param_value("answer");

        Quiz quiz(quizName);
        quiz.loadFromFile();

        Question q = quiz.getQuestion(index);
        if (q.correctOption == answer[0]) score++;

        index++;
        if (index >= quiz.getTotalQuestions()) {
            string result = "<h2>✅ Quiz Completed!</h2>";
            result += "<p>Score: " + to_string(score) + " / " + to_string(quiz.getTotalQuestions()) + "</p>";

            string timestamp = "2024-05-31"; // Replace with real timestamp if needed
            Result r(currentUser, quizName, score, quiz.getTotalQuestions(), timestamp, false);
            FileManager::saveLine("data/results.txt", r.toString());

            res.set_content(result, "text/html");
            return;
        }

        // Load next question
        Question nextQ = quiz.getQuestion(index);
        string page = load_file("web/quiz_question_template.html");
        string optionsHTML;
        char option = 'A';
        for (const auto& opt : nextQ.options) {
            optionsHTML += "<label><input type='radio' name='answer' value='" + string(1, option) + "' required> " + opt + "</label><br>";
            option++;
        }

        // ✅ Replace all placeholders in the HTML
        replace_all(page, "{{question_text}}", nextQ.questionText);
        replace_all(page, "{{options_html}}", optionsHTML);
        replace_all(page, "{{quiz}}", quizName);
        replace_all(page, "{{index}}", to_string(index));
        replace_all(page, "{{score}}", to_string(score));
        replace_all(page, "{{total}}", to_string(quiz.getTotalQuestions()));

        res.set_content(page, "text/html");
    }
    catch (const exception& e) {
        res.set_content("<h2>❌ Internal server error: " + string(e.what()) + "</h2>", "text/html");
    }
});


    svr.Post("/student/disqualified", [](const httplib::Request& req, httplib::Response& res) {
    string quiz = req.get_param_value("quiz");
    string score = req.get_param_value("score");
    string total = req.get_param_value("total");

    string ts = "2024-05-31"; // Replace with current timestamp if needed
    Result r(currentUser, quiz, stoi(score), stoi(total), ts, true); // flagged = true
    FileManager::saveLine("data/results.txt", r.toString());

    res.set_content("<h2>❌ You have been disqualified for switching tabs.</h2><p><a href='/student'>⬅ Back to Dashboard</a></p>", "text/html");
    });


    svr.Get("/student/results", [](const httplib::Request&, httplib::Response& res) {
    string page = load_file("web/student_results_template.html");
    string rows;

    auto lines = FileManager::loadLines("data/results.txt");

    for (const auto& line : lines) {
        Result r = Result::fromString(line);
        if (r.getUsername() == currentUser) {
            rows += "<tr><td>" + r.getQuizTitle() + "</td><td>" +
                    to_string(r.getScore()) + "</td><td>" +
                    to_string(r.total) + "</td><td>" +
                    r.timestamp + "</td><td>" + (r.flagged ? "Yes" : "No") + "</td></tr>";
        }
    }

    size_t pos = page.find("{{result_rows}}");
    if (pos != string::npos) {
        page.replace(pos, 15, rows.empty() ? "<tr><td colspan='4'>No results yet.</td></tr>" : rows);
    }

    res.set_content(page, "text/html");
    });

    svr.Get("/teacher", [](const httplib::Request&, httplib::Response& res) {
    string page = load_file("web/teacher_template.html");

    size_t pos = page.find("{{username}}");
    if (pos != string::npos) {
        page.replace(pos, 11, currentUser);
    }

    res.set_content(page, "text/html");
    });

    svr.Get("/teacher/create", [](const httplib::Request&, httplib::Response& res) {
        string page = load_file("web/create_quiz_form.html");
        res.set_content(page, "text/html");
    });


    svr.Get("/teacher/create_form", [](const httplib::Request& req, httplib::Response& res) {
        string title = req.get_param_value("title");
        int count = stoi(req.get_param_value("count"));

        string form = "<h2>✏ Enter Questions for Quiz: <em>" + title + "</em></h2>";
        form += "<form action='/teacher/save_quiz' method='post'>";
        form += "<input type='hidden' name='title' value='" + title + "'>";
        form += "<input type='hidden' name='count' value='" + to_string(count) + "'>";

        for (int i = 0; i < count; ++i) {
            form += "<hr><strong>Question " + to_string(i + 1) + "</strong><br>";
            form += "Text: <input type='text' name='q" + to_string(i) + "' required><br>";
            form += "A: <input type='text' name='a" + to_string(i) + "' required><br>";
            form += "B: <input type='text' name='b" + to_string(i) + "' required><br>";
            form += "C: <input type='text' name='c" + to_string(i) + "' required><br>";
            form += "D: <input type='text' name='d" + to_string(i) + "' required><br>";
            form += "Correct (1-4): <input type='number' name='correct" + to_string(i) + "' min='1' max='4' required><br>";
        }

        form += "<br><button type='submit'>✅ Save Quiz</button></form>";
        form += "<p><a href='/teacher'>⬅ Back to Dashboard</a></p>";

        res.set_content(form, "text/html");
    });



    svr.Post("/teacher/save_quiz", [](const httplib::Request& req, httplib::Response& res) {
    string title = req.get_param_value("title");
    int count = stoi(req.get_param_value("count"));

    ofstream out("data/" + title + ".txt");
    if (!out.is_open()) {
        res.set_content("<h2>❌ Failed to save quiz file.</h2>", "text/html");
        return;
    }

    for (int i = 0; i < count; ++i) {
        out << req.get_param_value("q" + to_string(i)) << "\n";
        out << req.get_param_value("a" + to_string(i)) << "\n";
        out << req.get_param_value("b" + to_string(i)) << "\n";
        out << req.get_param_value("c" + to_string(i)) << "\n";
        out << req.get_param_value("d" + to_string(i)) << "\n";
        out << req.get_param_value("correct" + to_string(i)) << "\n";
        out << "---\n";
    }

    out.close();

    // ✅ Update quizzes.txt if needed
    auto all = FileManager::loadLines("data/quizzes.txt");
    if (find(all.begin(), all.end(), title) == all.end()) {
        FileManager::saveLine("data/quizzes.txt", title);
    }

    res.set_content("<h2>✅ Quiz saved successfully!</h2><p><a href='/teacher'>⬅ Back to Dashboard</a></p>", "text/html");
    });

    svr.Get("/teacher/delete", [](const httplib::Request&, httplib::Response& res) {
    string page = load_file("web/delete_quiz_template.html");
    string quizOptions;

    auto quizList = FileManager::loadLines("data/quizzes.txt");
    for (const auto& q : quizList) {
        quizOptions += "<option value='" + q + "'>" + q + "</option>";
    }

    size_t pos = page.find("{{quiz_list}}");
    if (pos != string::npos) {
        page.replace(pos, 13, quizOptions.empty() ? "<option disabled>No quizzes available</option>" : quizOptions);
    }

    res.set_content(page, "text/html");
    });

    svr.Get("/teacher/delete_confirm", [](const httplib::Request& req, httplib::Response& res) {
    string quiz = req.get_param_value("quiz");

    string path = "data/" + quiz + ".txt";
    if (remove(path.c_str()) == 0) {
        // Remove from quizzes.txt
        auto quizzes = FileManager::loadLines("data/quizzes.txt");
        quizzes.erase(remove(quizzes.begin(), quizzes.end(), quiz), quizzes.end());
        FileManager::overwriteFile("data/quizzes.txt", quizzes);

        res.set_content("<h2>✅ Quiz '" + quiz + "' deleted successfully.</h2><p><a href='/teacher'>⬅ Back</a></p>", "text/html");
    } else {
        res.set_content("<h2>❌ Failed to delete quiz '" + quiz + "'. File not found.</h2><p><a href='/teacher'>⬅ Back</a></p>", "text/html");
    }
    });

    svr.Get("/teacher/results", [](const httplib::Request&, httplib::Response& res) {
    string page = load_file("web/teacher_results_template.html");
    string tableRows;

    auto lines = FileManager::loadLines("data/results.txt");

    for (const string& line : lines) {
        Result r = Result::fromString(line);

        tableRows += "<tr>";
        tableRows += "<td>" + r.getUsername() + "</td>";
        tableRows += "<td>" + r.getQuizTitle() + "</td>";
        tableRows += "<td>" + to_string(r.getScore()) + "</td>";
        tableRows += "<td>" + to_string(r.total) + "</td>";
        tableRows += "<td>" + r.timestamp + "</td>";
        tableRows += "<td>" + string(r.flagged ? "Yes" : "No") + "</td>";
        tableRows += "</tr>";
    }

    size_t pos = page.find("{{results_table}}");
    if (pos != string::npos) {
        page.replace(pos, 17, tableRows.empty() ? "<tr><td colspan='6'>No results yet.</td></tr>" : tableRows);
    }

    res.set_content(page, "text/html");
    });




    cout << "🚀 Server running at http://localhost:8080\n";
    svr.listen("localhost", 8080);
}