#include "regex.h"
#include <list>

static regex_component* regex_component_from_ast(ast_re_expr *expr) {
    switch (expr->get_ast_re_expr_type()) {
        case AST_RE_CHAR: {
                ast_re_char *rexpr = (ast_re_char*)expr;
                return new regex_char(rexpr->value);
            }
        case AST_RE_EPSILON: {
                ast_re_epsilon *rexpr = (ast_re_epsilon*)expr;
                return new regex_epsilon();
            }
        case AST_RE_OR_LIST: {
                ast_re_or_list *rexpr = (ast_re_or_list*)expr;
                std::list<ast_re_expr*> &exprs = rexpr->exprs;
                int cnt = rexpr->exprs.size();
                assert(cnt >= 2);

                std::list<ast_re_expr*>::const_reverse_iterator it = exprs.rbegin();
                ast_re_expr* last = *it++;
                ast_re_expr* last2 = *it++;

                regex_or *res = new regex_or(regex_component_from_ast(last2),
                                             regex_component_from_ast(last));

                while (it != exprs.rend()) {
                    res = new regex_or(regex_component_from_ast(*it++), res);
                }

                return res;
            }
        case AST_RE_CONCAT_LIST: {
                ast_re_concat_list *rexpr = (ast_re_concat_list*)expr;
                std::list<ast_re_expr*> &exprs = rexpr->exprs;
                int cnt = rexpr->exprs.size();
                assert(cnt >= 2);

                std::list<ast_re_expr*>::const_reverse_iterator it = exprs.rbegin();
                ast_re_expr* last = *it++;
                ast_re_expr* last2 = *it++;

                regex_concat *res = new regex_concat(regex_component_from_ast(last2),
                                                     regex_component_from_ast(last));

                while (it != exprs.rend()) {
                    res = new regex_concat(regex_component_from_ast(*it++), res);
                }

                return res;
            }
        case AST_RE_PLUS: {
                ast_re_plus *rexpr = (ast_re_plus*)expr;
                return new regex_plus(regex_component_from_ast(rexpr->expr));
            }
        case AST_RE_STAR: {
                ast_re_star *rexpr = (ast_re_star*)expr;
                return new regex_star(regex_component_from_ast(rexpr->expr));
            }
        case AST_RE_MAYBE: {
                ast_re_maybe *rexpr = (ast_re_maybe*)expr;
                return new regex_maybe(regex_component_from_ast(rexpr->expr));
            }
    }

    assert(false);
}

regex* regex_from_ast(ast_regex *m) {
    return new regex(regex_component_from_ast(m->expr));
}

