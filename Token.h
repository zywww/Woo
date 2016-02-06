//
// Created by TanGreen on 15/12/31.
//

#ifndef MYCOMPILER_TOKEN_H
#define MYCOMPILER_TOKEN_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <list>
#include "Regex/RexHandle.h"

using namespace std;


class Token {
public:
    virtual ~Token() { }

    Token();

    int start;
    int end;
    int token;
    int lineIndex;
    string value;
};

class Scan {
public:
    Scan();

    void getToken();

    int getTokenId(string s);

    virtual ~Scan() { }

    map<int, string> T_map;
    list<Token *> list_token;

    string srcfile;
    string outfile;
};

#endif //MYCOMPILER_TOKEN_H
