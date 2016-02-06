//
// Created by TanGreen on 16/1/2.
//

#include "Parse.h"
#include <sstream>


enum TokenSymbol {
    T_DEF,
    T_STRING,
    T_INT,
    T_IDENTIFIER,
    T_IF,
    T_WHILE,
    T_NUMBER,
    T_BRACE_SL,
    T_BRACE_SR,
    T_BRACE_ML,
    T_BRACE_MR,
    T_BRACE_LL,
    T_BRACE_LR,
    T_LTHAN,
    T_GTHAN,
    T_GEQUAL,
    T_EQUAL,
    T_EEQUAL,
    T_LEQUAL,
    T_NON,
    T_ADD,
    T_MIN,
    T_MUL,
    T_DIV,
    T_COMMA,
    T_END,
    T_EXTERN,
    T_RETURN,
    T_SQUOTE,
    T_DQUOTE,
    T_ESCAPE,
    T_BLANK

};

//static map<string, IRitem* > IRTable;
static int labelcount;
static map<string, int> LogicMem;
static string asmfile;
static string rawcodefile;
static int precedenceArray[T_BLANK + 1];  //all initial to 0
static list<Token *> Tokenlist;
static list<Token *> TokenlistCopy;
static list<Token *>::iterator topfileit;
static int CurTok;
static string IdentifierStr;  // Filled in if T_IDENTIFIER
static int NumVal;              // Filled in if T_NUMBER

//
static int gettok() {
    if (++topfileit != Tokenlist.end()) {
        Token *tmp = *topfileit;
        switch (tmp->token) {
            case T_NUMBER:
                NumVal = atoi(tmp->value.c_str());
                return T_NUMBER;
            case T_IDENTIFIER:
                IdentifierStr = tmp->value;
                return T_IDENTIFIER;
            case T_DEF:
            case T_STRING:
            case T_INT:
            case T_IF:
            case T_WHILE:
            case T_BRACE_SL:
            case T_BRACE_SR:
            case T_BRACE_ML:
            case T_BRACE_MR:
            case T_BRACE_LL:
            case T_BRACE_LR:
            case T_EQUAL:
            case T_ADD:
            case T_MIN:
            case T_DIV:
            case T_MUL:
            case T_COMMA:
            case T_BLANK:
            case T_RETURN:
            default:
                return tmp->token;
        }
    }
    return -1;
}

static shared_ptr<ExprAST> ParseLocalExpression();

//
static int getNextToken() {
    return CurTok = gettok();
}

//
static int getTokPrecedence() {
    return precedenceArray[CurTok];
}

// Error*
shared_ptr<ExprAST> Error(string Str) {
    throw runtime_error(Str.c_str());
}


static shared_ptr<ExprAST> ParseExpression();

/* identifierexpr
 * ::= identifier
 * ::= identifier '(' expression* ')'  其实这里的标识符,要么是变量,要么是函数调用名
 * */
static shared_ptr<ExprAST> ParseIdentifierExpr() {
    string IdName = IdentifierStr;
    vector<shared_ptr<ExprAST> > Args;
    getNextToken();
    if (CurTok != T_BRACE_SL)
        return shared_ptr<ExprAST>(new VariableExprAST(IdName));
    getNextToken();//eat '('
    if (CurTok != T_BRACE_SR) {
        while (1) { //循环的目的是为了把函数的参数列表项都记录下来,来构造完整的函数调用AST.
            shared_ptr<ExprAST> Arg = ParseExpression(); //处理第一个表达式,暂时不用管ParseExpression怎么做的,只关心返回值.
            if (!Arg) return 0; //返回0,一定有函数调用的语法错误.
            Args.push_back(Arg);
            if (CurTok == T_BRACE_SR) {
                getNextToken();  //eat ')'
                break;
            }
            if (CurTok != T_COMMA) {
                string errormsg = "funcion " + IdName + " : expected ')' or ',' in argument list";
                return Error(errormsg);
            }
            getNextToken(); //eat ','
        }
    }
    if (CurTok == T_BRACE_SR) getNextToken(); //eat ')'
    return shared_ptr<ExprAST>(new CallExprAST(IdName, Args));
}

