#include <iostream>
#include "Token.h"
#include "Parse.h"
using namespace std;

int main() {
    Scan ltest;
    string s;
    cout << "input the code file: ";
    cin >> s;
    ltest.srcfile = s;
    ltest.outfile = "/Users/TanGreen/ClionProjects/Woo/Debug/out.txt";
    ltest.getToken();
    Parse ptest(ltest.list_token, "/Users/TanGreen/ClionProjects/Woo/Debug/asm.txt",
                "/Users/TanGreen/ClionProjects/Woo/Debug/main.txt");
    ptest.test();
    return 0;
}

