#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
using namespace std;

class FileManager {
public:
    static void saveLine(const string& filename, const string& line);
    static vector<string> loadLines(const string& filename);
    static void overwriteFile(const string& filename, const vector<string>& lines);
};

#endif