/* numberexpr
 * ::= number
 * */
static shared_ptr<ExprAST> ParseNumberExpr() {
    shared_ptr<ExprAST> result = shared_ptr<ExprAST>(new NumberExprAST(NumVal));
    getNextToken();
    return result;
}

/*
 * conststringexpr
 * ::= const string
 */
static shared_ptr<ExprAST> ParseConststringExpr() {
    getNextToken(); //eat " or '
    string tmp;
    while (CurTok != T_SQUOTE && CurTok != T_DQUOTE) {
        tmp = tmp + IdentifierStr;
        getNextToken();
        if (CurTok == -1) return Error("string didn't have end quote");
    }
    shared_ptr<ExprAST> result = shared_ptr<ExprAST>(new ConststringAST(tmp));
    getNextToken();
    return result;
}

/* parenexpr
 * ::= '(' expression ')'  带括号的抽象级别最高的表达式
 * */
static shared_ptr<ExprAST> ParseParenExpr() {
    getNextToken();        //eat ')'
    shared_ptr<ExprAST> exp = ParseExpression();
    if (!exp) return 0;
    if (CurTok != T_BRACE_SR)
        return Error("expected an ')' ");
    getNextToken(); //eat ')'
    return exp;
}

/* primary
 * ::= identifierexpr   变量,函数调用的表达式
 * ::= numberexpr       字面值(数字)表达式
 * ::= parenexpr        带括号的表达式
 * ::= const string
 * */
static shared_ptr<ExprAST> ParsePrimary() {
    switch (CurTok) {
        case T_BRACE_SL:
            return ParseParenExpr();
        case T_IDENTIFIER:
            return ParseIdentifierExpr();
        case T_NUMBER:
            return ParseNumberExpr();
        case T_DQUOTE:
            return ParseConststringExpr();
        case T_SQUOTE:
            return ParseConststringExpr();
        default:
            return Error("unknown token when expected an expression  ");
    }
}

/* binoprhs             到这里为止,primary是描述的单一的表达式类型,下面将写出带二元操作符的复合表达式类型
 * ::= ('+' primary)*
 * */
static shared_ptr<ExprAST> ParseBinOpRHS(int lasprec, shared_ptr<ExprAST> LHS) {
    while (1) {
        int curprec = getTokPrecedence();
        if (curprec < lasprec)
            return LHS;
        int op = CurTok;    //先保存一下,为了后面构造二元对AST
        getNextToken();     //eat op
        auto RHS = ParsePrimary();//由于之前写好的推导,括号情况就被包括进去了,这里就不用考虑了.
        if (!RHS) return 0;
        int nextprec = getTokPrecedence();
        if (curprec < nextprec) {
            RHS = ParseBinOpRHS(curprec + 1, RHS); //向优先级更高的情况递归,一旦出现优先级比当前符号优先级小的,就不再递归
            if (!RHS) return 0;
        }
        LHS = shared_ptr<ExprAST>(new BinaryExprAST(op, LHS, RHS));
    }
}

/* expression           完整的表达式语法结束  右值表达式
 * ::= primary binoprhs
 * */
static shared_ptr<ExprAST> ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS)
        return 0;
    if (LHS->getType() == Ty_string)
        return LHS;
    return ParseBinOpRHS(1, LHS);
}

/* conditionexpr
 * */
