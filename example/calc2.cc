//
//  calc2.cc
//
//  Copyright (c) 2015 Yuji Hirose. All rights reserved.
//  MIT License
//

#include <peglib.h>
#include <iostream>
#include <cstdlib>

using namespace peglib;
using namespace std;

//
//  PEG syntax:
//
//      EXPRESSION       <-  TERM (TERM_OPERATOR TERM)*
//      TERM             <-  FACTOR (FACTOR_OPERATOR FACTOR)*
//      FACTOR           <-  NUMBER / '(' EXPRESSION ')'
//      TERM_OPERATOR    <-  [-+]
//      FACTOR_OPERATOR  <-  [/*]
//      NUMBER           <-  [0-9]+
//
int main(int argc, const char** argv)
{
    if (argc < 2 || string("--help") == argv[1]) {
        cout << "usage: calc [formula]" << endl;
        return 1;
    }

    const char* s = argv[1];

    auto reduce = [](const SemanticValues& sv) -> long {
        auto result = sv[0].val.get<long>();
        for (auto i = 1u; i < sv.size(); i += 2) {
            auto num = sv[i + 1].val.get<long>();
            auto ope = sv[i].val.get<char>();
            switch (ope) {
                case '+': result += num; break;
                case '-': result -= num; break;
                case '*': result *= num; break;
                case '/': result /= num; break;
            }
        }
        return result;
    };

    rule EXPRESSION, TERM, FACTOR, TERM_OPERATOR, FACTOR_OPERATOR, NUMBER;

    EXPRESSION      <= seq(TERM, zom(seq(TERM_OPERATOR, TERM))),         reduce;
    TERM            <= seq(FACTOR, zom(seq(FACTOR_OPERATOR, FACTOR))),   reduce;
    FACTOR          <= cho(NUMBER, seq(chr('('), EXPRESSION, chr(')')));
    TERM_OPERATOR   <= cls("+-"),                                        [](const char* s, size_t l) { return (char)*s; };
    FACTOR_OPERATOR <= cls("*/"),                                        [](const char* s, size_t l) { return (char)*s; };
    NUMBER          <= oom(cls("0-9")),                                  [](const char* s, size_t l) { return atol(s); };

    long val = 0;
    if (EXPRESSION.parse_with_value(s, val).ret) {
        cout << s << " = " << val << endl;
        return 0;
    }

    return -1;
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix