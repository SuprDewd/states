#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <list>
#include <queue>
#include <cassert>
#include "utils.h"
#include "nfa.h"
#include "ast.h"
using namespace std;

extern int yyparse();
extern ast_program *main_program;

int main() {

    if (yyparse()) {
        // something went wrong
        exit(1);
    }

    // main_program->print(cout);

    nfa *S = nfa_from_ast(main_program->nfa);

    for (nfa::iterator it = S->begin(); it != S->end(); ++it) {
        cout << *it << endl;
    }

    // int len = 0, cnt = 0;
    // for (nfa::iterator it = S->begin(); it != S->end(); ++it) {
    //     if (size(*it) == len) {
    //         cnt++;
    //     } else {
    //         while (len != size(*it)) {
    //             printf("%d %d\n", len, cnt);
    //             len++;
    //             cnt = 0;
    //         }

    //         cnt = 1;
    //     }
    // }

    // while (true) {
    //     string line;
    //     getline(cin, line);

    //     if (S->accepts(line)) {
    //         printf("%s\n", line.c_str());
    //         // printf("YES\n");
    //     } else {
    //         // printf("NO\n");
    //     }
    // }

    return 0;
}