static shared_ptr<ConditionAST> ParseConditionExpr() {
    getNextToken(); // eat cond symbol
    shared_ptr<ExprAST> lhs = ParseExpression();
    string cond = "";
    switch (CurTok) {
        case T_EQUAL:
            getNextToken();
            if (CurTok == T_EQUAL) {
                cond = "==";
                getNextToken();
            }
            else cond = "=";
            break;
        case T_GTHAN:
            getNextToken();
            if (CurTok == T_EQUAL) {
                cond = ">=";
                getNextToken();
            }
            else cond = ">";
            break;
        case T_LTHAN:
            getNextToken();
            if (CurTok == T_EQUAL) {
                cond = "<=";
                getNextToken();
            }
            else cond = "<";
            break;
        case T_NON:
            getNextToken();
            if (CurTok == T_EQUAL) {
                cond = "!=";
                getNextToken();
            }
            else return 0;
            break;
        default:
            break;
    }
    if (cond != "") {
        shared_ptr<ExprAST> rhs = ParseExpression();
        return shared_ptr<ConditionAST>(new ConditionAST(lhs, rhs, cond));
    } else return 0;
}

/* assignmentexpr
 * */
static shared_ptr<ExprAST> ParseAssignmentExpr() {
    shared_ptr<ExprAST> var = ParseIdentifierExpr();
    if (!var)
        return 0;
    if (CurTok != T_EQUAL)
        return var;
    getNextToken(); //eat '='
    shared_ptr<ExprAST> expr = ParseExpression();
    if (!expr)
        return 0;
    return shared_ptr<AssignmentAST>(new AssignmentAST(var, expr));
}

/* returnexpr
 * */
static shared_ptr<ExprAST> ParseReturnExpr() {
    getNextToken();//eat return
    return shared_ptr<ExprAST>(new ReturnAST(ParseExpression()));
}

/* whileexpr
 * */
static shared_ptr<ExprAST> ParseWhileExpr() {
    shared_ptr<ExprAST> cond = ParseConditionExpr();
    vector<shared_ptr<ExprAST> > body;
    while (CurTok != T_END) {
        shared_ptr<ExprAST> e = ParseLocalExpression();
        if (e)
            body.push_back(e);
        else return 0;
    }
    if (CurTok != T_END) Error("'while' statement should end by 'end' ");
    getNextToken();//eat end
    return shared_ptr<WhileAST>(new WhileAST(cond, body));
}

/*
 * ifexpr
 */
static shared_ptr<ExprAST> ParseIfExpr() {
    shared_ptr<ExprAST> cond = ParseConditionExpr();
    vector<shared_ptr<ExprAST> > body;
    while (CurTok != T_END) {
        shared_ptr<ExprAST> e = ParseLocalExpression();
        if (e)
            body.push_back(e);
        else break;
    }
    if (CurTok != T_END) Error("'if' statement should end by 'end' ");
    getNextToken();//eat end
    return shared_ptr<IfAST>(new IfAST(cond, body));
}

/* localexpression           完整的表达式语法结束  左值表达式
 * ::= assignment | returnexpr
 * */
static shared_ptr<ExprAST> ParseLocalExpression() {
    if (CurTok == T_RETURN)
        return ParseReturnExpr();
    if (CurTok == T_IDENTIFIER)
        return ParseAssignmentExpr();
    if (CurTok == T_WHILE)
        return ParseWhileExpr();
    if (CurTok == T_IF)
        return ParseIfExpr();
    return 0;
}


/* prototype                                   函数接口 只用于函数定义,外部函数,函数的前置声明
 * ::= identifierexpr '(' identifierexpr* ')'  注意,这里的id是指的产生式
 * */
static shared_ptr<PrototypeAST> ParsePrototype() {
    if (CurTok != T_IDENTIFIER)
        Error("expected function name in prototype");
    string FnName = IdentifierStr;
    getNextToken();
    if (CurTok != T_BRACE_SL)
        Error("expected '(' in prototype");
    vector<string> ArgNames;
    while (getNextToken() != T_BRACE_SR) {
        if (CurTok == T_IDENTIFIER)
            ArgNames.push_back((*topfileit)->value);
    }
    getNextToken();  // eat ')'.
    return shared_ptr<PrototypeAST>(new PrototypeAST(FnName, ArgNames));
}


