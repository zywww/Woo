#ifndef NFATODFA_H
#define NFATODFA_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include "Graph.h"
#include "LineLink.h"

using namespace std;


class NfaToDfa {
public:
    NfaToDfa();

    ~NfaToDfa();

    vector<int> gnodes;
    vector<int> endnodes;
    vector<int> dfa_endnodes;
    Graph dfa;
    int count;
    int st_count;

    void outGraph();

    void translate(Graph *nfa);

    void translate_enfa2nfa(Graph *nfa);

    void translate_nfa2dfa(Graph *nfa);

    vector<struct seT> *findInNfa(struct seT, Graph *nfa, vector<struct seT> *set_Dstates);

    void pushTheDifferNewState(GraphNode *thisNode,
                               vector<struct seT> *set_T,
                               vector<struct seT> *set_Dstates,
                               queue<struct seT> *L,
                               Graph *dfa);

    vector<pair<int, int> > stringTest(string &s);

protected:
private:
};

#endif // NFATODFA_H


