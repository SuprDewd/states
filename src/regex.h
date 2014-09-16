#ifndef REGEX_H
#define REGEX_H

#include "ast.h"
#include "machine.h"
#include "utils.h"
#include <map>
#include <string>

class regex_component;

typedef std::map<std::pair<regex_component*, std::pair<int, int> >, bool> state_memory;

class regex_component {
public:

    virtual bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) = 0;

    bool accepts(state_memory &mem, const std::string &s, int start, int len) {
        std::pair<regex_component*, std::pair<int, int> > state(this, std::pair<int, int>(start, len));
        if (mem.find(state) == mem.end()) {
            return mem[state] = this->accepts_raw(mem, s, start, len);
        } else {
            return mem[state];
        }
    }

    virtual std::string to_string() = 0;
};

class regex: public machine {
public:

    regex_component *expr;

    regex(regex_component *expr) {
        this->expr = expr;
    }

    machine_type get_machine_type() {
        return MACHINE_REGEX;
    }

    bool accepts(const std::string &s) {
        DEBUGMSG("regex starting '%s'\n", s.c_str());
        state_memory mem;
        bool res = this->expr->accepts(mem, s, 0, s.size());
        DEBUGMSG("regex starting '%s': %s\n", s.c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        return this->expr->to_string();
    }
};

class regex_star: public regex_component {
public:

    regex_component *expr;

    regex_star(regex_component *expr) {
        this->expr = expr;
    }

    bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) {
        DEBUGMSG("regex star '%s'\n", s.substr(start, len).c_str());

        bool res = false;
        if (len == 0) {
            res = true;
        } else {
            for (int l = 1; l <= len; l++) {
                if (this->expr->accepts(mem, s, start, l) &&
                    this->accepts(mem, s, start + l, len - l)) {
                    res = true;
                    break;
                }
            }
        }

        DEBUGMSG("regex star '%s': %s\n", s.substr(start, len).c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        return "(" + this->expr->to_string() + ")*";
    }
};

class regex_plus: public regex_component {
public:

    regex_component *expr;

    regex_plus(regex_component *expr) {
        this->expr = expr;
    }

    bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) {
        DEBUGMSG("regex plus '%s'\n", s.substr(start, len).c_str());

        bool res = false;
        if (this->expr->accepts(mem, s, start, len)) {
            res = true;
        } else {
            for (int l = 1; l <= len - 1; l++) {
                if (this->expr->accepts(mem, s, start, l) &&
                    this->accepts(mem, s, start + l, len - l)) {
                    res = true;
                    break;
                }
            }
        }

        DEBUGMSG("regex plus '%s': %s\n", s.substr(start, len).c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        return "(" + this->expr->to_string() + ")+";
    }
};

class regex_maybe: public regex_component {
public:

    regex_component *expr;

    regex_maybe(regex_component *expr) {
        this->expr = expr;
    }

    bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) {
        DEBUGMSG("regex maybe '%s'\n", s.substr(start, len).c_str());

        bool res = len == 0 || this->expr->accepts(mem, s, start, len);

        DEBUGMSG("regex maybe '%s': %s\n", s.substr(start, len).c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        return "(" + this->expr->to_string() + ")?";
    }
};

class regex_or: public regex_component {
public:

    regex_component *option1, *option2;

    regex_or(regex_component *option1, regex_component *option2) {
        this->option1 = option1;
        this->option2 = option2;
    }

    bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) {
        DEBUGMSG("regex or '%s'\n", s.substr(start, len).c_str());

        bool res = this->option1->accepts(mem, s, start, len) ||
                   this->option2->accepts(mem, s, start, len);

        DEBUGMSG("regex or '%s': %s\n", s.substr(start, len).c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        return "(" + this->option1->to_string() + ")|(" + this->option2->to_string() + ")";
    }
};

class regex_concat: public regex_component {
public:

    regex_component *left, *right;

    regex_concat(regex_component *left, regex_component *right) {
        this->left = left;
        this->right = right;
    }

    bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) {
        DEBUGMSG("regex concat '%s'\n", s.substr(start, len).c_str());

        bool res = false;
        for (int l = 0; l <= len; l++) {
            if (this->left->accepts(mem, s, start, l) &&
                this->right->accepts(mem, s, start + l, len - l)) {
                res = true;
                break;
            }
        }

        DEBUGMSG("regex concat '%s': %s\n", s.substr(start, len).c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        return "(" + this->left->to_string() + ")(" + this->right->to_string() + ")";
    }
};

class regex_char: public regex_component {
public:

    char value;
    regex_char(char value) {
        this->value = value;
    }

    bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) {
        DEBUGMSG("regex char '%s'\n", s.substr(start, len).c_str());

        bool res = len == 1 && s[start] == this->value;

        DEBUGMSG("regex char '%s': %s\n", s.substr(start, len).c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        return std::string("") + this->value;
    }
};

class regex_epsilon: public regex_component {
public:

    regex_epsilon() {
    }

    bool accepts_raw(state_memory &mem, const std::string &s, int start, int len) {
        DEBUGMSG("regex epsilon '%s'\n", s.substr(start, len).c_str());

        bool res = len == 0;

        DEBUGMSG("regex epsilon '%s': %s\n", s.substr(start, len).c_str(), res ? "true" : "false");
        return res;
    }

    std::string to_string() {
        // TODO: make this parseable
        return std::string("epsilon");
    }
};

regex* regex_from_ast(ast_regex *m);

#endif