/* definition                      函数定义的产生式
 * ::= 'def' prototype expression
 * */
static shared_ptr<FunctionAST> ParseDefinition() {
    getNextToken();  // eat def.
    shared_ptr<PrototypeAST> Proto = ParsePrototype();
    vector<shared_ptr<ExprAST> > body;
    if (Proto == 0) return 0;
    while (shared_ptr<ExprAST> E = ParseLocalExpression()) {
        if (CurTok == T_END) {
            body.push_back(E);
            getNextToken(); //eat T_END
            return shared_ptr<FunctionAST>(new FunctionAST(Proto, body));
        }
        else body.push_back(E);
    }
    if (CurTok == -1) Error("function definition should end by 'end'");
    else Error("invalid expression statement");
    return 0;
}

/* toplevelexpr
 * ::= expression
 * ::= assignment
 * ::= conditionexpr
 * */
static shared_ptr<ExprAST> ParseTopLevelExpr() {
    shared_ptr<ExprAST> E;
    if (CurTok == T_WHILE)
        E = ParseWhileExpr();
    else if (CurTok == T_IF)
        E = ParseIfExpr();
    else if (CurTok == T_IDENTIFIER)
        E = ParseAssignmentExpr();
    else Error("invalid expression statement");
    if (E) {
        return E;
    } else return 0;
}

/// external ::= 'extern' prototype
static shared_ptr<PrototypeAST> ParseExtern() {
    getNextToken();  // eat extern.
    return ParsePrototype();
}


static void HandleDefinition() {
    if (shared_ptr<ExprAST> E = ParseDefinition()) {
        E->codegen();
        //cout<<"Parsed a function definition.\n";
    } else {
        getNextToken();
    }
}

static void HandleExtern() {
    if (shared_ptr<ExprAST> E = ParseExtern()) {
        //cout<<"Parsed an extern\n";
    } else {
        getNextToken();
    }
}

static void HandleTopLevelExpression() {
    if (shared_ptr<ExprAST> E = ParseTopLevelExpr()) {
        E->codegen();
        //cout<<"Parsed a top-level expr\n";
    } else {
        getNextToken();
    }
}

/// top ::= definition | external | expression | 'end'
static void MainLoop() {
    try {
        while (1) {
            switch (CurTok) {
                case -1:
                    cout << "Woo " << rawcodefile << ":" << endl;
                    cout << "#######  success  #######" << endl;
                    return;
                case T_DEF:
                    HandleDefinition();
                    break;
                case T_EXTERN:
                    HandleExtern();
                    break;
                default:
                    HandleTopLevelExpression();
                    break;
            }
        }
    } catch (runtime_error e) {
        if (topfileit == TokenlistCopy.end()) {
            cout << "Woo " << rawcodefile << "(" << TokenlistCopy.back()->lineIndex << "," <<
            TokenlistCopy.back()->start << ")" << ": ";
            cout << "Error: " << e.what() << endl;
            int length = 0;

            for (auto it : TokenlistCopy) {
                if (it->lineIndex == TokenlistCopy.back()->lineIndex) {
                    cout << it->value;
                    length += it->value.size();
                }
            }
            cout << endl;

            for (int i = 0; i < length; ++i) {
                if (i == TokenlistCopy.back()->start) {
                    cout << '^';
                    continue;
                }
                if (i >= TokenlistCopy.back()->start && i <= TokenlistCopy.back()->end)
                    cout << '~';
                else
                    cout << ' ';
            }
            return;
        }

        cout << "Woo " << rawcodefile << "(" << (*topfileit)->lineIndex << "," << (*topfileit)->start << ")" << ": ";
        cout << "Error: " << e.what() << endl;
        int length = 0;
        for (auto it : TokenlistCopy) {
            if (it->lineIndex == (*topfileit)->lineIndex) {
                cout << it->value;
                length += it->value.size();
            }
        }
        cout << endl;

        for (int i = 0; i < length; ++i) {
            if (i == (*topfileit)->start) {
                cout << '^';
                continue;
            }
            if (i >= (*topfileit)->start && i <= (*topfileit)->end)
                cout << '~';
            else
                cout << ' ';
        }
        return;
    }
}


