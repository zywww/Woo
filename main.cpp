#include <iostream>
#include "Token.h"
#include "Parse.h"
using namespace std;

int main() {
    Scan ltest;
    ltest.srcfile = "/Users/TanGreen/ClionProjects/Woo/Debug/main.txt";
    ltest.outfile = "/Users/TanGreen/ClionProjects/Woo/Debug/out.txt";
    ltest.getToken();
    Parse ptest(ltest.list_token, "/Users/TanGreen/ClionProjects/Woo/Debug/asm.txt");
    ptest.test();
    return 0;
}

