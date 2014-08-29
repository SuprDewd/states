#ifndef AST_H
#define AST_H

#include <list>
#include <set>
#include <string>
#include <iostream>
#include <cassert>

#define IND1 "    "
#define IND2 IND1 IND1
#define IND3 IND2 IND1
#define IND4 IND3 IND1

class ast_state;
class ast_on;

typedef std::list<ast_state*> ast_statelist;
typedef std::list<ast_on*> ast_onlist;

class ast_node {
    virtual std::ostream& print(std::ostream &outs) = 0;
};

class ast_on: public ast_node {
public:
    std::string token;
    std::string target_state;

    ast_on(std::string token, std::string target_state) {
        this->token = token;
        this->target_state = target_state;
    }

    std::ostream& print(std::ostream &outs) {
        return outs << IND2 << "on " << this->token << " " << this->target_state << ";" << std::endl;
    }
};

class ast_state: public ast_node {
public:
    std::string name;
    ast_onlist next;
    bool accept;

    ast_state(std::string name, bool accept, ast_onlist next) {
        this->name = name;
        this->accept = accept;
        this->next = next;
    }

    std::ostream& print(std::ostream &outs) {
        outs << IND1 << (this->accept ? "accept " : "") << "state " << this->name << " {" << std::endl;
        for (ast_onlist::const_iterator it = this->next.begin(); it != this->next.end(); ++it) {
            (*it)->print(outs);
        }
        return outs << IND1 << "}" << std::endl;
    }
};

enum ast_machine_type { AST_MACHINE_NFA, AST_MACHINE_DFA, AST_MACHINE_REGEX };

/* An abstract class representing any pattern-matching machine, such as NFA,
 * DFA or a regular expression */
class ast_machine: public ast_node {
public:

    virtual ast_machine_type get_ast_machine_type() = 0;

    virtual std::ostream& print(std::ostream &outs) = 0;
};

class ast_nfa: public ast_machine {
public:
    std::string start;
    ast_statelist states;

    ast_nfa(std::string start, ast_statelist states) {
        this->start = start;
        this->states = states;
    }

    ast_machine_type get_ast_machine_type() {
        return AST_MACHINE_NFA;
    }

    std::ostream& print(std::ostream &outs) {
        outs << "nfa " << this->start << " {" << std::endl;
        for (ast_statelist::const_iterator it = this->states.begin(); it != this->states.end(); ++it) {
            outs << std::endl;
            (*it)->print(outs);
        }
        return outs << "}" << std::endl;
    }
};

enum ast_re_expr_type { AST_RE_CHAR, AST_RE_OR_LIST, AST_RE_CONCAT_LIST, AST_RE_PLUS, AST_RE_MAYBE, AST_RE_STAR };

class ast_re_expr: public ast_node {
public:

    virtual ast_re_expr_type get_ast_re_expr_type() = 0;

    virtual std::ostream& print(std::ostream &outs) = 0;
};

class ast_re_char: public ast_re_expr {
public:

    char value;
    ast_re_char(char value) {
        this->value = value;
    }

    ast_re_expr_type get_ast_re_expr_type() {
        return AST_RE_CHAR;
    }

    std::ostream& print(std::ostream &outs) {
        return outs << this->value;
    }
};

class ast_re_or_list: public ast_re_expr {
public:

    std::list<ast_re_expr*> exprs;
    ast_re_or_list() {
    }

    ast_re_expr_type get_ast_re_expr_type() {
        return AST_RE_OR_LIST;
    }

    void push_front(ast_re_expr *expr) {
        this->exprs.push_front(expr);
    }

    std::ostream& print(std::ostream &outs) {
        bool first = true;
        for (std::list<ast_re_expr*>::const_iterator it = this->exprs.begin(); it != this->exprs.end(); ++it) {
            if (first) first = false;
            else outs << "|";

            outs << "(";
            (*it)->print(outs);
            outs << ")";
        }
        return outs;
    }
};

