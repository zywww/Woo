#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <vector>
#include <iostream>

using namespace std;

class LineLink;

class GraphNode {
public:
    GraphNode();

    ~GraphNode();

    int ID;
    //CPoin position;
    int radius;
    vector<LineLink *> LinkNodes;
    vector<int> set_newState;
protected:
private:
};

#endif // GRAPHNODE_H


