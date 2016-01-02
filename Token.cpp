//
// Created by TanGreen on 15/12/31.
//

#include "Token.h"
#include <fstream>

#define REX_NUMBER "(0|1|2|3|4|5|6|7|8|9)"
#define REX_CHAR "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)"
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


Token::Token() {

}

void Scan::getToken() {
    string s = string(T_map[T_IDENTIFIER])
               + string("|") + string(T_map[T_NUMBER])
               + string("|") + string(T_map[T_BRACE_SL])
               + string("|") + string(T_map[T_BRACE_SR])
               + string("|") + string(T_map[T_BRACE_ML])
               + string("|") + string(T_map[T_BRACE_MR])
               + string("|") + string(T_map[T_BRACE_LL])
               + string("|") + string(T_map[T_BRACE_LR])
               + string("|") + string(T_map[T_EQUAL])
               + string("|") + string(T_map[T_ADD])
               + string("|") + string(T_map[T_MUL])
               + string("|") + string(T_map[T_COMMA]);
    RexHandle rh;
    rh.setRextext(s);
    rh.run();

    fstream in;
    ofstream out;
    in.open("/Users/TanGreen/ClionProjects/mycompiler/Debug/main.txt");
    out.open("/Users/TanGreen/ClionProjects/mycompiler/Debug/out.txt",ios::trunc);

    string ts;
    int line = 0;
    vector<pair<int, int> > tpair;
    while (getline(in, ts)) {
        line++;
        tpair = rh.compareString(ts);
        for (int i = 0; i < tpair.size(); ++i) {
            if (tpair[i].first >= 0 && tpair[i].second >= 0) {
                Token *ttoken = new Token;
                ttoken->start = tpair[i].first;
                ttoken->end = tpair[i].second;
                string tmp = ts.substr(ttoken->start, ttoken->end-ttoken->start+1);
                ttoken->token = getTokenId(tmp);
                ttoken->value = tmp;
                ttoken->lineIndex = line;
                list_token.push_back(ttoken);
                out<<"Line:"<<ttoken->lineIndex<<" At:"<<ttoken->start<<" TokenType:"<<ttoken->token<<" Value="<<tmp<<endl;
            }
        }
    }
    in.close();
    out.close();
}

Scan::Scan() {
    T_map.insert(pair<int, string>(T_STRING,     string("string")));
    T_map.insert(pair<int, string>(T_INT,        string("int")));
    T_map.insert(pair<int, string>(T_IDENTIFIER, string(REX_CHAR) + string("*")));//TODO add '+' to regex system is better
    T_map.insert(pair<int, string>(T_IF,         string("if")));
    T_map.insert(pair<int, string>(T_WHILE,      string("while")));
    T_map.insert(pair<int, string>(T_NUMBER,     string(REX_NUMBER) + string("*")));
    T_map.insert(pair<int, string>(T_BRACE_SL,   string("\\(")));
    T_map.insert(pair<int, string>(T_BRACE_SR,   string("\\)")));
    T_map.insert(pair<int, string>(T_BRACE_ML,   string("\\[")));
    T_map.insert(pair<int, string>(T_BRACE_MR,   string("\\]")));
    T_map.insert(pair<int, string>(T_BRACE_LL,   string("\\{")));
    T_map.insert(pair<int, string>(T_BRACE_LR,   string("\\}")));
    T_map.insert(pair<int, string>(T_EQUAL,      string("\\=")));
    T_map.insert(pair<int, string>(T_ADD,        string("\\+|\\-")));
    T_map.insert(pair<int, string>(T_MUL,        string("\\*|\\/")));
    T_map.insert(pair<int, string>(T_MUL,        string("\\*|\\/")));
    T_map.insert(pair<int, string>(T_COMMA,      string("\\,")));
    T_map.insert(pair<int, string>(T_BLANK,      string("\32*")));
    T_map.insert(pair<int, string>(T_DEF,        string("def")));

}

int Scan::getTokenId(string s) {
    if (s[0] == '\32')             return T_BLANK;
    if (s == "string")             return T_STRING;
    if (s == "int")                return T_INT;
    if (s == "if")                 return T_IF;
    if (s == "while")              return T_WHILE;
    if (s == "(")                  return T_BRACE_SL;
    if (s == ")")                  return T_BRACE_SR;
    if (s == "[")                  return T_BRACE_ML;
    if (s == "]")                  return T_BRACE_MR;
    if (s == "{")                  return T_BRACE_LL;
    if (s == "}")                  return T_BRACE_LR;
    if (s == "=")                  return T_EQUAL;
    if (s == "+"||s == "-")        return T_ADD;
    if (s == "*"||s == "/")        return T_MUL;
    if (s == ",")                  return T_COMMA;
    if (s == "def")                return T_DEF;
    for (int i = 0; i < s.size(); ++i) {
        if(!(s[i]>=0&&s[i]<=9))
            break;
        else if(i==s.size()) return T_NUMBER;
    }
    return T_IDENTIFIER;
}