class ast_re_concat_list: public ast_re_expr {
public:

    std::list<ast_re_expr*> exprs;
    ast_re_concat_list() {
    }

    ast_re_expr_type get_ast_re_expr_type() {
        return AST_RE_CONCAT_LIST;
    }

    void push_front(ast_re_expr *expr) {
        this->exprs.push_front(expr);
    }

    std::ostream& print(std::ostream &outs) {
        for (std::list<ast_re_expr*>::const_iterator it = this->exprs.begin(); it != this->exprs.end(); ++it) {
            outs << "(";
            (*it)->print(outs);
            outs << ")";
        }
        return outs;
    }
};

class ast_re_plus: public ast_re_expr {
public:
    ast_re_expr *expr;

    ast_re_plus(ast_re_expr *expr) {
        this->expr = expr;
    }

    ast_re_expr_type get_ast_re_expr_type() {
        return AST_RE_PLUS;
    }

    std::ostream& print(std::ostream &outs) {
        outs << "(";
        this->expr->print(outs);
        return outs << ")+";
    }
};

class ast_re_star: public ast_re_expr {
public:
    ast_re_expr *expr;

    ast_re_star(ast_re_expr *expr) {
        this->expr = expr;
    }

    ast_re_expr_type get_ast_re_expr_type() {
        return AST_RE_STAR;
    }

    std::ostream& print(std::ostream &outs) {
        outs << "(";
        this->expr->print(outs);
        return outs << ")*";
    }
};

class ast_re_maybe: public ast_re_expr {
public:
    ast_re_expr *expr;

    ast_re_maybe(ast_re_expr *expr) {
        this->expr = expr;
    }

    ast_re_expr_type get_ast_re_expr_type() {
        return AST_RE_MAYBE;
    }

    std::ostream& print(std::ostream &outs) {
        outs << "(";
        this->expr->print(outs);
        return outs << ")?";
    }
};

class ast_regex: public ast_machine {
public:
    ast_re_expr *expr;

    ast_regex(ast_re_expr *expr) {
        this->expr = expr;
    }

    ast_machine_type get_ast_machine_type() {
        return AST_MACHINE_REGEX;
    }

    std::ostream& print(std::ostream &outs) {
        outs << "regex ";
        this->expr->print(outs);
        return outs << std::endl;
    }
};

class ast_alphabet: public ast_node {
public:
    std::list<char> alph;

    ast_alphabet() {
    }

    void add_front(char c) {
        this->alph.push_front(c);
    }

    void add_range_front(char a, char b) {
        if (a < b) {
            char t = a;
            a = b;
            b = t;
        }

        while (true) {
            this->alph.push_front(a);

            if (a == b) {
                break;
            }

            a--;
        }
    }

    std::ostream& print(std::ostream &outs) {
        assert(this->alph.size() > 0);

        outs << "alphabet";
        std::set<char> v(this->alph.begin(), this->alph.end());

        std::set<char>::const_iterator it = v.begin();
        char start = *it++,
             end = start;

        while (it != v.end()) {
            char cur = *it++;
            if (end + 1 == cur) {
                end = cur;
            } else {
                if (start == end) {
                    outs << " " << start;
                } else {
                    outs << " " << start << "-" << end;
                }

                start = end = cur;
            }
        }

        if (start == end) {
            outs << " " << start;
        } else {
            outs << " " << start << "-" << end;
        }

        return outs << ";" << std::endl;
    }
};

class ast_program: public ast_node {
public:

    ast_alphabet *alph;
    ast_machine *machine;

    ast_program(ast_alphabet *alph, ast_machine *machine) {
        this->alph = alph;
        this->machine = machine;
    }

    std::ostream& print(std::ostream &outs) {
        outs << std::endl;
        this->alph->print(outs);
        outs << std::endl;
        this->machine->print(outs);
        return outs << std::endl;
    }
};

#endif
