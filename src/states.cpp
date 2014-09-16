#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <list>
#include <queue>
#include <cstring>
#include <cassert>
#include "utils.h"
#include "nfa.h"
#include "regex.h"
#include "machine.h"
#include "ast.h"
#include "converters.h"
using namespace std;

extern int yyparse();
extern FILE *yyin;
extern ast_program *main_program;

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "usage: states OP FILE\n");
        return 1;
    }

    yyin = fopen(argv[2], "r");

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

    if (strcmp(argv[1], "grep") == 0) {

        string line;
        while (getline(cin, line)) {
            DEBUGMSG("read '%s'\n", line.c_str());

            if (m->accepts(line)) {
                printf("%s\n", line.c_str());
            }
        }

    } else if (strcmp(argv[1], "nfa2regex") == 0) {

        if (main_program->machine->get_ast_machine_type() != AST_MACHINE_NFA) {
            fprintf(stderr, "error: input must be an nfa\n");
            return 1;
        }

        regex *res = nfa2regex((nfa*)m);
        cout << res->to_string() << endl;
        delete res;

    } else {

        fprintf(stderr, "error: unsupported op '%s'\n", argv[1]);
        return 1;

    }

    return 0;
}

