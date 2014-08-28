alphabet 0 1;

nfa maybe {

    state maybe {
        on 0 yes;
        on 1 maybe;
    }

    accept state yes {
        on 0 yes;
        on 1 maybe;
    }
}
