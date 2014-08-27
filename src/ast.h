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

class ast_state {
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

class ast_nfa {
public:
    std::string start;
    ast_statelist states;

    ast_nfa(std::string start, ast_statelist states) {
        this->start = start;
        this->states = states;
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

class ast_alphabet {
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

class ast_program {
public:

    ast_alphabet *alph;
    ast_nfa *nfa;

    ast_program(ast_alphabet *alph, ast_nfa *nfa) {
        this->alph = alph;
        this->nfa = nfa;
    }

    std::ostream& print(std::ostream &outs) {
        outs << std::endl;
        this->alph->print(outs);
        outs << std::endl;
        this->nfa->print(outs);
        return outs << std::endl;
    }
};

#endif
