#ifndef GRAPH_H
#define GRAPH_H

#include"GraphNode.h"

class Graph {
public:
    Graph();

    ~Graph();

    int start, end;
    vector<GraphNode *> graphNodes;
protected:
private:
};

#endif // GRAPH_H


