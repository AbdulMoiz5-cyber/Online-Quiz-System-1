#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <algorithm>
using namespace std;

int getValidatedInteger(int min, int max);
string toLower(const string& str);
string xorEncryptDecrypt(const string& text, char key = 'K');


#endif
