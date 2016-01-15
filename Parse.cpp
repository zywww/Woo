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
    T_EQUAL,
    T_ADD,
    T_MIN,
    T_MUL,
    T_DIV,
    T_COMMA,
    T_END,
    T_EXTERN,
    T_BLANK
};

static string getRegistSymbol(int i) {
    switch (i) {
        case -1:
            return "RT";
        case -2:
            return "PC";
        case -3:
            return "AC";
        default:
            return "";
    }
}

//static map<string, IRitem* > IRTable;
static map<string, int> LogicMem;
static string asmfile = "/Users/TanGreen/ClionProjects/Woo/Debug/asm.txt";
static int precedenceArray[T_BLANK + 1];
static list<Token *> Tokenlist;
static list<Token *>::iterator it;
static int CurTok;
static string IdentifierStr;  // Filled in if T_IDENTIFIER
static int NumVal;              // Filled in if T_NUMBER

//
static int gettok() {
    if (++it != Tokenlist.end()) {
        Token *tmp = *it;
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
            default:
                return tmp->token;
        }
    }
    return -1;
}

//
static int getNextToken() {
    return CurTok = gettok();
}

//
static int getTokPrecedence() {
    return precedenceArray[CurTok];
}

// Error*
shared_ptr<ExprAST> Error(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return 0;
}

shared_ptr<PrototypeAST> ErrorP(const char *Str) {
    Error(Str);
    return 0;
}

shared_ptr<FunctionAST> ErrorF(const char *Str) {
    Error(Str);
    return 0;
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
            if (CurTok != T_COMMA)
                return Error("Expected ')' or ',' in argument list\n");
            getNextToken(); //eat ','
        }
    }
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

/* parenexpr
 * ::= '(' expression ')'  带括号的抽象级别最高的表达式
 * */
static shared_ptr<ExprAST> ParseParenExpr() {
    getNextToken();        //eat ')'
    shared_ptr<ExprAST> exp = ParseExpression();
    if (!exp) return 0;
    if (CurTok != T_BRACE_SR)
        return Error("expected an ')'    ");
    getNextToken(); //eat ')'
    return exp;
}

/* primary
 * ::= identifierexpr   变量,函数调用的表达式
 * ::= numberexpr       字面值(数字)表达式
 * ::= parenexpr        带括号的表达式
 * */
static shared_ptr<ExprAST> ParsePrimary() {
    switch (CurTok) {
        case T_BRACE_SL:
            return ParseParenExpr();
        case T_IDENTIFIER:
            return ParseIdentifierExpr();
        case T_NUMBER:
            return ParseNumberExpr();
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

/* expression           完整的表达式语法结束
 * ::= primary binoprhs
 * */
static shared_ptr<ExprAST> ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS)
        return 0;
    return ParseBinOpRHS(1, LHS);
}

/* prototype                                   函数接口 只用于函数定义,外部函数,函数的前置声明
 * ::= identifierexpr '(' identifierexpr* ')'  注意,这里的id是指的产生式
 * */
static shared_ptr<PrototypeAST> ParsePrototype() {
    if (CurTok != T_IDENTIFIER)
        return ErrorP("Expected function name in prototype");
    string FnName = IdentifierStr;
    getNextToken();
    if (CurTok != T_BRACE_SL)
        return ErrorP("Expected '(' in prototype");
    vector<string> ArgNames;
    while (getNextToken() != T_BRACE_SR) {
        if (CurTok == T_IDENTIFIER)
            ArgNames.push_back((*it)->value);
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
    if (Proto == 0) return 0;
    if (shared_ptr<ExprAST> E = ParseExpression()) {
        if (CurTok == T_END)
            return shared_ptr<FunctionAST>(new FunctionAST(Proto, E));
        else return ErrorF("expected 'end' in function definition");
    }
    return 0;
}

/* assignmentexpr
 * */
static shared_ptr<AssignmentAST> ParseAssignmentExpr() {
    shared_ptr<ExprAST> var = ParseIdentifierExpr();
    if (!var)
        return 0;
    if (CurTok != T_EQUAL)
        return shared_ptr<AssignmentAST>(new AssignmentAST(var, shared_ptr<ExprAST>(new ExprAST))); //int a 令a初始化为0
    getNextToken(); //eat '='
    shared_ptr<ExprAST> expr = ParseExpression();
    if (!expr)
        return 0;
    return shared_ptr<AssignmentAST>(new AssignmentAST(var, expr));
}


/* toplevelexpr
 * ::= expression
 * ::= assignment
 * */
static shared_ptr<FunctionAST> ParseTopLevelExpr() {
    shared_ptr<ExprAST> E = ParseAssignmentExpr();
    if (E) {
        E->codegen();
        vector<string> empty;
        shared_ptr<PrototypeAST> Proto = shared_ptr<PrototypeAST>(new PrototypeAST("", empty));
        return shared_ptr<FunctionAST>(new FunctionAST(Proto, E));
    } else return 0;
}

/// external ::= 'extern' prototype
static shared_ptr<PrototypeAST> ParseExtern() {
    getNextToken();  // eat extern.
    return ParsePrototype();
}


static void HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else {
        getNextToken();
    }
}

static void HandleExtern() {
    if (ParseExtern()) {
        fprintf(stderr, "Parsed an extern\n");
    } else {
        getNextToken();
    }
}

static void HandleTopLevelExpression() {
    if (ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expr\n");
    } else {
        getNextToken();
    }
}

/// top ::= definition | external | expression | 'end'
static void MainLoop() {
    while (1) {
        fprintf(stderr, "ready> ");
        switch (CurTok) {
            case -1:
                return;
            case T_END:
                getNextToken();
                break;  // ignore top-level semicolons.
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
}


Parse::Parse(list<Token *> &list) {
    Tokenlist = list;
    it = Tokenlist.begin();
    it--;
    getNextToken();
    CurTok = (*it)->token;
    precedenceArray[T_ADD] = 20;
    precedenceArray[T_MUL] = 30;
}


void Parse::test() {
    MainLoop();
}


string ExprAST::codegen() {

}

string NumberExprAST::codegen() {
    stringstream ss;
    ss << this->Val;
    string s;
    ss >> s;
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
    stringstream ss;
    ss << 't' << LogicMem.size();
    string s;
    ss >> s;
    switch (Op) {
        case T_ADD:
            fout << s << " + " << laddr << " " << raddr << endl;
            break;
        case T_MIN:
            fout << s << " + " << laddr << " " << raddr << endl;
            break;
        default:
            break;
    }
    fout.close();
    LogicMem[s] = 0; //add a mem item
    return s;
}

string CallExprAST::codegen() {
    return ExprAST::codegen();
}

string PrototypeAST::codegen() {
    return ExprAST::codegen();
}

string FunctionAST::codegen() {
    return ExprAST::codegen();
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
