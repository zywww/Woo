#include "Graph.h"

Graph::Graph() {
    //initial graph
    for (int i = 0; i < 400; i++) {
        GraphNode *node = new GraphNode;
        node->ID = i;
        graphNodes.push_back(node);
    }
}

Graph::~Graph() {
    //dtor
}

