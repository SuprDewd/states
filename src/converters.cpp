#include "converters.h"

class gnfa {
public:
    string start_state;
    string end_state;
    map<string, list<pair<regex_component*, string> > > edges;

    gnfa(string start_state, string end_state) {
        this->start_state = start_state;
        this->end_state = end_state;
    }
};

gnfa* nfa2gnfa(nfa *m) {
    gnfa *g = new gnfa("gstart", "gend"); // TODO: choose start and end names so that they don't collide with anything

    for (map<string, map<char, set<string> > >::const_iterator from = m->next_state.begin(); from != m->next_state.end(); ++from) {
        // from->first is the starting vertex

        assert(from->first != g->start_state);
        assert(from->first != g->end_state);

        for (map<char, set<string> >::const_iterator label = from->second.begin(); label != from->second.end(); ++label) {
            // label->first is the edge label

            for (set<string>::const_iterator to = label->second.begin(); to != label->second.end(); ++to) {
                // *to is the target vertex

                assert(*to != g->start_state);
                assert(*to != g->end_state);

                g->edges[from->first].push_back(make_pair(new regex_char(label->first), *to));
            }
        }
    }

    assert(m->start_state != g->start_state);
    assert(m->start_state != g->end_state);

    g->edges[g->start_state].push_back(make_pair(new regex_epsilon(), m->start_state));

    for (set<string>::const_iterator finals = m->final_states.begin(); finals != m->final_states.end(); ++finals) {
        assert(*finals != g->start_state);
        assert(*finals != g->end_state);

        g->edges[*finals].push_back(make_pair(new regex_epsilon(), g->end_state));
    }

    return g;
}

regex* nfa2regex(nfa *m) {
    // TODO: Make this method output shorter regexps

    gnfa *g = nfa2gnfa(m);

    set<string> states;
    set<string> states_left;
    for (map<string, list<pair<regex_component*, string> > >::const_iterator from = g->edges.begin(); from != g->edges.end(); ++from) {
        states_left.insert(from->first);
        if (from->first != g->start_state && from->first != g->end_state) {
            states.insert(from->first);
        }
        for (list<pair<regex_component*, string> >::const_iterator edge = from->second.begin(); edge != from->second.end(); ++edge) {
            states_left.insert(edge->second);
            if (edge->second != g->start_state && edge->second != g->end_state) {
                states.insert(edge->second);
            }
        }
    }

    for (set<string>::const_iterator kill = states.begin(); kill != states.end(); ++kill) {

        states_left.erase(*kill);

        vector<regex_component*> loops;
        for (list<pair<regex_component*, string> >::const_iterator edge = g->edges[*kill].begin(); edge != g->edges[*kill].end(); ++edge) {
            if (edge->second == *kill) {
                loops.push_back(edge->first);
            }
        }

        regex_component *loop = NULL;
        if (size(loops) == 1) {
            loop = loops[0];
        } else if (size(loops) > 1) {
            loop = loops[0];
            for (int i = 1; i < size(loops); i++) {
                loop = new regex_or(loop, loops[i]);
            }
        }

        for (set<string>::const_iterator from = states_left.begin(); from != states_left.end(); ++from) {
            for (list<pair<regex_component*, string> >::const_iterator edge1 = g->edges[*from].begin(); edge1 != g->edges[*from].end(); ++edge1) {
                if (edge1->second == *kill) {
                    for (list<pair<regex_component*, string> >::const_iterator edge2 = g->edges[*kill].begin(); edge2 != g->edges[*kill].end(); ++edge2) {
                        if (states_left.find(edge2->second) != states_left.end()) {

                            regex_component *nr = edge1->first;
                            if (loop != NULL) {
                                nr = new regex_concat(nr, new regex_star(loop));
                            }

                            nr = new regex_concat(nr, edge2->first);
                            g->edges[*from].push_back(make_pair(nr, edge2->second));
                        }
                    }
                }
            }
        }

        g->edges.erase(*kill);

        for (map<string, list<pair<regex_component*, string> > >::iterator from = g->edges.begin(); from != g->edges.end(); ++from) {
            for (list<pair<regex_component*, string> >::iterator edge = from->second.begin(); edge != from->second.end(); ) {
                if (edge->second == *kill) {
                    list<pair<regex_component*, string> >::iterator tmp = edge;
                    ++edge;

                    from->second.erase(tmp);

                } else {
                    ++edge;
                }
            }
        }
    }

    regex_component *res = NULL;

    for (list<pair<regex_component*, string> >::const_iterator rest = g->edges[g->start_state].begin(); rest != g->edges[g->start_state].end(); ++rest) {

        assert(rest->second == g->end_state);

        if (res == NULL) {
            res = rest->first;
        } else {
            res = new regex_or(res, rest->first);
        }
    }

    delete g;

    return new regex(res);
}

