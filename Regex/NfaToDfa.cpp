#include "NfaToDfa.h"

struct seT {
    int ID;
    char line;
    vector<int> state;
};

NfaToDfa::NfaToDfa() {
    //ctor
    this->count = 0;
    this->st_count = 0;
}

NfaToDfa::~NfaToDfa() {
    //dtor
}

void NfaToDfa::translate(Graph *nfa) {
    translate_enfa2nfa(nfa);
    translate_nfa2dfa(nfa);
}

void out_vector(vector<struct seT> *a) {
    cout << "DStates: ";
    for (unsigned long i = 0; i < a->size(); i++) {
        cout << " " << a->at(i).ID;
    }
    cout << endl << endl;
}

void out_set_vector(vector<struct seT> *a) {
    cout << "set_vector: " << endl;
    for (unsigned long i = 0; i < a->size(); i++) {
        cout << " ID:" << a->at(i).ID;
        cout << " line:" << a->at(i).line << endl;
        for (int j = 0; j < a->at(i).state.size(); j++) {
            cout << " nodeID:" << a->at(i).state[j];
        }
        cout << endl;
    }
}

bool isElemexist(int x, vector<int> *a) {
    for (unsigned long i = 0; i < a->size(); i++) {
        if (a->at(i) == x) return true;
    }
    return false;
}

void e_closure(GraphNode *node, vector<int> *closureNodes) {
    for (int i = 0; i < node->LinkNodes.size(); i++) {
        LineLink *linenode = node->LinkNodes[i];
        if (linenode->line == '@') {
            if (!isElemexist(linenode->node->ID, closureNodes)) {
                closureNodes->push_back(linenode->node->ID);
                e_closure(linenode->node, closureNodes);
            }
        }
    }
}


void deal_e_closure(Graph *nfa, vector<int> *endnodes, vector<int> *a) {
    for (unsigned long i = 0; i < a->size(); i++) {
        GraphNode *this_node = nfa->graphNodes[a->at(i)];
        //cout << "node:" << a->at(i) << endl;
        vector<int> closureNodes;
        e_closure(this_node, &closureNodes);
        for (int j = 0; j < closureNodes.size(); j++) {
            //cout << "closure:" << closureNodes[j] << endl;
            if (closureNodes[j] == nfa->end) {
                if (!isElemexist(this_node->ID, endnodes))
                    endnodes->push_back(this_node->ID);
            }
            vector<LineLink *> &linelink = nfa->graphNodes[closureNodes[j]]->LinkNodes;//��ӱ�
            for (int q = 0; q < linelink.size(); q++) {
                if (linelink[q]->line != '@')
                    this_node->LinkNodes.push_back(linelink[q]);
            }
        }
    }
}

void delete_non_able_nodes(Graph *nfa, vector<int> *gnodes) {
    for (int i = 0; i < nfa->graphNodes.size(); i++) {
        if (isElemexist(i, gnodes)) {
            vector<LineLink *>::iterator iter = nfa->graphNodes[i]->LinkNodes.begin();
            for (; iter != nfa->graphNodes[i]->LinkNodes.end();) {
                if ((*iter)->line == '@') {
                    iter = nfa->graphNodes[i]->LinkNodes.erase(iter);
                    //iter = nfa->graphNodes[i]->LinkNodes.begin();
                } else iter++;
            }
        }
        else nfa->graphNodes[i]->LinkNodes.clear();
    }
}

void NfaToDfa::translate_enfa2nfa(Graph *nfa) {
    this->gnodes.push_back(nfa->start);
    this->endnodes.push_back(nfa->end);
    for (int i = 0; i < nfa->graphNodes.size(); i++) {
        GraphNode *this_node;
        LineLink *link_line;
        this_node = nfa->graphNodes[i];
        for (int j = 0; j < this_node->LinkNodes.size(); j++) {
            link_line = this_node->LinkNodes[j];
            if (link_line->line != '@')
                this->gnodes.push_back(link_line->node->ID);
            //if(link_line->node->ID == nfa->end)
            //this->endnodes.push_back(this_node->ID);
        }
    }
    deal_e_closure(nfa, &this->endnodes, &this->gnodes);
    delete_non_able_nodes(nfa, &this->gnodes);
}


