/*
'&' : connect
'*' : repeat
'|' : choice
*/

#include "RexHandle.h"


RexHandle::RexHandle() {
    //ctor
    this->count = 0;

}

RexHandle::~RexHandle() {
    //dtor
}

void RexHandle::outputGraph() {
    cout << "-----NFA" << endl;
    GraphNode *t_node;
    for (int i = 0; i < nfa.graphNodes.size(); i++) {
        t_node = nfa.graphNodes[i];
        cout << "ID:" << t_node->ID;
        int num_link = t_node->LinkNodes.size();
        if (num_link != 0) {
            for (; num_link > 0; num_link--)
                cout << "  LinkNode_ID:" << t_node->LinkNodes[num_link - 1]->node->ID << " char:" <<
                t_node->LinkNodes[num_link - 1]->line;
        }
        cout << endl;
    }
}


bool RexHandle::isOperator(char x) {
    switch (x) {
        case '(':
            return 1;
        case ')':
            return 1;
        case '|':
            return 1;
        case '*':
            return 1;
        case '&':
            return 1;
    }
    return 0;
}

void RexHandle::repeatOperation() {
    if (s_nfa.empty())
        cout << "Error!" << endl;
    else {
        basicNfa thisnfa;
        thisnfa = s_nfa.top();
        s_nfa.pop();
        LineLink *tempLink = new LineLink;
        tempLink->node = thisnfa.startNode;
        tempLink->line = '@';
        thisnfa.endNode->LinkNodes.push_back(tempLink); //add the back line
        basicNfa midnfa = make_basic_nfa('@', count, count + 1);// add tow new node
        count += 2;
        tempLink = new LineLink;
        tempLink->node = thisnfa.startNode;
        tempLink->line = '@';
        midnfa.startNode->LinkNodes.push_back(tempLink);//add to midnfa
        tempLink = new LineLink;
        tempLink->node = midnfa.endNode;
        tempLink->line = '@';
        thisnfa.endNode->LinkNodes.push_back(tempLink);//add to midnfa

        s_nfa.push(midnfa);
    }
}

void RexHandle::choiceOperation() {
    if (!s_nfa.empty()) {
        basicNfa midnfa = make_basic_nfa('@', count, count + 1);// add tow new node
        count += 2;
        midnfa.startNode->LinkNodes.clear();
        basicNfa taliNode = s_nfa.top(); // pop tow node one time ,maybe  occur error
        s_nfa.pop();
        basicNfa headNode = s_nfa.top();
        s_nfa.pop();
        LineLink *tempLink = new LineLink;
        tempLink->node = midnfa.endNode;
        tempLink->line = '@';
        headNode.endNode->LinkNodes.push_back(tempLink);

        tempLink = new LineLink;
        tempLink->node = midnfa.endNode;
        tempLink->line = '@';
        taliNode.endNode->LinkNodes.push_back(tempLink);

        tempLink = new LineLink;
        tempLink->node = headNode.startNode;
        tempLink->line = '@';
        midnfa.startNode->LinkNodes.push_back(tempLink);

        tempLink = new LineLink;
        tempLink->node = taliNode.startNode;
        tempLink->line = '@';
        midnfa.startNode->LinkNodes.push_back(tempLink);

        s_nfa.push(midnfa);
    }
}

void RexHandle::connectOperation() {
    if (!s_nfa.empty()) {
        basicNfa con_nfa;
        basicNfa taliNode = s_nfa.top(); // pop tow node one time ,maybe  occur error
        s_nfa.pop();
        basicNfa headNode = s_nfa.top();
        s_nfa.pop();
        LineLink *tempLink = new LineLink;
        tempLink->node = taliNode.startNode;
        tempLink->line = '@';
        headNode.endNode->LinkNodes.push_back(tempLink);
        con_nfa.start = headNode.start;
        con_nfa.end = taliNode.end;
        con_nfa.startNode = headNode.startNode;
        con_nfa.endNode = taliNode.endNode;
        s_nfa.push(con_nfa);
    }
}

basicNfa RexHandle::make_basic_nfa(char x, int start, int end) {
    basicNfa b_nfa;
    b_nfa.start = start;
    b_nfa.end = end;
    b_nfa.startNode = nfa.graphNodes[start];
    b_nfa.endNode = nfa.graphNodes[end];
    LineLink *endLine = new LineLink();
    endLine->backLine = false;
    endLine->line = x;
    endLine->node = b_nfa.endNode;
    b_nfa.startNode->LinkNodes.push_back(endLine);
    return b_nfa;
}

