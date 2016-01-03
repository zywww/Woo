#ifndef INPUTANDOUTPUT_H
#define INPUTANDOUTPUT_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "GraphNode.h"
#include "Graph.h"
#include "LineLink.h"
#include "NfaToDfa.h"

using namespace std;

struct basicNfa {
    int start, end;
    GraphNode *startNode;
    GraphNode *endNode;
};

class RexHandle {
public:
    RexHandle();

    ~RexHandle();

    void run();

    basicNfa make_basic_nfa(char x, int start, int end);

    void repeatOperation();

    void choiceOperation();

    void connectOperation();

    bool isOperator(char x);

    void outputGraph();

    int count;
    string Rex;
    Graph nfa;
    NfaToDfa ntd;
    basicNfa *obj_e_nfa;
    stack<basicNfa> s_nfa;
    stack<char> s_symbol;

    void setRextext(string &s);

    vector<pair<int, int> > compareString(string &s);

    void outputPair(vector<pair<int, int> > a);

protected:
private:
};

#endif // INPUTANDOUTPUT_H