vector<struct seT> *NfaToDfa::findInNfa(struct seT undealState, Graph *nfa, vector<struct seT> *set_Dstates) {
    vector<struct seT> *final_T = new vector<struct seT>;
    for (int i = 0; i < undealState.state.size(); i++) {
        GraphNode *node = nfa->graphNodes[undealState.state[i]];
        for (int j = 0; j < node->LinkNodes.size(); j++) {
            struct seT x;
            char c = node->LinkNodes[j]->line;
            bool flag = true;
            if (final_T->size() == 0) {
                x.line = node->LinkNodes[j]->line;
                x.ID = this->st_count++;
                x.state.push_back(node->LinkNodes[j]->node->ID);
                final_T->push_back(x);
            }
            else {
                for (int q = 0; q < final_T->size(); q++) {
                    if ((*final_T)[q].line == c) {
                        (*final_T)[q].state.push_back(node->LinkNodes[j]->node->ID);
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    x.line = node->LinkNodes[j]->line;
                    x.ID = this->st_count++;
                    x.state.push_back(node->LinkNodes[j]->node->ID);
                    final_T->push_back(x);
                }
            }
        }
    }
    for (int i = 0; i < final_T->size(); i++) {
        for (int j = 0; j < set_Dstates->size(); j++) {
            sort((*final_T)[i].state.begin(), (*final_T)[i].state.end());
            sort((*set_Dstates)[j].state.begin(), (*set_Dstates)[j].state.end());
            if ((*final_T)[i].state == (*set_Dstates)[j].state)
                (*final_T)[i].ID = (*set_Dstates)[j].ID;
        }
    }
    return final_T;

}


void NfaToDfa::pushTheDifferNewState(GraphNode *thisNode,
                                     vector<struct seT> *set_T,
                                     vector<struct seT> *set_Dstates,
                                     queue<struct seT> *L,
                                     Graph *dfa) {

    vector<struct seT> temp;
    for (int i = 0; i < set_T->size(); i++) {
        bool flag = false;
        GraphNode *linknode = this->dfa.graphNodes[(*set_T)[i].ID];
        linknode->ID = (*set_T)[i].ID;
        for (int j = 0; j < (*set_T)[i].state.size(); j++) linknode->set_newState.push_back((*set_T)[i].state[j]);
        LineLink *tline = new LineLink;
        tline->line = (*set_T)[i].line;
        tline->node = linknode;
        thisNode->LinkNodes.push_back(tline);
        for (int j = 0; j < set_Dstates->size(); j++) {
            if ((*set_T)[i].ID == (*set_Dstates)[j].ID)
                flag = true;
        }
        if (!flag) {
            L->push((*set_T)[i]);
            temp.push_back((*set_T)[i]);
        }
    }
    for (int j = 0; j < temp.size(); j++) {
        set_Dstates->push_back(temp[j]);
    }
}

void NfaToDfa::translate_nfa2dfa(Graph *nfa) {
    //out_vector(&this->gnodes);
    //out_vector(&this->endnodes);
    queue<struct seT> *L = new queue<struct seT>;
    vector<struct seT> *set_T;
    vector<struct seT> *set_Dstates = new vector<struct seT>;

    vector<int> s(1, nfa->graphNodes[nfa->start]->ID);
    struct seT st;
    st.ID = this->st_count++;
    st.state = s;
    L->push(st);
    set_Dstates->push_back(st);

    //cout << "-----PROGRAM OF NO_E_NFA TO DFA" << endl;
    while (!L->empty()) {
        struct seT undealState = L->front();
        L->pop();
        //cout << "undealState.ID:" << undealState.ID << endl;
        GraphNode *node = this->dfa.graphNodes[undealState.ID];
        set_T = findInNfa(undealState, nfa, set_Dstates);
        //out_set_vector(set_T);
        pushTheDifferNewState(node, set_T, set_Dstates, L, &this->dfa);
        //out_vector(set_Dstates);
    }

    for (unsigned long i = 0; i < set_Dstates->size(); i++) {
        for (unsigned long j = 0; j < set_Dstates->at(i).state.size(); j++)
            if (isElemexist(set_Dstates->at(i).state[j], &this->endnodes)) {
                this->dfa_endnodes.push_back(set_Dstates->at(i).ID);
                break;
            }
    }
}


void NfaToDfa::outGraph() {
    cout << "-----DFA" << endl;
    GraphNode *t_node;
    for (int i = 0; i < this->dfa.graphNodes.size(); i++) {
        t_node = this->dfa.graphNodes[i];
        cout << "ID:" << t_node->ID;
        unsigned long num_link = t_node->LinkNodes.size();
        if (num_link != 0) {
            for (; num_link > 0; num_link--)
                cout << "  LinkNode_ID:" << t_node->LinkNodes[num_link - 1]->node->ID << " char:" <<
                t_node->LinkNodes[num_link - 1]->line;
        }
        cout << endl;
    }
    cout << "-----START_ID:0  END_ID:";
    for (int i = 0; i < this->dfa_endnodes.size(); i++) {
        cout << dfa_endnodes[i] << ' ';
    }
    cout << endl;
}


vector<pair<int, int> > NfaToDfa::stringTest(string &s) {
    vector<pair<int, int> > vp;
    GraphNode *node = dfa.graphNodes[0];
    int start = 0, end = 0;
    for (int i = 0; i < s.length(); i++) {
        bool flag = true;
        int j;
        vector<LineLink *> linkline = node->LinkNodes;
        for (j = 0; j < linkline.size(); j++) {
            if (linkline[j]->line == s[i]) {
                flag = false;
                node = linkline[j]->node;
                end++;
                break;
            }
        }
        if (flag) {
            bool flag2 = false;
            for (j = 0; j < dfa_endnodes.size(); j++) {
                if (node->ID == dfa_endnodes[j] && node->ID != dfa.graphNodes[0]->ID) {
                    vp.push_back(pair<int, int>(start, end - 1));
                    start = i;
                    end = i;
                    i--;
                    node = dfa.graphNodes[0];
                    flag2 = true;
                }
            }
            if (!flag2) {
                if (start == i && end == i) {
                    vp.push_back(pair<int, int>(-start - 100, -end - 100));
                    start = i + 1;
                    end = i + 1;
                }
                else {
                    vp.push_back(pair<int, int>(-start - 100, -end + 1 - 100));
                    start = i;
                    end = i;
                    i--;
                }
                node = dfa.graphNodes[0];
            }
        }
        if (i == s.length() - 1 && !flag) {
            for (j = 0; j < dfa_endnodes.size(); j++) {
                if (node->ID == dfa_endnodes[j]) {
                    vp.push_back(pair<int, int>(start, end - 1));
                }
            }
        }
    }
    return vp;
}

