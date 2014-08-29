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
#include "regex.h"
#include "machine.h"
#include "ast.h"
using namespace std;

extern int yyparse();
extern FILE *yyin;
extern ast_program *main_program;

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: states FILE\n");
        return 1;
    }

    yyin = fopen(argv[1], "r");

    if (yyparse()) {
        // something went wrong
        exit(1);
    }

#if DEBUG
    main_program->print(cout);
#endif

    machine *m;

    switch (main_program->machine->get_ast_machine_type()) {
        case AST_MACHINE_NFA: m = nfa_from_ast((ast_nfa*)main_program->machine); break;
        case AST_MACHINE_REGEX: m = regex_from_ast((ast_regex*)main_program->machine); break;
        default: assert(false);
    }

    // nfa *S = nfa_from_ast(main_program->nfa);

    // for (nfa::iterator it = S->begin(); it != S->end(); ++it) {
    //     cout << *it << endl;
    // }

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

    string line;
    while (getline(cin, line)) {
        DEBUGMSG("read '%s'\n", line.c_str());

        if (m->accepts(line)) {
            printf("%s\n", line.c_str());
        }
    }

    return 0;
}

