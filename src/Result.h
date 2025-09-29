#ifndef RESULT_H
#define RESULT_H
#include <string>
using namespace std;

class Result {
private:
    string studentUsername;
    string quizTitle;
    int score;


public:
    int total;
    bool flagged;
    string timestamp;
    Result(const string& uname, const string& qtitle, int s, int t, const string& ts, bool cheatFlag);
 
    
    bool isFlagged() const;

    Result();
    Result(const string& uname, const string& qtitle, int s, int t, const string& ts);

    void display() const;
    string toString() const;
    static Result fromString(const string& line);

    string getUsername() const;
    string getQuizTitle() const;
    int getScore() const;
    ~Result();
};


#endif
