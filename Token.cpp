//
// Created by TanGreen on 15/12/31.
//

#include "Token.h"
#include <fstream>
//TODO add '+' to regex engine is better
//TODO  正则引擎不是贪婪的
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
                               + string("|") + string(T_map[T_NON])
                               + string("|") + string(T_map[T_GTHAN])
                               + string("|") + string(T_map[T_LTHAN])
                               + string("|") + string(T_map[T_EQUAL])
                               + string("|") + string(T_map[T_ADD])
                               + string("|") + string(T_map[T_MIN])
                               + string("|") + string(T_map[T_MUL])
                               + string("|") + string(T_map[T_DIV])
                               + string("|") + string(T_map[T_COMMA])
                               + string("|") + string(T_map[T_SQUOTE])
                               + string("|") + string(T_map[T_DQUOTE])
                               + string("|") + string(T_map[T_ESCAPE])
                               + string("|") + string(T_map[T_BLANK])
                               + string("|") + string(T_map[-1]); //no use symnbol

    RexHandle rh;
    rh.setRextext(s);
    rh.run();

    fstream in;
    ofstream out;
    in.open(srcfile);
    out.open(outfile, ios::trunc);

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
                string tmp = ts.substr((unsigned long) ttoken->start, (unsigned long) ttoken->end - ttoken->start + 1);
                ttoken->token = getTokenId(tmp);
                ttoken->value = tmp;
                ttoken->lineIndex = line;
                list_token.push_back(ttoken);
                out << "Line:" << ttoken->lineIndex << " At:" << ttoken->start << " TokenType:" << ttoken->token <<
                " Value= " << tmp << endl;
            }
        }
    }
    in.close();
    out.close();
}

Scan::Scan() {
    T_map.insert(pair<int, string>(T_IDENTIFIER, string(REX_CHAR) + string("*")));
    T_map.insert(pair<int, string>(T_NUMBER,     string(REX_NUMBER) + string("*")));
    T_map.insert(pair<int, string>(T_BRACE_SL, string("\\(")));
    T_map.insert(pair<int, string>(T_BRACE_SR, string("\\)")));
    T_map.insert(pair<int, string>(T_BRACE_ML, string("\\[")));
    T_map.insert(pair<int, string>(T_BRACE_MR, string("\\]")));
    T_map.insert(pair<int, string>(T_BRACE_LL, string("\\{")));
    T_map.insert(pair<int, string>(T_BRACE_LR, string("\\}")));
    T_map.insert(pair<int, string>(T_LTHAN, string("\\<")));
    T_map.insert(pair<int, string>(T_GTHAN, string("\\>")));
    T_map.insert(pair<int, string>(T_EQUAL, string("\\=")));
    T_map.insert(pair<int, string>(T_NON, string("\\!")));
    T_map.insert(pair<int, string>(T_ADD, string("\\+")));
    T_map.insert(pair<int, string>(T_MIN, string("\\-")));
    T_map.insert(pair<int, string>(T_MUL, string("\\*")));
    T_map.insert(pair<int, string>(T_DIV, string("\\/")));
    T_map.insert(pair<int, string>(T_COMMA, string("\\,")));
    T_map.insert(pair<int, string>(T_SQUOTE, string("\\'")));
    T_map.insert(pair<int, string>(T_DQUOTE, string("\\\"")));
    T_map.insert(pair<int, string>(T_ESCAPE, string("\\\\")));
    T_map.insert(pair<int, string>(T_BLANK, string("\\ ")));   //here blank don't need escape !!!! WTF
    T_map.insert(pair<int, string>(-1, string("\\~|\\@|\\#|\\$|\\^|\\_|\\?")));
}

int Scan::getTokenId(string s) {
    if (s[0] == '\32') return T_BLANK;
    if (s == "string") return T_STRING;
    if (s == "int") return T_INT;
    if (s == "if") return T_IF;
    if (s == "while") return T_WHILE;
    if (s == "(") return T_BRACE_SL;
    if (s == ")") return T_BRACE_SR;
    if (s == "[") return T_BRACE_ML;
    if (s == "]") return T_BRACE_MR;
    if (s == "{") return T_BRACE_LL;
    if (s == "}") return T_BRACE_LR;
    if (s == ">") return T_GTHAN;
    if (s == "<") return T_LTHAN;
    if (s == "!") return T_NON;
    if (s == "=") return T_EQUAL;
    if (s == "+") return T_ADD;
    if (s == "-") return T_MIN;
    if (s == "*") return T_MUL;
    if (s == "/") return T_DIV;
    if (s == ",") return T_COMMA;
    if (s == "'") return T_SQUOTE;
    if (s == "\"") return T_DQUOTE;
    if (s == "def") return T_DEF;
    if (s == "extern") return T_EXTERN;
    if (s == "end") return T_END;
    if (s == "return") return T_RETURN;
    if (s == " ") return T_BLANK;
    if (s == "\\") return T_ESCAPE;

    for (int i = 0; i < s.size(); ++i) {
        if (!(s[i] >= '0' && s[i] <= '9'))
            break;
        if (i == s.size() - 1) return T_NUMBER;
    }
    return T_IDENTIFIER;
}
