#ifndef LINELINK_H
#define LINELINK_H

#include<iostream>
#include <string>

using namespace std;

class GraphNode;

class LineLink {
public:
    LineLink();

    ~LineLink();

    GraphNode *node;
    char line;
    bool backLine;
protected:
private:
};

#endif // LINELINK_H


