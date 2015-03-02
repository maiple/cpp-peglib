//
//  calc.cc
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
//      EXPRESSION       <-  _ TERM (TERM_OPERATOR TERM)*
//      TERM             <-  FACTOR (FACTOR_OPERATOR FACTOR)*
//      FACTOR           <-  NUMBER / '(' _ EXPRESSION ')' _
//      TERM_OPERATOR    <-  < [-+] > _
//      FACTOR_OPERATOR  <-  < [/*] > _
//      NUMBER           <-  < [0-9]+ > _
//      ~_               <-  [ \t\r\n]*
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

    const char* syntax =
        "  EXPRESSION       <-  _ TERM (TERM_OPERATOR TERM)*      "
        "  TERM             <-  FACTOR (FACTOR_OPERATOR FACTOR)*  "
        "  FACTOR           <-  NUMBER / '(' _ EXPRESSION ')' _   "
        "  TERM_OPERATOR    <-  < [-+] > _                        "
        "  FACTOR_OPERATOR  <-  < [/*] > _                        "
        "  NUMBER           <-  < [0-9]+ > _                      "
        "  ~_               <-  [ \t\r\n]*                        "
        ;

    peg parser(syntax);

    parser["EXPRESSION"]      = reduce;
    parser["TERM"]            = reduce;
    parser["TERM_OPERATOR"]   = [](const char* s, size_t l) { return (char)*s; };
    parser["FACTOR_OPERATOR"] = [](const char* s, size_t l) { return (char)*s; };
    parser["NUMBER"]          = [](const char* s, size_t l) { return atol(s); };

    long val = 0;
    if (parser.parse(s, val)) {
        cout << s << " = " << val << endl;
        return 0;
    }

    cout << "syntax error..." << endl;

    return -1;
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix