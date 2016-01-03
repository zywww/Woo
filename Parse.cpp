//
// Created by TanGreen on 16/1/2.
//

#include "Parse.h"


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
    T_MUL,
    T_COMMA,
    T_BLANK

};

Parse::Parse() {

}
