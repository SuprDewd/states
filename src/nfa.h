#ifndef NFA_H
#define NFA_H

#include <string>
#include <map>
#include <set>
#include <string>
#include <list>
#include <queue>
#include <cassert>
#include "utils.h"
#include "ast.h"
#include "machine.h"
using namespace std;

class nfa: public machine {
public:
    string start_state;
    set<string> final_states;
    map<string, map<char, set<string> > > next_state;

    machine_type get_machine_type() {
        return MACHINE_NFA;
    }

    bool accepts(const string &s) {
        return this->accepts_rec(this->start_state, s, 0);
    }

    void add_next(const string &cur_state, char token, const string &nxt_state) {
        this->next_state[cur_state][token].insert(nxt_state);
    }

    void add_final(const string &state) {
        this->final_states.insert(state);
    }

    bool is_final(const string &state) {
        return this->final_states.find(state) != this->final_states.end();
    }

    void set_start(string state) {
        this->start_state = state;
    }

    class iterator {
    public:
        iterator(nfa *s, queue<pair<string, set<string> > > q) {
            this->s = s;
            this->q = q;
            this->find_next_accept(false);
        }

        bool operator ==(const iterator &other) {
            return this->s == other.s && this->q == other.q;
        }

        bool operator !=(const iterator &other) {
            return !(*this == other);
        }

        void operator ++() {
            assert(!this->q.empty());
            this->find_next_accept(true);
        }

        string operator *() {
            assert(!this->q.empty());
            return this->q.front().first;
        }

    private:
        nfa *s;
        queue<pair<string, set<string> > > q;

        void find_next_accept(bool ignore_head) {

            DEBUGMSG("finding next accept state\n");

            while (!this->q.empty()) {
                pair<string, set<string> > cur = this->q.front();

                #if DEBUG
                DEBUGMSG("now at %s: ", cur.first.c_str());
                for (set<string>::const_iterator it = cur.second.begin(); it != cur.second.end(); ++it) {
                    DEBUGMSG("%s(%d), ", it->c_str(), this->s->is_final(*it));
                }
                DEBUGMSG("\n");
                #endif

                if (!ignore_head) {
                    for (set<string>::const_iterator it = cur.second.begin(); it != cur.second.end(); ++it) {
                        if (this->s->is_final(*it)) {
                            return;
                        }
                    }
                }

                ignore_head = false;

                this->q.pop();

                set<char> nexts;
                for (set<string>::const_iterator it = cur.second.begin(); it != cur.second.end(); it++) {
                    map<string, map<char, set<string> > >::const_iterator x = this->s->next_state.find(*it);
                    if (x == this->s->next_state.end()) {
                        continue;
                    }

                    for (map<char, set<string> >::const_iterator jt = x->second.begin(); jt != x->second.end(); jt++) {
                        nexts.insert(jt->first);
                    }
                }

                for (set<char>::const_iterator nxt = nexts.begin(); nxt != nexts.end(); ++nxt) {
                    set<string> nxt_states;
                    for (set<string>::const_iterator cur_state = cur.second.begin(); cur_state != cur.second.end(); ++cur_state) {
                        map<string, map<char, set<string> > >::const_iterator cur_state_it = this->s->next_state.find(*cur_state);
                        if (cur_state_it == this->s->next_state.end()) {
                            continue;
                        }

                        map<char, set<string> >::const_iterator nxt_states_it = cur_state_it->second.find(*nxt);
                        if (nxt_states_it == cur_state_it->second.end()) {
                            continue;
                        }

                        for (set<string>::const_iterator nxt_state = nxt_states_it->second.begin(); nxt_state != nxt_states_it->second.end(); ++nxt_state) {
                            nxt_states.insert(*nxt_state);
                        }
                    }

                    if (size(nxt_states) > 0) {
                        this->q.push(make_pair(cur.first + *nxt, nxt_states));
                    }
                }
            }
        }
    };

    iterator begin() {
        set<string> s;
        s.insert(this->start_state);
        queue<pair<string, set<string> > > q;
        q.push(make_pair("", s));
        return iterator(this, q);
    }

    iterator end() {
        queue<pair<string, set<string> > > q;
        return iterator(this, q);
    }

private:
    bool accepts_rec(const string &state, const string &s, int at) {
        if (at == size(s)) {
            return this->final_states.find(state) != this->final_states.end();
        }

        map<string, map<char, set<string> > >::iterator jt = this->next_state.find(state);
        if (jt == this->next_state.end()) {
            return false;
        }

        map<char, set<string> >::iterator it = jt->second.find(s[at]);
        if (it == jt->second.end()) {
            return false;
        }

        for (set<string>::const_iterator nxt_state = it->second.begin(); nxt_state != it->second.end(); nxt_state++) {
            if (this->accepts_rec(*nxt_state, s, at + 1)) {
                return true;
            }
        }

        return false;
    }
};

nfa *nfa_from_ast(ast_nfa *anfa);

#endif