Parse::Parse(list<Token *> &list, const string s, const string src) {
    Tokenlist = list;
    TokenlistCopy = list;
    asmfile = s;
    rawcodefile = src;
    ofstream f;
    f.open(asmfile, ios::trunc); //discard the contents when first open
    f.close();
}


void Parse::test() {
    //remove the no use blank  &&  change all of the string contents to T_IDENTIFIER
    for (list<Token *>::iterator it = Tokenlist.begin(); it != Tokenlist.end(); ++it) {
        if ((((*it)->token) == T_DQUOTE) || (((*it)->token) == T_SQUOTE)) {
            //cout<<"1:"<<(*it)->value<<endl;
            ++it;
            if (it == Tokenlist.end())
                break;
            while ((((*it)->token) != T_DQUOTE) && (((*it)->token) != T_SQUOTE)) {
                //cout << "2:" << (*it)->value << endl;
                (*it)->token = T_IDENTIFIER;
                if (it == Tokenlist.end())
                    break;
                ++it;
            }
        }
        else if ((*it)->token == T_BLANK) {
            //cout<<"3:"<<(*it)->value<<endl;
            it = Tokenlist.erase(it);
            --it;
        }
        //cout<<"4:"<<(*it)->value<<endl;
    }

    topfileit = Tokenlist.begin();
    --topfileit;
    getNextToken();
    CurTok = (*topfileit)->token;
    precedenceArray[T_ADD] = 20;
    precedenceArray[T_MIN] = 20;
    precedenceArray[T_MUL] = 30;
    precedenceArray[T_DIV] = 30;
    labelcount = 0;
    MainLoop();
}


string ExprAST::codegen() {
    return "";
}

string NumberExprAST::codegen() {
    stringstream ss;
    ss << this->Val;
    string s;
    ss >> s;
    s = "d: " + s;
    LogicMem[s] = this->Val;
    return s;
}

string VariableExprAST::codegen() {
    LogicMem[this->Name] = 0;
    return this->Name;
}


string BinaryExprAST::codegen() {
    string laddr = LHS->codegen();
    string raddr = RHS->codegen();
    ofstream fout;
    fout.open(asmfile, ios::app);

    stringstream lss;
    lss << 't' << LogicMem.size();
    string ltmp;
    lss >> ltmp;
    LogicMem[ltmp] = 0;
    fout << ltmp << " = " << laddr << endl;

    stringstream rss;
    rss << 't' << LogicMem.size();
    string rtmp;
    rss >> rtmp;
    LogicMem[rtmp] = 0;
    fout << rtmp << " = " << raddr << endl;


    stringstream ss;
    ss << 't' << LogicMem.size();
    string s;
    ss >> s;
    switch (Op) {
        case T_ADD:
            fout << s << " + " << ltmp << " " << rtmp << endl;
            break;
        case T_MIN:
            fout << s << " - " << ltmp << " " << rtmp << endl;
            break;
        case T_MUL:
            fout << s << " * " << ltmp << " " << rtmp << endl;
            break;
        case T_DIV:
            fout << s << " / " << ltmp << " " << rtmp << endl;
            break;
        default:
            break;
    }
    fout.close();
    LogicMem[s] = 0; //add a mem item
    return s;
}

string CallExprAST::codegen() {
    //param list
    ofstream fout;
    fout.open(asmfile, ios::app);
    vector<string> params;
    for (auto pa : this->Args) {
        params.push_back(pa->codegen());
    }
    for (auto t : params) {
        stringstream ss;
        ss << 't' << LogicMem.size();
        string s;
        ss >> s;
        fout << s << " = " << t << endl;
        fout << "PARAM " << s << endl;
        LogicMem[s] = 0;
    }
    stringstream ss;
    ss << 't' << LogicMem.size();
    string s;
    ss >> s;
    fout << s << " = " << "CALL " << this->Callna << " " << params.size() << endl;
    LogicMem[s] = 0;
    fout.close();
    return s;
}

