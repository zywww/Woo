#include <iostream>
#include "Token.h"
#include "Parse.h"
using namespace std;

int main() {
    Scan ltest;
    ltest.getToken();
    Parse ptest(ltest.list_token);
    ptest.test();
    return 0;
}

