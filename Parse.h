//
// Created by TanGreen on 16/1/2.
//

#ifndef MYCOMPILER_PARSE_H
#define MYCOMPILER_PARSE_H

#include "Token.h"
#include <list>
#include <cstdlib>
#include <memory>

class Parse {
public:
    Parse(list<Token *> &list);

    void test();
};

class ExprAST {
public:
    virtual ~ExprAST() { }
};

class NumberExprAST : public ExprAST {
    double Val;
public:
    NumberExprAST(double val) : Val(val) { }
};

class VariableExprAST : public ExprAST {
    string Name;
public:
    VariableExprAST(const string &name) : Name(name) { }
};

class BinaryExprAST : public ExprAST {
    char Op;
    shared_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char op, shared_ptr<ExprAST> lhs, shared_ptr<ExprAST> rhs) : Op(op), LHS(lhs), RHS(rhs) { }
};

class CallExprAST : public ExprAST {
    string Callna;
    vector<shared_ptr<ExprAST> > Args;
public:
    CallExprAST(const string &callna, vector<shared_ptr<ExprAST> > &args) : Callna(callna), Args(args) { }
};

//function declaration
class PrototypeAST : public ExprAST {
    string Name;
    vector<string> Args;
public:
    PrototypeAST(const string &name, vector<string> &args) : Name(name), Args(args) { }
};

//function definition
class FunctionAST : public ExprAST {
    shared_ptr<PrototypeAST> Proto;
    shared_ptr<ExprAST> Body;
public:
    FunctionAST(shared_ptr<PrototypeAST> proto, shared_ptr<ExprAST> body) : Proto(proto), Body(body) { }
};






#endif //MYCOMPILER_PARSE_H