string PrototypeAST::codegen() {
    ofstream fout;
    fout.open(asmfile, ios::app);
    fout << "FUNCTION " << this->Name << endl;
    for (auto tmp : this->Args) {
        fout << "PARAM " << tmp << endl;
    }

    fout.close();
    return "";
}

string FunctionAST::codegen() {

    this->Proto->codegen();
    for (auto tmp : this->Body) {
        tmp->codegen();
    }
    ofstream fout;
    fout.open(asmfile, ios::app);
    fout << "EXIST" << endl;
    return "";
}

string AssignmentAST::codegen() {
    string varaddr = Var->codegen();
    string rhsaddr = RHS->codegen();
    ofstream fout;
    fout.open(asmfile, ios::app);
    stringstream ss;
    ss << 't' << LogicMem.size();
    string s;
    ss >> s;
    fout << s << " = " << rhsaddr << endl;
    fout << varaddr << " = " << s << endl;
    LogicMem[s] = 0;
    fout.close();
    return varaddr;
}

string ReturnAST::codegen() {
    string tmp = this->rtVal->codegen();
    ofstream fout;
    fout.open(asmfile, ios::app);
    stringstream ss;
    ss << 't' << LogicMem.size();
    string s;
    ss >> s;
    fout << "RETURN " << s << " " << tmp << endl;
    LogicMem[s] = 0;
    fout.close();
    return s;
}

string WhileAST::codegen() {
    string s = condExpr->codegen();

    ofstream fout;
    fout.open(asmfile, ios::app);

    int record = labelcount;
    int record2 = record + 1;

    fout << "IF " << s << " GOTO LABEL " << ++labelcount << endl;
    fout.close();

    for (auto it : this->Body) {
        it->codegen();
    }

    fout.open(asmfile, ios::app);
    fout << "GOTO LABEL " << record << endl;
    fout << "LABEL " << record2 << endl;;
    fout.close();
    return "";
}

string ConditionAST::codegen() {    //条件表达式一般是被其他控制结构的codegen()调用,所以这里返回值要特殊处理一下.
    string s;

    ofstream fout;                  // lable 必须放在这,不然参数回填有麻烦
    fout.open(asmfile, ios::app);
    fout << "LABEL " << ++labelcount << endl;
    fout.close();

    string l = this->LHS->codegen();
    string r = this->RHS->codegen();


    fout.open(asmfile, ios::app);
    stringstream lss;
    lss << 't' << LogicMem.size();
    string ltmp;
    lss >> ltmp;
    LogicMem[ltmp] = 0;
    fout << ltmp << " = " << l << endl;

    stringstream rss;
    rss << 't' << LogicMem.size();
    string rtmp;
    rss >> rtmp;
    LogicMem[rtmp] = 0;
    fout << rtmp << " = " << r << endl;

    s = ltmp + " " + this->Cond + " " + rtmp;
    return s;
}

string IfAST::codegen() {
    string s = condExpr->codegen();

    ofstream fout;
    fout.open(asmfile, ios::app);
    fout << "IF " << s << " GOTO LABEL " << ++labelcount << endl;

    int record = labelcount; //记住label

    fout.close();

    for (auto it : this->Body) {
        it->codegen();
    }

    fout.open(asmfile, ios::app);
    fout << "LABEL " << record << endl;
    fout.close();
    return "";
}

string ConststringAST::codegen() {
    ofstream fout;
    fout.open(asmfile, ios::app);
    stringstream ss;
    ss << 't' << LogicMem.size();
    string s;
    ss >> s;
    LogicMem[s] = 0;
    fout << s << " = s: " << Content << endl;
    return s;
}
