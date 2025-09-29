#include "FileManager.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

void FileManager::saveLine(const string& filename, const string& line) {
    ofstream out(filename, ios::app);
    if (out) {
        out << line << "\n";
    }
    out.close();
}

vector<string> FileManager::loadLines(const string& filename) {
    ifstream in(filename);
    vector<string> lines;
    string line;

    while (getline(in, line)) {
        lines.push_back(line);
    }

    in.close();
    return lines;
}

void FileManager::overwriteFile(const string& filename, const vector<string>& lines) {
    ofstream out(filename);
    for (const auto& line : lines) {
        out << line << "\n";
    }
    out.close();
}


