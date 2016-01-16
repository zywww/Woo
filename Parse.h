//
// Created by TanGreen on 16/1/2.
//

#ifndef MYCOMPILER_PARSE_H
#define MYCOMPILER_PARSE_H

#include "Token.h"
#include <list>
#include <cstdlib>
#include <fstream>
#include <map>


enum VaribleType {
    Ty_unknown,
    Ty_int,
    Ty_double,
    Ty_string,
    Ty_bincombo
};


class Parse {
public:
    Parse(list<Token *> &list);

    void test();
};

class ExprAST {
public:
    virtual unsigned int getType() { return Ty_unknown; }

    virtual string codegen();
    virtual ~ExprAST() { }
};

class NumberExprAST : public ExprAST {
    int Val;
public:
    unsigned int getType() { return Ty_int; }

    string codegen();

    NumberExprAST(int val) : Val(val) { }
};

class VariableExprAST : public ExprAST {
    string Name;
public:
    unsigned int getType() { return Ty_string; }

    string codegen();
    VariableExprAST(const string &name) : Name(name) { }
};

class BinaryExprAST : public ExprAST {
    int Op;
    shared_ptr<ExprAST> LHS, RHS;
public:
    unsigned int getType() { return Ty_bincombo; }

    string codegen();

    BinaryExprAST(int op, shared_ptr<ExprAST> lhs, shared_ptr<ExprAST> rhs) : Op(op), LHS(lhs), RHS(rhs) { }
};

class CallExprAST : public ExprAST {
    string Callna;
    vector<shared_ptr<ExprAST> > Args;
public:
    string codegen();
    CallExprAST(const string &callna, vector<shared_ptr<ExprAST> > &args) : Callna(callna), Args(args) { }
};

//function declaration
class PrototypeAST : public ExprAST {
    string Name;
    vector<string> Args;
public:
    string codegen();
    PrototypeAST(const string &name, vector<string> &args) : Name(name), Args(args) { }
};

//function definition
class FunctionAST : public ExprAST {
    shared_ptr<PrototypeAST> Proto;
    vector<shared_ptr<ExprAST> > Body;
public:
    string codegen();

    FunctionAST(shared_ptr<PrototypeAST> proto, vector<shared_ptr<ExprAST> > body) : Proto(proto), Body(body) { }
};

//assignment
class AssignmentAST : public ExprAST {
    shared_ptr<ExprAST> Var;
    shared_ptr<ExprAST> RHS;
public:
    string codegen();

    AssignmentAST(shared_ptr<ExprAST> var, shared_ptr<ExprAST> rhs) : Var(var), RHS(rhs) { }
};

//return
class ReturnAST : public ExprAST {
    shared_ptr<ExprAST> rtVal;
public:
    string codegen();

    ReturnAST(shared_ptr<ExprAST> var) : rtVal(var) { }
};

//while
class WhileAST : public ExprAST {
};

//foreach
class ForeachAST : public ExprAST {

};

//IR items
class IRitem {
public:
    string varL;
    string varR;
    string rusult;
    int OP;
};



#endif //MYCOMPILER_PARSE_H
