#include "src/Student.h"
#include "src/Teacher.h"
#include "src/UserManager.h"
#include "src/Utils.h"
#include <iostream>
#include <cstdlib>
using namespace std;

void ensureDataFolderExists() {
    system("mkdir -p data");
}

int main() {
    ensureDataFolderExists();
    cout << "==============================\n";
    cout << "     Welcome to Quiz System   \n";
    cout << "==============================\n";

    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Exit\n";
        cout << "Choose option: ";
        int mode = getValidatedInteger(1, 3);

        if (mode == 3) {
            cout << "Thank you for using the Quiz System. Goodbye!\n";
            break;
        }

        cout << "\nSelect Role:\n";
        cout << "1. Student\n";
        cout << "2. Teacher\n";
        cout << "Choose: ";
        int roleChoice = getValidatedInteger(1, 2);
        string role = (roleChoice == 1) ? "student" : "teacher";

        string username, password;
        cout << "Username: ";
        getline(cin >> ws, username);
        cout << "Password: ";
        getline(cin, password);

        if (mode == 2) {
            UserManager::registerUser(role, username, password);
            cout << "✅ Registration successful.\n";
        }

        if (UserManager::login(role, username, password)) {
            cout << "✅ Login successful.\n";

            if (role == "student") {
                Student s(username, password);
                s.menu();
            } else {
                Teacher t(username, password);
                t.menu();
            }
        } else {
            cout << "❌ Login failed. Please try again.\n";
        }
    }

    return 0;
}
