#include "nfa.h"

nfa* nfa_from_ast(ast_nfa *m) {
    nfa *res = new nfa();
    res->set_start(m->start);

    for (ast_statelist::const_iterator it = m->states.begin(); it != m->states.end(); ++it) {
        ast_state *state = *it;

        if (state->accept) {
            res->add_final(state->name);
        }

        for (ast_onlist::const_iterator jt = state->next.begin(); jt != state->next.end(); ++jt) {
            ast_on *on = *jt;

            char t;
            if (on->token.size() == 1) {
                t = on->token[0];
            } else if (on->token == "epsilon") {
                t = 0;
            } else {
                assert(false);
            }

            res->add_next(state->name, t, on->target_state);
        }
    }

    return res;
}