void RexHandle::run() {
    char ch;
    for (int i = 0; i < Rex.length(); i++) {
        switch (Rex[i]) {
            case '(':
                if (i != 0 && ((Rex[i - 1] >= 'A' && Rex[i - 1] <= 'z') || (Rex[i - 1] >= '0' && Rex[i - 1] <= '9')))
                    s_symbol.push('&');
                else if (i > 1 && Rex[i - 2] == '\\') {
                    s_symbol.push('&');
                }
                s_symbol.push(Rex[i]);
                break;

            case ')':
                ch = s_symbol.top();
                while (ch != '(') {
                    switch (ch) {
                        case '|':
                            s_symbol.pop();
                            choiceOperation();
                            break;
                        case '&':
                            s_symbol.pop();
                            connectOperation();
                            break;
                    }
                    ch = s_symbol.top();
                }
                s_symbol.pop();
                break;

            case '|':
                if (!s_symbol.empty()) {
                    ch = s_symbol.top();
                    while (ch != '(' && (!s_symbol.empty())) {
                        ch = s_symbol.top();
                        switch (ch) {
                            case '&':
                                s_symbol.pop();
                                connectOperation();
                                break;
                            case '|':
                                s_symbol.pop();
                                choiceOperation();
                                break;
                        }
                    }
                }
                s_symbol.push(Rex[i]);
                break;

            case '*':
                if (i + 1 < Rex.length() && (!isOperator(Rex[i + 1]) || Rex[i + 1] == '('))
                    s_symbol.push('&');
                repeatOperation();
                break;
            case '\\':
                if (i != 0 && !isOperator(Rex[i - 1])) {
                    s_symbol.push('&');
                    basicNfa bNfa = make_basic_nfa(Rex[i + 1], count, count + 1);
                    count += 2;
                    s_nfa.push(bNfa);
                    i++;
                    break;
                }
                else {
                    basicNfa bNfa = make_basic_nfa(Rex[i + 1], count, count + 1);
                    count += 2;
                    s_nfa.push(bNfa);
                    i++;
                    break;
                }
            default:
                if ((Rex[i] >= 'A' && Rex[i] <= 'z') || (Rex[i] >= '0' && Rex[i] <= '9')) {
                    if (i != 0 &&
                        ((Rex[i - 1] >= 'A' && Rex[i - 1] <= 'z') || (Rex[i - 1] >= '0' && Rex[i - 1] <= '9'))) {
                        s_symbol.push('&');
                        basicNfa bNfa = make_basic_nfa(Rex[i], count, count + 1);
                        count += 2;
                        s_nfa.push(bNfa);
                    } else if (i > 1 && Rex[i - 2] == '\\') {
                        s_symbol.push('&');
                        basicNfa bNfa = make_basic_nfa(Rex[i], count, count + 1);
                        count += 2;
                        s_nfa.push(bNfa);
                    }
                    else {
                        basicNfa bNfa = make_basic_nfa(Rex[i], count, count + 1);
                        count += 2;
                        s_nfa.push(bNfa);
                    }
                }
                break;
        }
    }
    while (!s_symbol.empty()) {
        ch = s_symbol.top();
        switch (ch) {
            case '&':
                connectOperation();
                s_symbol.pop();
                break;
            case '|':
                choiceOperation();
                s_symbol.pop();
                break;
        }
    }
    if (!s_nfa.empty()) {
        this->obj_e_nfa = &s_nfa.top();
    }
    else cout << "stack_nfa error!" << endl;

    nfa.start = obj_e_nfa->start;
    nfa.end = obj_e_nfa->end;
    ntd.translate(&nfa);


//	outputGraph();
//	cout << "-----START_ID:" << nfa.start << "  END_ID:";
//	for (int i = 0; i < ntd.endnodes.size(); i++)
//	{
//		cout << ntd.endnodes[i] << ' ';
//	}
//	cout << endl;
//	ntd.outGraph();



}

void RexHandle::setRextext(string &s) {
    this->Rex = s;
}

vector<pair<int, int> > RexHandle::compareString(string &s) {

    return ntd.stringTest(s);
}

void RexHandle::outputPair(vector<pair<int, int> > a) {
    int count = 0;
    for (int i = 0; i < a.size(); ++i) {
        printf("%d,%d\n", a[i].first, a[i].second);
        if (a[i].first >= 0) count++;
    }
    printf("token:%d\n", count);
}
